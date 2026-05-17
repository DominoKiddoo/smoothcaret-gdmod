#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        Loader::get()->queueInMainThread([this]() {
            log::info("MENU LAYER INITED!!");
            if (!Mod::get()->getSavedValue<bool>("shown-update-1.0.6")) {
                FLAlertLayer::create(
                    "Smooth Caret",
                    "Hello there smooth caret user! Update 1.0.6 has added cool new caret customisation settings. Give them a try!",
                    "I will!"
                )->show();
                Mod::get()->setSavedValue<bool>("shown-update-1.0.6", true);
            }
            return true;
        });

        return true;
    }
};
