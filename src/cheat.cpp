#include "cheat.h"

#include "win_hooks.h"
#include "game_hooks.h"
#include "config.h"

namespace Cheat
{
    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4);

    bool Initialize(void* hCheat)
    {
        if (!Hax::Unity::IsUnityProcess())
            return false;

        GCheat = Hax::New<Context>();
        GCheat->hCheat = (HMODULE)hCheat;

        Hax::IniFile& iniFile = GCheat->IniFile;
        Hax::IniAddEntry(iniFile, "Cheat", "UseConsole",  &GCheat->UseConsole,  Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Cheat", "VkOpenClose", &GCheat->VkOpenClose, Hax::IniFileWrite_Int,  Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Cheat", "Language", &GCheat->Lang, Hax::IniFileWrite_Int,  Hax::IniFileRead_Int);
        Hax::IniLoad(iniFile);

        Hax::LogFile& logFile = GCheat->LogFile;
        Hax::InitLogFile(logFile, L"haxsdk_logs.txt", GCheat->UseConsole);

        Hax::Log(logFile, L"Waiting for unity virtual machine...");
        while (!Hax::Unity::GetUvmHandle())
            Sleep(200);

        {
            HANDLE hEvent = ::CreateEvent(0, TRUE, FALSE, nullptr);
            HAX_ASSERT(hEvent != nullptr);

            // To verify the unity is fully loaded, I wait for the first Update method to be called
            GCheat->UnityLoadedEvent = hEvent;
            HookModuleProc((HMODULE)Hax::Unity::GetUvmHandle(), "mono_runtime_invoke", Hooked_MonoRuntimeInvoke, GCheat->MonoRuntimeInvokeHook);

            Hax::Log(logFile, L"Waiting for unity...");
            ::WaitForSingleObject(hEvent, INFINITE);
            ::CloseHandle(hEvent);

            GCheat->MonoRuntimeInvokeHook.reset();
            GCheat->UnityLoadedEvent = nullptr;
        }

        Hax::Unity::Initialize(&logFile);
        Cheat::WinHooks::Install();

        HMODULE hDirectX11 = 0;
        do { Sleep(200); hDirectX11 = ::GetModuleHandleW(L"d3d11.dll"); } while (hDirectX11 == 0);

        Cheat::Visuals::InitializeMenu((Hax::Handle)hDirectX11);
        Cheat::GameHooks::Install();

        if (Cheat::Config::Exists(L"default"))
            Cheat::Config::Load(L"default");

        return true;
    }

    void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name)
    {
        auto res = SafetyHookInline::create(ptr, detour);
        HAX_PANIC(res.has_value(), &GCheat->LogFile, L"Failed to hooks %hs. Error %d", name, (int)res.error().type);
        out = std::move(*res);
    }

    void HookModuleProc(HMODULE module, LPCSTR procName, void* procHook, SafetyHookInline& out)
    {
        void* procPtr = ::GetProcAddress(module, procName);
        HAX_PANIC(procPtr != nullptr, &GCheat->LogFile, L"Proc %s not found", procName);

        Hook(procPtr, procHook, out, procName);
    }

    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4)
    {
        void* ret = GCheat->MonoRuntimeInvokeHook.unsafe_fastcall<void*, void*, void*, void*, void*>(a1, a2, a3, a4);

        Hax::StringView methodName = UVM::MethodGetName(*a1);
        if (methodName == "Update")
            ::SetEvent(GCheat->UnityLoadedEvent);

        return ret;
    }
}