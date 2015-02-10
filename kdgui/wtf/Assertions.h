
#ifndef Assertions_h
#define Assertions_h

#ifndef NDEBUG
#define ASSERT(expr)  do {if((expr) == FALSE){ DebugBreak(); }} while (0);
#else
#define ASSERT(expr) do {} while (0);
#endif

#define ASSERT_UNUSED(variable, assertion) ASSERT(assertion)

#define LOG_ERROR(...) ((void)0)

inline void CRASH() {
	DebugBreak();
};

/* COMPILE_ASSERT */
#ifndef COMPILE_ASSERT
#define COMPILE_ASSERT(exp, name) typedef int dummy##name [(exp) ? 1 : -1]
#endif

#endif // Assertions_h