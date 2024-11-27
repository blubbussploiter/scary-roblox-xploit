
#include <Windows.h>

namespace AoBS {
	static bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
			if (*szMask == 'x' && *pData != *bMask) return 0;
		return (*szMask) == NULL;
	}

	static DWORD SigScan(const char* AoB, const char* Mask) {

        MEMORY_BASIC_INFORMATION MBI = { 0 };
        SYSTEM_INFO SI = { 0 };
        GetSystemInfo(&SI);
        DWORD Start = (DWORD)SI.lpMinimumApplicationAddress;
        DWORD End = (DWORD)SI.lpMaximumApplicationAddress;

        do
        {
            while (VirtualQuery((void*)Start, &MBI, sizeof(MBI))) {
                if ((MBI.Protect & PAGE_READWRITE) && !(MBI.Protect & PAGE_GUARD))
                {
                    for (DWORD i = (DWORD)(MBI.BaseAddress); i - (DWORD)(MBI.BaseAddress) < MBI.RegionSize; ++i)
                    {
                        if (Compare((BYTE*)i, (BYTE*)AoB, Mask))
                            return i;
                    }
                }
                Start += MBI.RegionSize;
            }
        } while (Start < End);
		return 0;
	}
}