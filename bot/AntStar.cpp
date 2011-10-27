#include "AntStar.hpp"
#include "Map.hpp"

AntStar::AntStar()
{

}


Path AntStar::findPath(Map* m, Pos from, Pos to) {
	m_grid = StarGrid(m->m_rows, std::vector<StarAnt>(m->m_cols, StarAnt()));
	StarAnt start = m_grid[from.x()][from.y()];
	StarAnt goal = m_grid[to.x()][to.y()];
	m_openList.insert(start);

	while(!m_openList.empty()) {
		StarAnt current = *m_openList.begin();
		m_openList.erase(m_openList.begin());

		if(current == goal) {
			Path goalPath;
			StarAnt curr = current;
			while(curr.parent != NULL){
				goalPath.push_front(Pos(curr.x, curr.y));
				curr = *curr.parent;
			}
		}

		for(int i = 0; i < 4; i++){

		}
	}
}
