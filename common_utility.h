#ifndef _COMMON_UTILITY_H_
#define _COMMON_UTILITY_H_ 

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace std;
class Utility
{
public:
    static int getCurMileSeconds()
    {
        struct timeval now;
        gettimeofday(&now);
        return now.tv_sec * 1000 + now.tv_usec / 1000;
    }

};

#endif
