#include "ckey.h"

unsigned int RBX::getCkey(DWORD * a1)
{
    return (int)a1 + a1[2] + *(DWORD*)((char*)a1 + a1[2] + 36) + 36;
}

unsigned int RBX::getEncode(std::uintptr_t rL)
{
    unsigned int ckey = getCkey((DWORD*)rL);
    return multiplicativeInverse(ckey, 1ll << 32);
}