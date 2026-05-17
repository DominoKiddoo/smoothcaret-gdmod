#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/cocos/extensions/GUI/CCControlExtension/CCControlUtils.h>

using namespace geode::prelude;
class ModSettingsPopup : public geode::Popup {};

class $modify(MyCCTextInputNode, CCTextInputNode) {
	struct Fields {
        CCLabelBMFont* m_trueCaret = nullptr;
		CCLabelBMFont* m_smoothCaret = nullptr;
    };

    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
        if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) {
            return false;
        }

		Ref<MyCCTextInputNode> aliveNode = this;

		Loader::get()->queueInMainThread([aliveNode]() {
			if (CCScene::get()->getChildByType<ModSettingsPopup>(0)) {
				return; 
			}

			if (!aliveNode || !aliveNode->getParent()) return;

			auto children = aliveNode->getChildren();
			if (!children) return;

			for (auto* child : CCArrayExt<CCNode*>(children)) {
				if (auto* label = typeinfo_cast<CCLabelBMFont*>(child)) {
					std::string_view content = label->getString();
					if (content == "|") {
						// log::info("fntfile: {}", label->getFntFile());
						if (std::string_view(label->getFntFile()) == "chatFont.fnt" ) {
							aliveNode->m_fields->m_trueCaret = label;
							break;
						}
						
					}
				}
			}

			auto trueCaret = aliveNode->m_fields->m_trueCaret;

			if (!trueCaret) {
				log::warn("Could not find the true caret");
				return; 
			}

			auto smoothCaret = CCLabelBMFont::create("|", "chatFont.fnt");

			auto cColType = Mod::get()->getSettingValue<std::string>("col-type");
			bool isChroma = false;
			if (cColType == "Chroma") {
				isChroma = true;
			}

			if (Mod::get()->getSettingValue<bool>("custom-colour") && isChroma) {
				smoothCaret->setColor(ccColor3B(255, 0, 0)); 
			} else {
				smoothCaret->setColor(trueCaret->getColor());
			}
			
			smoothCaret->setOpacity(trueCaret->getOpacity());

			smoothCaret->setContentSize(trueCaret->getContentSize());
			smoothCaret->setAnchorPoint(trueCaret->getAnchorPoint());
			smoothCaret->ignoreAnchorPointForPosition(trueCaret->isIgnoreAnchorPointForPosition());

			smoothCaret->setPosition(trueCaret->getPosition());
			smoothCaret->setScale(trueCaret->getScale());
			

			smoothCaret->setID("smooth-caret"_spr);
			
			trueCaret->setOpacity(0); // so you cant see it but then i can check later if its visible, and then steal it >:3


			trueCaret->getParent()->addChild(smoothCaret);

			aliveNode->schedule(schedule_selector(MyCCTextInputNode::updSmoothCaret));
			aliveNode->m_fields->m_smoothCaret = smoothCaret;
			return;
		});

		return true;
    }

	void updSmoothCaret(float dt) {

		auto trueCaret = m_fields->m_trueCaret;
		auto smoothCaret = m_fields->m_smoothCaret;
		if (!trueCaret || !smoothCaret) return;

		CCPoint oldPos = smoothCaret->getPosition();
    	CCPoint targetPos = trueCaret->getPosition();

		// make sure settings are applied

		smoothCaret->setContentSize(trueCaret->getContentSize());
		smoothCaret->setAnchorPoint(trueCaret->getAnchorPoint());
		smoothCaret->ignoreAnchorPointForPosition(trueCaret->isIgnoreAnchorPointForPosition());

		float lerpSpeed = Mod::get()->getSettingValue<float>("Weight"); 
    
		float newX = std::lerp(smoothCaret->getPositionX(), trueCaret->getPositionX(), std::min(1.0f, dt * lerpSpeed));
		float newY = std::lerp(smoothCaret->getPositionY(), trueCaret->getPositionY(), std::min(1.0f, dt * lerpSpeed));

		float velocityX = (newX - oldPos.x) / dt;

		float stretchIntensity = 0.1; 
   		float stretch = 1.0f + std::abs(velocityX) * stretchIntensity;

		float baseScale = trueCaret->getScale();

		if (Mod::get()->getSettingValue<bool>("Stretch")) {
			smoothCaret->setScaleX(stretch * baseScale);
		}
		
	

		smoothCaret->setPosition(newX, newY);
		smoothCaret->setVisible(trueCaret->isVisible());

		// colour stuff!!!
		if (Mod::get()->getSettingValue<bool>("custom-colour")) {
			auto colType = Mod::get()->getSettingValue<std::string>("col-type");
			if (colType == "Static Colour") {
				smoothCaret->setColor(Mod::get()->getSettingValue<cocos2d::ccColor3B>("col"));
			} else {
				// RAINBOW STUFFS!!
				ccColor3B oldCol = smoothCaret->getColor();

				// caret col to rgba
				cocos2d::extension::RGBA rgba;
				rgba.r = oldCol.r / 255.0f;
				rgba.g = oldCol.g / 255.0f;
				rgba.b = oldCol.b / 255.0f;
				rgba.a = 1.0f;

				// caret col to hsv
				cocos2d::extension::HSV toHSV = cocos2d::extension::CCControlUtils::HSVfromRGB(rgba);

				// add hue
				toHSV.h += (Mod::get()->getSettingValue<float>("chroma-speed") * 10) * dt;
				if (toHSV.h >= 360.0f) toHSV.h -= 360.0f;

				// BACK to rgba
				rgba = cocos2d::extension::CCControlUtils::RGBfromHSV(toHSV);

				// turn THAT into ccColor3B (this is such a long process oml 😭😭)
				ccColor3B finalCol = ccColor3B(rgba.r * 255, rgba.g * 255, rgba.b * 255);
				smoothCaret->setColor(finalCol);
			}
			
		} else {
			smoothCaret->setColor(trueCaret->getColor());
		}

	}
};