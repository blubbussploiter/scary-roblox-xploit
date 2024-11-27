
#include <Windows.h>
#include <cstdint>

#include "robloxOpcodes.h"

static long long multiplicativeInverse(long long a, long long n)
{
    long long t = 0;
    long long newt = 1;

    long long r = n;
    long long newr = a;

    while (newr != 0)
    {
        long long q = r / newr;

        long long curt = t;
        t = newt;
        newt = curt - q * newt;

        long long curr = r;
        r = newr;
        newr = curr - q * newr;
    }

    return (t < 0) ? t + n : t;
}

namespace RBX
{
    extern unsigned int getCkey(DWORD* a1);
    extern unsigned int getEncode(std::uintptr_t rL);
}