#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <nwo5.silly-api/include/utils/include.hpp>
#include "settings.hpp"

using namespace geode::prelude;

class SmoothCaret final : public CCLabelBMFont {
protected:
	CCTextInputNode* m_inputNode = nullptr;
	CCLabelBMFont* m_trueCaret = nullptr;

	float m_baseScale;
	
	bool m_blinkVisible = true;

	void copyTrueCaretValues(bool updateTransform) {
		this->setContentSize(m_trueCaret->getContentSize());
		this->setAnchorPoint(m_trueCaret->getAnchorPoint());
		this->ignoreAnchorPointForPosition(m_trueCaret->isIgnoreAnchorPointForPosition());
		this->setVisible(m_trueCaret->isVisible());
		this->setColor(m_trueCaret->getColor());

		if (updateTransform) {
			this->setPosition(m_trueCaret->getPosition());
			this->setScale(m_trueCaret->getScale());
		}
	}

	bool init(CCTextInputNode* inputNode, CCLabelBMFont* trueCaret) {
		if (!CCLabelBMFont::initWithString("|", "chatFont.fnt")) {
			return false;
		}

		m_inputNode = inputNode;
		m_trueCaret = trueCaret;

		this->copyTrueCaretValues(true);

		this->setOpacity(Settings::opacity);

		m_baseScale = this->getScale();

		return true;
	}
	
	float getStretchScale(float dt, float newX, float oldX, float targetX) {
		float dist = std::abs(oldX - targetX);
		float velocity = std::abs((newX - oldX) / dt);

		float stretch;
		if (Settings::stretchMethod == "Logarithm") {
			// different stretch intensity multipliers so they stretch somewhat similarly on the same stretch intensity settings (kinda)
			float stretchIntensity = Settings::stretchIntensity * 5.0f;
			stretch = std::log1pf(velocity * stretchIntensity);
		} else if (Settings::stretchMethod == "Square Root") {
			float stretchIntensity = Settings::stretchIntensity * 0.5f;
			stretch = std::sqrt(velocity * stretchIntensity);
		} else {
			float stretchIntensity = Settings::stretchIntensity * 0.05f;
			stretch = std::abs(velocity * stretchIntensity);
		}

		return std::clamp(1.0f + stretch, 1.0f, std::max(1.0f, dist / (this->getContentWidth() * m_baseScale)));
	}

	virtual void update(float dt) {
		if (!m_trueCaret) {
			return;
		}

		copyTrueCaretValues(false);
    
		CCPoint oldPos = this->getPosition();
    	CCPoint targetPos = m_trueCaret->getPosition();
		CCPoint newPos = ccpLerp(oldPos, targetPos, std::min(1.0f, Settings::weight * dt));

		this->setPosition(newPos);
		
		if (Settings::stretchEnabled) {
			float stretch = getStretchScale(dt, newPos.x, oldPos.x, targetPos.x);
			float thickness = Settings::thickness;

			// so stretch is still consistent with different caret thickness
			if (stretch > 1.1f) {
				thickness = 1.0f;
			}

			this->setScaleX(m_baseScale * thickness * stretch);
		} else {
			this->setScaleX(m_baseScale * Settings::thickness);
		}

		if (Settings::customColoursEnabled) {
			if (Settings::chroma) {
				this->setColor(nwo5::utils::getChroma<ccColor3B>(
					Settings::chromaSpeed, false, Settings::chromaSaturation
				));
			} else {
				this->setColor(Settings::caretColour);
			}
		}
	}

	void updateBlink(float dt = {}) {
		if (Settings::blinkCursor) {
			m_blinkVisible = !m_blinkVisible;
		} else {
			m_blinkVisible = true;
		}

		this->setOpacity(m_blinkVisible ? Settings::opacity : 0);
	}

public:
	static SmoothCaret* create(CCTextInputNode* inputNode, CCLabelBMFont* trueCaret) {
		auto ret = new SmoothCaret;

		if (!ret->init(inputNode, trueCaret)) {
			delete ret;

			return nullptr;
		}

		ret->autorelease();

		return ret;
	}

	void inputUpdated() {
		m_blinkVisible = true;
		this->setOpacity(Settings::opacity);

		// fixes mod settings popup idk it apparently just unschedules shit when u type and i cant find y but wtv
		// still kinda broken if u use sliders to adjust inputs but wtv good enough im tired
		
		this->scheduleUpdate(); // dont need to unschedule since there can only b one update function
		
		this->unschedule(schedule_selector(SmoothCaret::updateBlink));
		this->schedule(schedule_selector(SmoothCaret::updateBlink), Settings::blinkSpeed);
	}
};

class $modify(MyCCTextInputNode, CCTextInputNode) {
	struct Fields {
		SmoothCaret* m_smoothCaret = nullptr;
    };

    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
        if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) {
            return false;
        }

		Ref<MyCCTextInputNode> aliveNode = this;

		Loader::get()->queueInMainThread([aliveNode] () {
			auto trueCaret = aliveNode->m_cursor;

			if (!trueCaret) {
				log::warn("Could not find the true caret");

				return; 
			}

			trueCaret->setOpacity(0); // so you cant see it but then i can check later if its visible, and then steal it >:3

			auto smoothCaret = SmoothCaret::create(aliveNode, trueCaret);

			smoothCaret->setID("smooth-caret"_spr);
			trueCaret->getParent()->addChild(smoothCaret);

			aliveNode->m_fields->m_smoothCaret = smoothCaret;
		});

		return true;
    }

	void textChanged() {
		CCTextInputNode::textChanged();

		if (auto smoothCaret = m_fields->m_smoothCaret) {
			smoothCaret->inputUpdated();
		}
	}

	// called on click
	void updateCursorPosition(CCPoint position, CCRect rect) {
		CCTextInputNode::updateCursorPosition(position, rect);

		if (auto smoothCaret = m_fields->m_smoothCaret) {
			smoothCaret->inputUpdated();
		}
	}
};

$on_mod(Loaded) {
	SettingsManager::get()->load();
}