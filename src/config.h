#pragma once

#include "haxsdk/hax.h"

namespace Cheat::Config
{
    void Save(Hax::WStringView name);
    void Load(Hax::WStringView name);
    void Delete(Hax::WStringView name);
    bool Exists(Hax::WStringView name);
    void ResetToDefaults();

    void Refresh();
    size_t Count();
    Hax::WStringView At(size_t i);
}
