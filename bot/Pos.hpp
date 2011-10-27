#ifndef POS_H_
#define POS_H_

#include <vector>
#include <set>
#include <ostream>
#include <cassert>

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

	inline int& row() { return coords[0]; }
	inline int row() const { return coords[0]; }

	inline int& col() { return coords[1]; }
	inline int col() const { return coords[1]; }
};

typedef Pos Vec2;

typedef std::set<Pos> PosSet;
typedef std::vector<Pos> PosList;

inline bool operator < (Pos const& a, Pos const& b) {
	return (a[0] < b[0])||((a[0] == b[0]) && (a[1] < b[1]));
}

inline std::ostream& operator<<(std::ostream &os, const Pos& pos) {
	return os << "(" << pos[0] << ", " << pos[1] << ")";
}

//////////////////////////////////////////////////////

// Wrapped postivie distance between points a,b both on [0,size)
inline int wrappedDist(int a, int b, int size) {
	int d = abs(a-b);
	return std::min(d, size-d);
}

// Bounding box
class BB {
public:
	/* remember that topLeft.x CAN be greater than bottomRight.x
	   if the room is wrapped! */
	Pos topLeft, bottomRight; // [,) style ranges

	/*
	wont work when wrapped...
	int width()  const { return bottomRight.x()-topLeft.x(); }
	int height() const { return bottomRight.y()-topLeft.y(); }
	*/

	// pos must be inside map!
	bool contains(const Pos& pos) const {
		for (int a=0; a<2; ++a) {
			if (topLeft[a] <= bottomRight[a]) {
				// Non-wrapped
				if (pos[a]<topLeft[a]) return false;
				if (pos[a]>=bottomRight[a]) return false;
			} else {
				// Wrapped
				if (bottomRight[a]<=pos[a] && pos[a]<topLeft[a])
					return false;
			}
		}
		return true;
	}

	/* distance to boundingbox edge.
	   iff both returned coordinates are 0, contains(pos) == true.
	   size == size of map.
	*/
	Vec2 distance(const Pos& pos, const Vec2& size) const {
		Pos ret;
		for (int a=0; a<2; ++a) {
			if (topLeft[a] <= bottomRight[a]) {
				// Non-wrapped
				if (topLeft[a] <= pos[a] && pos[a] < bottomRight[a])
					ret[a] = 0; // Inside
				else
					ret[a] = std::min(
								wrappedDist(pos[a], topLeft[a],       size[a]),
								wrappedDist(pos[a], bottomRight[a]-1, size[a])); // -1 for distance to inclusive
			} else {
				// Wrapped
				if (pos[a] < bottomRight[a] || pos[a] >= topLeft[a])
					ret[a] = 0; // inside
				else
					ret[a] = std::min(
								wrappedDist(pos[a], topLeft[a],       size[a]),
								wrappedDist(pos[a], bottomRight[a]-1, size[a])); // -1 for distance to inclusive
			}
		}
		return ret;
	}

	// 0 iff inside
	int distanceManhattan(const Pos& pos, const Vec2& size) const {
		Vec2 r = this->distance(pos, size);
		assert(r.x() >= 0);
		assert(r.y() >= 0);
		return r.x() + r.y();
	}
};

//////////////////////////////////////////////////////
// Bös

typedef std::vector<int> IntList;
typedef std::set<int> IntSet;

#endif //POS_H_
