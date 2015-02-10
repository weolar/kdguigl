
#ifndef CurrentTime_h
#define CurrentTime_h

// Returns the current UTC time in seconds, counted from January 1, 1970.
// Precision varies depending on platform but is usually as good or better
// than a millisecond.
double currentTime();

// Same thing, in milliseconds.
inline double currentTimeMS()
{
	return currentTime() * 1000.0;
}

#endif // CurrentTime_h