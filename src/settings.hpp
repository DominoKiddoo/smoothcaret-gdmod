#pragma once

#include <nwo5.silly-api/cache/include.hpp>

using namespace nwo5::cache;
namespace Settings {
    inline SettingCache<float> weight{"weight"};
    inline SettingCache<bool> stretchEnabled{"stretch"};
    inline SettingCache<float> stretchIntensity{"stretch-intensity"};
    inline SettingCache<std::string> stretchMethod{"stretch-method"};
    inline SettingCache<float> thickness{"thickness"};
    inline SettingCache<int> opacity{"opacity"};
    inline SettingCache<bool> blinkCursor{"blink-cursor"};
    inline SettingCache<float> blinkSpeed{"blink-speed"};
    inline SettingCache<bool> customColoursEnabled{"custom-caret-colour"};
    inline SettingCache<cocos2d::ccColor3B> caretColour{"caret-colour"};
    inline SettingCache<bool> chroma{"chroma"};
    inline SettingCache<float> chromaSaturation{"chroma-saturation"};
    inline SettingCache<float> chromaSpeed{"chroma-speed"};
    inline SettingCache<bool> unlimitedStretch{"unlimited-stretch"};

}