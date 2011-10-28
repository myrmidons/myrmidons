#ifndef UTIL_HPP
#define UTIL_HPP

/////////////////////////////////////////////////////////////
/*  Helper function for iterating over collections.
	Usage: ITC(PosList, pit, myList)
			cout << *pit,

	ITC - C is for Constant.
*/

#define IT( Class, it, c) for(Class::iterator       it = (*&(c)).begin(); it != (c).end(); ++it)
#define ITC(Class, it, c) for(Class::const_iterator it = (*&(c)).begin(); it != (c).end(); ++it)

#define IT_T( Class, it, c) for(typename Class::iterator       it = (*&(c)).begin(); it != (c).end(); ++it)
#define ITC_T(Class, it, c) for(typename Class::const_iterator it = (*&(c)).begin(); it != (c).end(); ++it)

#define IT_REVERSE( Class, it, c) for(Class::reverse_iterator       it = (*&(c)).rbegin(); it != (c).rend(); ++it)
#define ITC_REVERSE(Class, it, c) for(Class::const_reverse_iterator it = (*&(c)).rbegin(); it != (c).rend(); ++it)

// For iterating over an integer interval.
#define IT_INTERVAL(varName, begin, end)  for(int varName=(begin), the_end=(end); varName<the_end; ++varName)
#define IT_INTERVALU(varName, begin, end) for(size_t varName=(begin), the_end=(end); varName<the_end; ++varName)

/////////////////////////////////////////////////////////////

/* Usage:
	Room() : m_id(getID<Room>()) {  }
*/

template<class T>
int getID() {
	static int s_id=0;
	return s_id++;
}

/////////////////////////////////////////////////////////////

#endif // UTIL_HPP
