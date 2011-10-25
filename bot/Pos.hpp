#ifndef POS_H_
#define POS_H_

#include <vector>
#include <set>

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

	inline int& operator[] (int const& i) { return coords[i]; }
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

typedef sd::set<Pos> PosSet;
typedef sd::vector<Pos> PosList;

inline bool operator < (Pos const& a, Pos const& b) {
	return (a[0] < b[0])||((a[0] == b[0]) && (a[1] < b[1]));
}

inline std::ostream& operator<<(std::ostream &os, const Pos& pos) {
	return os << "(" << pos[0] << ", " << pos[1] << ")";
}

//////////////////////////////////////////////////////

// Bounding box
class BB {
	Pos topLeft, bottomRight; // [,) style ranges

	int width()  const { return bottomRight.x()-topLeft.x(); }
	int height() const { return bottomRight.y()-topLeft.y(); }
};

//////////////////////////////////////////////////////
// Bös

typedef std::vector<int> IntList;
typedef std::set<int> IntSet;

#endif //POS_H_
