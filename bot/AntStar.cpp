#include "AntStar.hpp"
#include "Map.hpp"

AntStar::AntStar()
{

}

int AntStar::getMovementCost(){
	return 1; // kostar alltid 1 att flytta sig mellan steg atm.
}

double AntStar::heuristics(StarAnt* current, StarAnt* goal){
	return g_map->distance(current->p, goal->p);
}

Path AntStar::findPath(Pos from, Pos to) {
	assert(from.x() > -1 && from.x() < g_map->m_cols);
	assert(from.y() > -1 && from.y() < g_map->m_rows);

	m_grid = StarGrid(g_map->m_rows);
	for(int r=0;r < g_map->m_rows;r++){
		m_grid[r].reserve(g_map->m_cols);
		for(int c=0;c< g_map->m_cols;c++){
			m_grid[r].push_back( StarAnt(r,c) );
		}
	}

	StarAnt start = m_grid[from.x()][from.y()];
	StarAnt goal = m_grid[to.x()][to.y()];
	m_openList.insert(start);

	while(!m_openList.empty()) {
		StarAnt current = *m_openList.begin();
		m_openList.erase(m_openList.begin());

		// are we there yet? Yes we are son, back-trace vägen
		if(current == goal) {
			Path goalPath;
			StarAnt curr = current;
			while(curr.parent != NULL){
				goalPath.push_front(curr.p);
				curr = *curr.parent;
			}
			return goalPath;
		}

		// gå i de olika vädersträcken
		for(int i = 0; i < 4; i++){
			Pos nPos = g_map->getLocation(current.p, i);
			StarAnt n = m_grid[nPos.x()][nPos.y()]; // neighbour

			if(n.passable || n.closed) {
				// kan man inte gå på den eller så har den redan besökts, hoppa
				continue;
			}

			int gScore = current.g + getMovementCost();
			int beenVisited = n.visited;

			// har inte besökts, eller så har det tagit färre steg att komma till den här grannen
			if(!beenVisited || gScore < n.g){
				n.visited = true;
				n.parent = &current;
				n.h = heuristics(&n, &goal);
				n.g = gScore;
				n.f = n.h + n.g;
				if(beenVisited)
					m_openList.insert(n);
				else {
					m_openList.erase(n);
					m_openList.insert(n);
				}
			}
		}
	}
	return Path();
}
