#ifndef POS_HPP
#define POS_HPP

#include <vector>
#include <set>
#include <ostream>
#include <cstdlib>
#include "Assert.hpp"

template<typename T>
struct Vec2T
{
	Vec2T() {
		m_v[0] = m_v[1] = 0;
	}

	Vec2T(T x, T y) {
		m_v[0] = x;
		m_v[1] = y;
	}

	template<typename F>
	explicit Vec2T(const Vec2T<F>& v) {
		m_v[0] = (T)v.x();
		m_v[1] = (T)v.y();
	}

	inline T& operator[] (int const& i)       { ASSERT(i==0 || i==1); return m_v[i]; }
	inline T  operator[] (int const& i) const { ASSERT(i==0 || i==1); return m_v[i]; }

	inline T& x()       { return m_v[0]; }
	inline T  x() const { return m_v[0]; }

	inline T& y()       { return m_v[1]; }
	inline T  y() const { return m_v[1]; }

private:
	T m_v[2];
};

typedef Vec2T<int> Vec2;
typedef Vec2 Pos;
typedef Vec2T<float> Vec2f;

typedef std::set<Pos> PosSet;
typedef std::vector<Pos> PosList;
typedef std::vector<Pos> PosVec;

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

template<typename T>
T sqr(const T& x) { return x*x; }

template<typename T>
T Abs(const T& x) { return x<0 ? -x : +x; }

inline int sign(int v) {
	if (v<0) return -1;
	if (v>0) return +1;
	return 0;
}

// wrap to [0,size)
template<typename T>
T wrap(T x, int size) {
	while (x < 0)     x += size;
	while (x >= size) x -= size;
	return x;
}

// Wrapped positive distance between points a,b both on [0,size)
inline int wrappedAbsDist(int a, int b, int size) {
	int d = Abs(a-b);
	return std::min(d, size-d);
}

inline float wrappedAbsDist(float a, float b, int size) {
	float d = Abs(a-b);
	return std::min(d, size-d);
}

// Signed distance from a to b - like b-a, but wrapped. Ansewer in [size/2, size/2]
inline int wrappedSignedDist(int a, int b, int size) {
	int d = b-a;
	while (d < -size/2) d += size;
	while (d > +size/2) d -= size;
	return d;
}

inline float wrappedDistanceSqr(const Vec2f& a, const Vec2f& b, const Vec2& size) {
	return sqr(wrappedAbsDist(a[0], b[0], size[0])) + sqr(wrappedAbsDist(a[1], b[1], size[1]));
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
			// Wrapped
			return size + m_max[a] - m_min[a] - 1;
		}
	}

	Vec2 size(const Vec2& mapSize) const {
		return Vec2(
					sizeAxis(0, mapSize[0]),
					sizeAxis(1, mapSize[1])
					);
	}

	float centerAxisF(int a, int size) const {
		return wrap(m_min[a] + 0.5f*(sizeAxis(a, size)-1), size);
	}

	Vec2f centerF(const Vec2& mapSize) const {
		return Vec2f(
					centerAxisF(0, mapSize[0]),
					centerAxisF(1, mapSize[1])
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
