#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        Loader::get()->queueInMainThread([this]() {
            if (!Mod::get()->getSavedValue<bool>("shown-update-1.1.0")) {
                FLAlertLayer::create(
                    "Smooth Caret",
                    "Hello there smooth caret user! Check settings, because theres so many new ones to try!",
                    "I will!"
                )->show();
                Mod::get()->setSavedValue<bool>("shown-update-1.1.0", true);
            }
            return true;
        });

        return true;
    }
};
