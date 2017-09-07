#include "ResourceMonitor.h"

#include <iostream>
#include <iomanip>

#include <windows.h>
#include <psapi.h>

namespace ResourceMonitor 
{

namespace
{

/**

The source code that calculate the CPU Usage is from:
https://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c

To be honest, I did some research about how to calculate the cpu usage and ended up
at this url. I tested it but it seems like the calculation is not precise
and sometimes incorrect.

Let's assume that this calculation is correct anyway for the test purpose.

*/
static float calculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
   static unsigned long long _previousTotalTicks = 0;
   static unsigned long long _previousIdleTicks = 0;

   unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
   unsigned long long idleTicksSinceLastTime  = idleTicks - _previousIdleTicks;

   float ret = 1.0f - ((totalTicksSinceLastTime > 0) ?
       ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);

   _previousTotalTicks = totalTicks;
   _previousIdleTicks  = idleTicks;

   return ret;
}

static unsigned long long fileTimeToInt64(const FILETIME& ft)
{
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) |
            ((unsigned long long)ft.dwLowDateTime);
}

} // anonymous namespace.

//-----------------------------------------------------------------------------

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
double getCPUUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ?
        calculateCPULoad(fileTimeToInt64(idleTime),
            fileTimeToInt64(kernelTime) + fileTimeToInt64(userTime)) : -1.0f;
}

double getMemoryUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalMem = memInfo.ullTotalPhys;
    DWORDLONG memUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

    return (double)memUsed / totalMem;
}

unsigned long getProcessNumber()
{
    DWORD processIds[1024], bytesReturn;
    if (!EnumProcesses(processIds, sizeof(processIds), &bytesReturn)) {
        throw ResourceMonitorError("Cannot get the number of process!");
    }

    // Calculate how many process identifiers were returned.
    return bytesReturn / sizeof(DWORD);
}

} // ResourceMonitor namespace
