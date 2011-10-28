#include "Ant.hpp"

Ant::Ant(const Pos &loc)
	: m_position(loc) {
}

Ant::Ant(Ant const& ant)
	: m_position(ant.m_position){

}

Ant& Ant::operator=(Ant const& ant) {
	m_position = ant.m_position;
	return *this;
}

Pos& Ant::pos() {
	return m_position;
}
