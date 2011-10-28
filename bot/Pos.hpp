#ifndef POS_HPP
#define POS_HPP

#include <vector>
#include <set>
#include <ostream>
#include <cstdlib>
#include "Assert.hpp"

/*
	struct for representing positions in the grid.
*/
struct Pos
{
	int coords[2];

	Pos() {
		coords[0] = coords[1] = 0;
	}

	Pos(int r, int c) {
		coords[0] = r;
		coords[1] = c;
	}

	inline int& operator[] (int const& i)       { return coords[i]; }
	inline int  operator[] (int const& i) const { return coords[i]; }

	inline int& x() { return coords[0]; }
	inline int x() const { return coords[0]; }

	inline int& y() { return coords[1]; }
	inline int y() const { return coords[1]; }
};

typedef Pos Vec2;

typedef std::set<Pos> PosSet;
typedef std::vector<Pos> PosList;

inline bool operator < (Pos const& a, Pos const& b) {
	return (a[0] < b[0])||((a[0] == b[0]) && (a[1] < b[1]));
}

inline bool operator == (Pos const& a, Pos const& b) {
	return a.x() == b.x() && a.y() == b.y();
}

inline std::ostream& operator<<(std::ostream &os, const Pos& pos) {
	return os << "(" << pos[0] << ", " << pos[1] << ")";
}

//////////////////////////////////////////////////////

// Wrapped positive distance between points a,b both on [0,size)
inline int wrappedAbsDist(int a, int b, int size) {
	int d = std::abs(a-b);
	return std::min(d, size-d);
}

// Signed distance from a to b - like b-a, but wrapped. Ansewer in [size/2, size/2]
inline int wrappedSignedDist(int a, int b, int size) {
	int d = b-a;
	while (d < -size/2) d += size;
	while (d > +size/2) d -= size;
	return d;
}

// Bounding box
class BB {
public:
	/* remember that m_min.x CAN be greater than m_max.x
	   if the room is wrapped! */
	Pos m_min, m_max; // [,] style ranges! INCLUSIVE!

	int sizeAxis(int a, int size) const {
		if (m_min[a] <= m_max[a]) {
			// Non-wrapped
			return m_max[a] - m_min[a] + 1;
		} else {
			return size + m_max[a] - m_min[a] - 1;
		}
	}

	Vec2 size(const Vec2& mapSize) const {
		return Vec2(
					sizeAxis(0, mapSize[0]),
					sizeAxis(1, mapSize[1])
					);
	}

	bool containsOnAxis(int a, int pos) const {
		if (m_min[a] <= m_max[a]) {
			// Non-wrapped
			return m_min[a] <= pos && pos <= m_max[a];
		} else {
			// Wrapped
			return pos <= m_max[a] || m_min[a] <= pos;
		}
	}

	// pos must be inside map!
	bool contains(const Pos& pos) const {
		for (int a=0; a<2; ++a)
			if (!containsOnAxis(a, pos[a]))
				return false;
		return true;
	}

	/* distance to boundingbox edge.
	   iff both returned coordinates are 0, contains(pos) == true.
	   size == size of map.
	*/
	Vec2 distance(const Pos& pos, const Vec2& size) const {
		Pos ret;
		for (int a=0; a<2; ++a) {
			if (containsOnAxis(a, pos[a])) {
				ret[a] = 0; // inside
				continue;
			} else {
				// Non-wrapped
				ret[a] = std::min(
							wrappedAbsDist(pos[a], m_min[a], size[a]),
							wrappedAbsDist(pos[a], m_max[a], size[a]));
			}
		}
		return ret;
	}

	// 0 iff inside
	int distanceManhattan(const Pos& pos, const Vec2& size) const {
		Vec2 r = this->distance(pos, size);
		ASSERT(r.x() >= 0);
		ASSERT(r.y() >= 0);
		return r.x() + r.y();
	}

	// "pos" is close (or inside) bounding box. Expand bounding box to include it.
	void expandTo(const Pos& pos, const Vec2& size) {
		for (int a=0; a<2; ++a) {
			if (containsOnAxis(a, pos[a]))
				continue;

			if (m_min[a] == m_max[a]) {
				// Special case: expand left or right?
				if (wrappedSignedDist(m_min[a], pos[a], size[a]) < 0)
					m_min[a] = pos[a];
				else
					m_max[a] = pos[a];
			}
			else {
				// Expand to whichever is closest:
				if (wrappedAbsDist(pos[a], m_min[a], size[a]) < wrappedAbsDist(pos[a], m_max[a], size[a]))
					m_min[a] = pos[a];
				else
					m_max[a] = pos[a];
			}
		}
	}
};

//////////////////////////////////////////////////////
// Bös

typedef std::vector<int> IntList;
typedef std::set<int> IntSet;

#endif //POS_H_
