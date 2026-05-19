#pragma once

#include <nwo5.silly-api/include/settings/include.hpp>

using namespace nwo5::settings::prelude;

namespace Settings {
    inline Setting<float> weight{"weight"};
    inline Setting<bool> stretchEnabled{"stretch"};
    inline Setting<float> stretchIntensity{"stretch-intensity"};
    inline Setting<std::string> stretchMethod{"stretch-method"};
    inline Setting<float> thickness{"thickness"};
    inline Setting<int> opacity{"opacity"};
    inline Setting<bool> blinkCursor{"blink-cursor"};
    inline Setting<float> blinkSpeed{"blink-speed"};
    inline Setting<bool> customColoursEnabled{"custom-caret-colour"};
    inline Setting<cocos2d::ccColor3B> caretColour{"caret-colour"};
    inline Setting<bool> chroma{"chroma"};
    inline Setting<float> chromaSaturation{"chroma-saturation"};
    inline Setting<float> chromaSpeed{"chroma-speed"};
    inline Setting<bool> unlimitedStretch{"unlimited-stretch"};

}