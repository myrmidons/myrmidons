#ifndef ASSERT_HPP
#define ASSERT_HPP

void AssertionFailed(const char* expr,
					 const char* function, const char* file, long line);


#define ASSERT_ALWAYS(expr) ((expr) ? ((void)0) : AssertionFailed(#expr, __FUNCTION__, __FILE__, __LINE__))

// ASSERT is debug only
#ifndef NDEBUG
#	define ASSERT(x) ASSERT_ALWAYS(x)
#else
#	define ASSERT(x) ((void)0)
#endif

#endif // ASSERT_HPP
