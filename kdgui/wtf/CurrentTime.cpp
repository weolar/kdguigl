#include "CurrentTime.h"
#include <time.h>
#include <sys/timeb.h>

#ifdef _MSC_VER
#include <mmsystem.h>
#endif

static bool syncedTime;
const double msPerSecond = 1000.0;

#ifdef _MSC_VER

static LARGE_INTEGER qpcFrequency;

static double highResUpTime()
{
    // We use QPC, but only after sanity checking its result, due to bugs:
    // http://support.microsoft.com/kb/274323
    // http://support.microsoft.com/kb/895980
    // http://msdn.microsoft.com/en-us/library/ms644904.aspx ("...you can get different results on different processors due to bugs in the basic input/output system (BIOS) or the hardware abstraction layer (HAL)."

    static LARGE_INTEGER qpcLast;
    static DWORD tickCountLast;
    static bool inited;

    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    DWORD tickCount = GetTickCount();

    if (inited) {
        __int64 qpcElapsed = ((qpc.QuadPart - qpcLast.QuadPart) * 1000) / qpcFrequency.QuadPart;
        __int64 tickCountElapsed;
        if (tickCount >= tickCountLast)
            tickCountElapsed = (tickCount - tickCountLast);
        else {
// #if COMPILER(MINGW)
//             __int64 tickCountLarge = tickCount + 0x100000000ULL;
// #else
            __int64 tickCountLarge = tickCount + 0x100000000I64;
// #endif
            tickCountElapsed = tickCountLarge - tickCountLast;
        }

        // force a re-sync if QueryPerformanceCounter differs from GetTickCount by more than 500ms.
        // (500ms value is from http://support.microsoft.com/kb/274323)
        __int64 diff = tickCountElapsed - qpcElapsed;
        if (diff > 500 || diff < -500)
            syncedTime = false;
    } else
        inited = true;

    qpcLast = qpc;
    tickCountLast = tickCount;

    return (1000.0 * qpc.QuadPart) / static_cast<double>(qpcFrequency.QuadPart);
}

static double lowResUTCTime()
{
#if 0 // OS(WINCE)
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);
    struct tm tmtime;
    tmtime.tm_year = systemTime.wYear - 1900;
    tmtime.tm_mon = systemTime.wMonth - 1;
    tmtime.tm_mday = systemTime.wDay;
    tmtime.tm_wday = systemTime.wDayOfWeek;
    tmtime.tm_hour = systemTime.wHour;
    tmtime.tm_min = systemTime.wMinute;
    tmtime.tm_sec = systemTime.wSecond;
    time_t timet = mktime(&tmtime);
    return timet * msPerSecond + systemTime.wMilliseconds;
#else
#pragma warning(push)  
#pragma warning(disable : 4996)
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    return timebuffer.time * msPerSecond + timebuffer.millitm;
#pragma warning(pop)
#endif
}

static bool qpcAvailable()
{
    static bool available;
    static bool checked;

    if (checked)
        return available;

    available = !!QueryPerformanceFrequency(&qpcFrequency);
    checked = true;
    return available;
}

double currentTime1()
{
    // Use a combination of ftime and QueryPerformanceCounter.
    // ftime returns the information we want, but doesn't have sufficient resolution.
    // QueryPerformanceCounter has high resolution, but is only usable to measure time intervals.
    // To combine them, we call ftime and QueryPerformanceCounter initially. Later calls will use QueryPerformanceCounter
    // by itself, adding the delta to the saved ftime.  We periodically re-sync to correct for drift.
    static double syncLowResUTCTime;
    static double syncHighResUpTime;
    static double lastUTCTime;

    double lowResTime = lowResUTCTime();

    if (!qpcAvailable())
        return lowResTime / 1000.0;

    double highResTime = highResUpTime();

    if (!syncedTime) {
        timeBeginPeriod(1); // increase time resolution around low-res time getter
        syncLowResUTCTime = lowResTime = lowResUTCTime();
        timeEndPeriod(1); // restore time resolution
        syncHighResUpTime = highResTime;
        syncedTime = true;
    }

    double highResElapsed = highResTime - syncHighResUpTime;
    double utc = syncLowResUTCTime + highResElapsed;

    // force a clock re-sync if we've drifted
    double lowResElapsed = lowResTime - syncLowResUTCTime;
    const double maximumAllowedDriftMsec = 15.625 * 2.0; // 2x the typical low-res accuracy
    if (fabs(highResElapsed - lowResElapsed) > maximumAllowedDriftMsec)
        syncedTime = false;

    // make sure time doesn't run backwards (only correct if difference is < 2 seconds, since DST or clock changes could occur)
    const double backwardTimeLimit = 2000.0;
    if (utc < lastUTCTime && (lastUTCTime - utc) < backwardTimeLimit)
        return lastUTCTime / 1000.0;
    lastUTCTime = utc;
    return utc / 1000.0;
}

double currentTime()
{
	// Use a combination of ftime and QueryPerformanceCounter.
	// ftime returns the information we want, but doesn't have sufficient resolution.
	// QueryPerformanceCounter has high resolution, but is only usable to measure time intervals.
	// To combine them, we call ftime and QueryPerformanceCounter initially. Later calls will use QueryPerformanceCounter
	// by itself, adding the delta to the saved ftime.  We periodically re-sync to correct for drift.
	static double syncLowResUTCTime;
	static double syncHighResUpTime;
	static double lastUTCTime;

	double lowResTime = highResUpTime();

	if (!qpcAvailable())
		return lowResTime / 1000.0;

	double highResTime = highResUpTime();

	if (!syncedTime) {
		timeBeginPeriod(1); // increase time resolution around low-res time getter
		syncLowResUTCTime = lowResTime = highResUpTime();
		timeEndPeriod(1); // restore time resolution
		syncHighResUpTime = highResTime;
		syncedTime = true;
	}

	double highResElapsed = highResTime - syncHighResUpTime;
	double utc = syncLowResUTCTime + highResElapsed;

	return utc / 1000.0;
}

#else // _MSC_VER

double currentTime()
{
	struct timeval now;
	gettimeofday(&now, 0);
	return now.tv_sec + now.tv_usec / 1000000.0;
}

#endif // _MSC_VER