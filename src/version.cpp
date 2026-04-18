
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define EXPORT_PROC(ret, name, args, args_call) \
extern "C" ret WINAPI _##name##args \
{ \
    __pragma(comment(linker, "/EXPORT:" #name "=" __FUNCDNAME__))\
    static auto s_Proc = (decltype(&_##name))GetProcAddress(GetProxyModule(), #name);\
    return s_Proc##args_call;\
}

static HMODULE GetProxyModule()
{
    static HMODULE s_Module;
    if (s_Module)
        return s_Module;

    wchar_t path[MAX_PATH];
    ::GetSystemDirectoryW(path, MAX_PATH);
    wcscat_s(path, MAX_PATH, L"\\version.dll");
    return s_Module = ::LoadLibraryW(path);
}

EXPORT_PROC(BOOL, GetFileVersionInfoA, (LPCSTR a1, DWORD a2, DWORD a3, LPVOID a4), (a1, a2, a3, a4))
EXPORT_PROC(BOOL, GetFileVersionInfoByHandle, (DWORD a1, HANDLE a2, LPVOID* a3, PDWORD a4), (a1, a2, a3, a4))
EXPORT_PROC(BOOL, GetFileVersionInfoExA, (DWORD a1, LPCSTR a2, DWORD a3, DWORD a4, LPVOID a5), (a1, a2, a3, a4, a5))
EXPORT_PROC(BOOL, GetFileVersionInfoExW, (DWORD a1, LPCWSTR a2, DWORD a3, DWORD a4, LPVOID a5), (a1, a2, a3, a4, a5))
EXPORT_PROC(DWORD, GetFileVersionInfoSizeA, (LPCSTR a1, LPDWORD a2), (a1, a2))
EXPORT_PROC(DWORD, GetFileVersionInfoSizeExA, (DWORD a1, LPCSTR a2, LPDWORD a3), (a1, a2, a3))
EXPORT_PROC(DWORD, GetFileVersionInfoSizeExW, (DWORD a1, LPCWSTR a2, LPDWORD a3), (a1, a2, a3))
EXPORT_PROC(DWORD, GetFileVersionInfoSizeW, (LPCSTR a1, LPDWORD a2), (a1, a2))
EXPORT_PROC(BOOL, GetFileVersionInfoW, (LPCSTR a1, DWORD a2, DWORD a3, LPVOID a4), (a1, a2, a3, a4))

EXPORT_PROC(DWORD, VerFindFileA,
    (DWORD a1, LPCSTR a2, LPCSTR a3, LPCSTR a4, LPSTR a5, PUINT a6, LPSTR a7, PUINT a8),
    (a1, a2, a3, a4, a5, a6, a7, a8))

    EXPORT_PROC(DWORD, VerFindFileW,
        (DWORD a1, LPCWSTR a2, LPCWSTR a3, LPCWSTR a4, LPWSTR a5, PUINT a6, LPWSTR a7, PUINT a8),
        (a1, a2, a3, a4, a5, a6, a7, a8))

    EXPORT_PROC(DWORD, VerInstallFileA,
        (DWORD a1, LPCSTR a2, LPCSTR a3, LPCSTR a4, LPCSTR a5, LPCSTR a6, LPSTR a7, PUINT a8),
        (a1, a2, a3, a4, a5, a6, a7, a8))

    EXPORT_PROC(DWORD, VerInstallFileW,
        (DWORD a1, LPCWSTR a2, LPCWSTR a3, LPCWSTR a4, LPCWSTR a5, LPCWSTR a6, LPWSTR a7, PUINT a8),
        (a1, a2, a3, a4, a5, a6, a7, a8))

    EXPORT_PROC(DWORD, VerLanguageNameA, (DWORD a1, LPSTR a2, DWORD a3), (a1, a2, a3))
    EXPORT_PROC(DWORD, VerLanguageNameW, (DWORD a1, LPWSTR a2, DWORD a3), (a1, a2, a3))
    EXPORT_PROC(BOOL, VerQueryValueA, (LPCVOID a1, LPCSTR a2, LPVOID* a3, PUINT a4), (a1, a2, a3, a4))
    EXPORT_PROC(BOOL, VerQueryValueW, (LPCVOID a1, LPCWSTR a2, LPVOID* a3, PUINT a4), (a1, a2, a3, a4))