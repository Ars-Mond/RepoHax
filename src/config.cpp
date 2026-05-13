#include "config.h"

#include "cheat.h"

namespace Cheat::Config
{
    // section, name, type, member, default
    #define CONFIG_FIELDS(X)                                                            \
        X("Player",   "Godmode",                Bool, Godmode,                 false)   \
        X("Player",   "InfStamina",             Bool, InfStamina,              false)   \
        X("Player",   "InfJumps",               Bool, InfJumps,                false)   \
        X("Player",   "NoTumble",               Bool, NoTumble,                false)   \
        X("Player",   "AccelWalking",           Int,  Acceleration.Walking,    1)       \
        X("Player",   "AccelSprinting",         Int,  Acceleration.Sprinting,  1)       \
        X("Player",   "AccelCrouching",         Int,  Acceleration.Crouching,  1)       \
        X("Player",   "ImproveVision",          Bool, ImproveVision,           false)   \
        X("Player",   "FOV",                    Int,  FOV,                     60)      \
        X("Player",   "ThirdPerson",            Bool, ThirdPerson,             false)   \
        X("Player",   "HeadMaxBattery",         Bool, HeadMaxBattery,          false)   \
        X("Flashlight", "Intensity",            Int,  Flashlight.Intensity,    10)      \
        X("Flashlight", "Angle",                Int,  Flashlight.Angle,        60)      \
        X("Flashlight", "AllowInCrouch",        Bool, Flashlight.AllowInCrouch,false)   \
        X("Grab",     "EasyGrab",               Bool, EasyGrab,                false)   \
        X("Grab",     "UnlimGrabRange",         Bool, UnlimGrabRange,          false)   \
        X("Grab",     "NoOverCharge",           Bool, NoOverCharge,            false)   \
        X("Upgrades", "AutoUseUpgr",            Bool, AutoUseUpgr,             false)   \
        X("Items",    "Unbreakable",            Bool, Unbreakable,             false)   \
        X("Items",    "InfBattery",             Bool, InfBattery,              false)   \
        X("Items",    "ItemsChams",             Bool, ItemsChams,              false)   \
        X("Items",    "UseLaser",               Bool, UseLaser,                false)   \
        X("Enemies",  "Blind",                  Bool, Blind,                   false)   \
        X("Enemies",  "NoGrabMaxTime",          Bool, NoGrabMaxTime,           false)   \
        X("Esp",      "EnemiesEsp",             Bool, EnemiesEsp,              false)   \
        X("Esp",      "ValuablesEsp",           Bool, ValuablesEsp,            false)   \
        X("Esp",      "ExtrPointsEsp",          Bool, ExtrPointsEsp,           false)   \
        X("Esp",      "TruckEsp",               Bool, TruckEsp,                false)   \
        X("Esp",      "PlayersEsp",             Bool, PlayersEsp,              false)   \
        X("Esp",      "PlayerChams",            Bool, PlayerChams,             false)   \
        X("Esp",      "CosmeticBoxesEsp",       Bool, CosmeticBoxesEsp,        false)   \
        X("Round",    "RemoveCosmeticLimit",    Bool, RemoveCosmeticLimit,     false)   \
        X("Round",    "FreezeExtraction",       Bool, FreezeExtraction,        false)   \
        X("Ui",       "DarkenBg",               Bool, DarkenBg,                false)

    static constexpr const wchar_t* kConfigsDir = L"configs";
    static constexpr size_t kMaxNameLen = 64;

    struct ProfileSlot { wchar_t Name[kMaxNameLen]; };
    static_assert(std::is_trivially_copyable_v<ProfileSlot>);

    static Hax::Vector<ProfileSlot> g_Profiles;

    static void EnsureConfigsDir()
    {
        ::CreateDirectoryW(kConfigsDir, nullptr);
    }

    static void BuildPath(Hax::WStringView name, Hax::WStringBuilder<260>& out)
    {
        out.Clear();
        out.AppendF(L"%s\\%.*s.ini", kConfigsDir, (int)name.Length(), name.Data());
    }

    static void RegisterFields(Hax::IniFile& file)
    {
        #define X(sect, key, type, member, defv) \
            Hax::IniAddEntry(file, sect, key, &GCheat->member, Hax::IniFileWrite_##type, Hax::IniFileRead_##type);
        CONFIG_FIELDS(X)
        #undef X
    }

    void Save(Hax::WStringView name)
    {
        if (name.Empty()) return;

        EnsureConfigsDir();

        Hax::WStringBuilder<260> path;
        BuildPath(name, path);

        Hax::IniFile file(path.CStr());
        RegisterFields(file);
        Hax::IniSave(file);
    }

    void Load(Hax::WStringView name)
    {
        if (name.Empty()) return;

        Hax::WStringBuilder<260> path;
        BuildPath(name, path);

        Hax::IniFile file(path.CStr());
        RegisterFields(file);
        Hax::IniLoad(file);
    }

    void Delete(Hax::WStringView name)
    {
        if (name.Empty()) return;

        Hax::WStringBuilder<260> path;
        BuildPath(name, path);
        ::DeleteFileW(path.CStr());
    }

    bool Exists(Hax::WStringView name)
    {
        if (name.Empty()) return false;

        Hax::WStringBuilder<260> path;
        BuildPath(name, path);
        DWORD attrs = ::GetFileAttributesW(path.CStr());
        return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
    }

    void ResetToDefaults()
    {
        #define X(sect, key, type, member, defv) GCheat->member = defv;
        CONFIG_FIELDS(X)
        #undef X
    }

    void Refresh()
    {
        g_Profiles.Clear();

        Hax::WStringBuilder<260> pattern;
        pattern.AppendF(L"%s\\*.ini", kConfigsDir);

        WIN32_FIND_DATAW fd{};
        HANDLE h = ::FindFirstFileW(pattern.CStr(), &fd);
        if (h == INVALID_HANDLE_VALUE)
            return;

        do
        {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                continue;

            Hax::WStringView fileName(fd.cFileName);
            Hax::WStringView ext(L".ini");
            if (!fileName.EndsWith(ext))
                continue;

            fileName.RemoveSuffix(ext.Length());

            ProfileSlot slot{};
            size_t n = Hax::Min(fileName.Length(), kMaxNameLen - 1);
            for (size_t i = 0; i < n; ++i) slot.Name[i] = fileName[i];
            slot.Name[n] = 0;
            g_Profiles.PushBack(slot);
        } while (::FindNextFileW(h, &fd));

        ::FindClose(h);
    }

    size_t Count()
    {
        return g_Profiles.Size();
    }

    Hax::WStringView At(size_t i)
    {
        if (i >= g_Profiles.Size())
            return Hax::WStringView(L"", 0);
        return Hax::WStringView(g_Profiles[i].Name);
    }
}
