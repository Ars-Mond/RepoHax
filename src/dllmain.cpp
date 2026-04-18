
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Cheat
{
    extern bool Initialize(void* hDll); //!
}

static DWORD WINAPI Start(LPVOID handle)
{
    Cheat::Initialize((void*)handle);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, Start, hModule, 0, NULL);
    }

    return TRUE;
}

