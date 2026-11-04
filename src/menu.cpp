// MegaMobile by EJB3 - the draggable floating ball and the tabbed menu popup.
//
// The menu is made only of toggles, +/- buttons and pages (no scrolling, no sliders), so it works
// the same with a finger on any screen size. Texts come from logic.hpp in English, Spanish or Russian.

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/Popup.hpp>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "state.hpp"

using namespace geode::prelude;

// --------------------------------------------------------------------- helpers

namespace {

    std::string T(mm::S s) {
        return mm::tr(mm::lang(), s);
    }

    std::string valueText(mm::FeatureDef const& d, double x) {
        char buf[24];
        if (d.step >= 1.0) std::snprintf(buf, sizeof(buf), "%.0f", x);
        else if (d.step < 0.1) std::snprintf(buf, sizeof(buf), "%.2f", x);
        else std::snprintf(buf, sizeof(buf), "%.1f", x);
        return buf;
    }

    // A button whose width follows the length of its text (so Russian and Spanish fit).
    ButtonSprite* makeButtonSprite(std::string const& text, float scale) {
        const int chars = mm::utf8Length(text);
        const int width = std::clamp(chars * 14 + 28, 44, 360);
        auto* sprite = ButtonSprite::create(text.c_str(), width, true, mm::buttonFont(), "GJ_button_01.png", 25.f, 0.8f);
        sprite->setScale(scale);
        return sprite;
    }

} // namespace

// ------------------------------------------------------------------- the menu

class MegaMenuPopup : public geode::Popup {
protected:
    inline static bool s_open = false;

    static constexpr int kTabs = 7;   // Player, Visual, Perf, HUD, Misc, Presets, Stats
    static constexpr int kRows = 5;

    int m_tab = 0;
    int m_page = 0;
    bool m_safeArmed = false;         // the first tap on "turn safe mode off" was done
    std::vector<CCNode*> m_dyn;       // rows, rebuilt when the tab or page changes
    std::vector<CCNode*> m_chrome;    // tabs and footer buttons, rebuilt when the language changes
    CCLabelBMFont* m_msg = nullptr;   // the line under the rows (help text and confirmations)

    std::string tabName(int t) const {
        static const mm::S names[kTabs] = {mm::S::TabPlayer, mm::S::TabVisual, mm::S::TabPerf, mm::S::TabHud,
                                           mm::S::TabMisc, mm::S::TabPresets, mm::S::TabStats};
        return T(names[std::clamp(t, 0, kTabs - 1)]);
    }

    std::vector<int> featuresOf(int tab) const {
        std::vector<int> out;
        for (int i = 0; i < mm::kCount; ++i) {
            if (static_cast<int>(mm::kDefs[i].cat) == tab) out.push_back(i);   // hidden ones never match a tab
        }
        return out;
    }

    void clearDyn() {
        for (CCNode* n : m_dyn) n->removeFromParent();
        m_dyn.clear();
    }

    void clearChrome() {
        for (CCNode* n : m_chrome) n->removeFromParent();
        m_chrome.clear();
    }

    void addLabel(std::string const& text, bool big, float x, float y, float scale, bool left, float maxWidth) {
        auto* label = mm::makeLabel(text.c_str(), big);
        if (!label) return;
        const float sc = scale * mm::fontScale();
        label->setScale(sc);
        if (maxWidth > 0.f) label->limitLabelWidth(maxWidth, sc, 0.15f);
        if (left) label->setAnchorPoint({0.f, 0.5f});
        m_mainLayer->addChildAtPosition(label, Anchor::Center, {x, y});
        m_dyn.push_back(label);
    }

    CCMenuItemSpriteExtra* addButton(std::vector<CCNode*>& group, std::string const& text, float scale,
                                     SEL_MenuHandler handler, int tag, float x, float y) {
        auto* btn = CCMenuItemSpriteExtra::create(makeButtonSprite(text, scale), this, handler);
        btn->setTag(tag);
        m_buttonMenu->addChildAtPosition(btn, Anchor::Center, {x, y});
        group.push_back(btn);
        return btn;
    }

    void showMessage(std::string const& text) {
        if (m_msg) {
            m_msg->removeFromParent();
            m_msg = nullptr;
        }
        m_msg = mm::makeLabel(text.c_str(), false);
        if (!m_msg) return;
        const float sc = 0.6f * mm::fontScale();
        m_msg->setScale(sc);
        m_msg->limitLabelWidth(420.f, sc, 0.15f);
        m_mainLayer->addChildAtPosition(m_msg, Anchor::Center, {0.f, -76.f});
    }

    // ------------------------------------------------------------------ content

    static float rowY(int row) { return 58.f - 28.f * static_cast<float>(row); }

    void buildFeatureRows() {
        const mm::Lang lang = mm::lang();
        const std::vector<int> list = featuresOf(m_tab);
        const int pages = std::max(1, (static_cast<int>(list.size()) + kRows - 1) / kRows);
        m_page = std::clamp(m_page, 0, pages - 1);
        mm::Values& v = mm::vals();

        for (int row = 0; row < kRows; ++row) {
            const int k = m_page * kRows + row;
            if (k >= static_cast<int>(list.size())) break;
            const int i = list[k];
            mm::FeatureDef const& d = mm::kDefs[i];
            const float y = rowY(row);

            addLabel(mm::featureName(lang, i), true, -205.f, y, 0.42f, true, 235.f);
            addButton(m_dyn, "?", 0.4f, menu_selector(MegaMenuPopup::onInfo), i, 205.f, y);

            if (d.kind == mm::Kind::Toggle) {
                auto* toggler = CCMenuItemToggler::createWithStandardSprites(
                    this, menu_selector(MegaMenuPopup::onToggle), 0.55f);
                toggler->toggle(v.v[i] >= 0.5);
                toggler->setTag(i);
                m_buttonMenu->addChildAtPosition(toggler, Anchor::Center, {160.f, y});
                m_dyn.push_back(toggler);
            } else {
                addLabel(valueText(d, v.v[i]), true, 62.f, y, 0.45f, false, 60.f);
                addButton(m_dyn, "-", 0.45f, menu_selector(MegaMenuPopup::onStep), i * 2, 105.f, y);
                addButton(m_dyn, "+", 0.45f, menu_selector(MegaMenuPopup::onStep), i * 2 + 1, 150.f, y);
            }
        }

        if (pages > 1) {
            addButton(m_dyn, "<", 0.45f, menu_selector(MegaMenuPopup::onPage), 0, -60.f, -98.f);
            addButton(m_dyn, ">", 0.45f, menu_selector(MegaMenuPopup::onPage), 1, 60.f, -98.f);
            addLabel(std::to_string(m_page + 1) + "/" + std::to_string(pages), true, 0.f, -98.f, 0.45f, false, 0.f);
        }
    }

    void buildPresets() {
        const mm::Lang lang = mm::lang();
        for (int p = 0; p < mm::kPresetCount; ++p) {
            const auto preset = static_cast<mm::Preset>(p);
            const float y = rowY(p);
            addLabel(mm::presetName(lang, preset), true, -205.f, y + 6.f, 0.42f, true, 300.f);
            addLabel(mm::presetDesc(lang, preset), false, -205.f, y - 8.f, 0.6f, true, 330.f);
            addButton(m_dyn, T(mm::S::Apply), 0.45f, menu_selector(MegaMenuPopup::onPreset), p, 165.f, y);
        }
    }

    void buildStats() {
        mm::Runtime& r = mm::rt();
        const double acc = mm::accuracyPercent(r.attempt.frames, r.attempt.deathFrames);
        const std::vector<std::string> lines = mm::statsLines(r.stats, acc, mm::lang());
        for (size_t i = 0; i < lines.size(); ++i) {
            addLabel(lines[i], false, -205.f, rowY(static_cast<int>(i)), 0.75f, true, 410.f);
        }
        addButton(m_dyn, T(mm::S::ResetStats), 0.45f, menu_selector(MegaMenuPopup::onResetStats), 0, 0.f, -98.f);
    }

    void rebuild(bool resetMessage = true) {
        clearDyn();
        addLabel("- " + tabName(m_tab) + " -", false, 0.f, 80.f, 0.8f, false, 0.f);
        if (m_tab < 5) buildFeatureRows();
        else if (m_tab == 5) buildPresets();
        else buildStats();
        if (resetMessage) showMessage(T(mm::S::TapForInfo));
    }

    // Tabs on top and the three always-visible buttons at the bottom.
    void buildChrome() {
        clearChrome();
        for (int t = 0; t < kTabs; ++t) {
            addButton(m_chrome, tabName(t), 0.4f, menu_selector(MegaMenuPopup::onTab), t,
                      -189.f + 63.f * static_cast<float>(t), 102.f);
        }
        const bool safe = mm::vals().on(mm::F::PracticeOnly);
        addButton(m_chrome, T(safe ? mm::S::SafeOn : mm::S::SafeOff), 0.4f, menu_selector(MegaMenuPopup::onSafe), 0,
                  -150.f, -124.f);
        addButton(m_chrome, T(mm::S::LanguageLabel) + ": " + mm::langName(mm::lang()), 0.4f,
                  menu_selector(MegaMenuPopup::onLang), 0, 0.f, -124.f);
        addButton(m_chrome, T(mm::S::ResetBall), 0.4f, menu_selector(MegaMenuPopup::onResetBall), 0, 150.f, -124.f);
    }

    // Not called "init": that name would override CCLayer::init() and could be called by the engine itself.
    bool setup() {
        if (!Popup::init(460.f, 280.f)) return false;
        this->setTitle("MegaMobile by EJB3");
        buildChrome();
        rebuild();
        return true;
    }

    // ---------------------------------------------------------------- callbacks

    void disarm() { m_safeArmed = false; }

    void onTab(CCObject* sender) {
        disarm();
        m_tab = static_cast<CCNode*>(sender)->getTag();
        m_page = 0;
        rebuild();
    }

    void onPage(CCObject* sender) {
        disarm();
        m_page += (static_cast<CCNode*>(sender)->getTag() == 1) ? 1 : -1;
        rebuild();
    }

    // Safe mode ON  -> OFF needs two taps (the first one only shows a warning).
    // Safe mode OFF -> ON happens right away.
    void setSafe(bool safe) {
        m_safeArmed = false;
        mm::vals().set(mm::F::PracticeOnly, safe ? 1.0 : 0.0);
        mm::saveValues();
        buildChrome();
        rebuild(false);
        showMessage(T(safe ? mm::S::SafeEnabled : mm::S::SafeDisabled));
    }

    void requestSafeToggle() {
        if (!mm::vals().on(mm::F::PracticeOnly)) {
            setSafe(true);
        } else if (!m_safeArmed) {
            m_safeArmed = true;
            rebuild(false);   // puts the toggle back to its real position
            showMessage(T(mm::S::SafeConfirm) + ". " + T(mm::S::SafeWarning));
        } else {
            setSafe(false);
        }
    }

    void onSafe(CCObject*) {
        requestSafeToggle();
    }

    void onToggle(CCObject* sender) {
        auto* toggler = static_cast<CCMenuItemToggler*>(sender);
        const int i = toggler->getTag();
        if (i == mm::idx(mm::F::PracticeOnly)) {
            requestSafeToggle();
            return;
        }
        disarm();
        // GD calls this before the toggler flips, so the new value is the opposite of isToggled().
        mm::vals().setAt(i, toggler->isToggled() ? 0.0 : 1.0);
        mm::saveValues();
    }

    void onStep(CCObject* sender) {
        disarm();
        const int tag = static_cast<CCNode*>(sender)->getTag();
        const int i = tag / 2;
        const int dir = (tag % 2 == 1) ? 1 : -1;
        mm::Values& v = mm::vals();
        v.setAt(i, mm::stepped(mm::kDefs[i], v.v[i], dir));
        mm::saveValues();
        if (i == mm::idx(mm::F::BallSize) || i == mm::idx(mm::F::BallOpacity)) refreshBall();
        rebuild(false);
    }

    void onInfo(CCObject* sender) {
        disarm();
        const int i = static_cast<CCNode*>(sender)->getTag();
        if (i < 0 || i >= mm::kCount) return;
        showMessage(std::string(mm::featureName(mm::lang(), i)) + ": " + mm::featureDesc(mm::lang(), i));
    }

    void onPreset(CCObject* sender) {
        disarm();
        const int p = static_cast<CCNode*>(sender)->getTag();
        if (p < 0 || p >= mm::kPresetCount) return;
        mm::applyPreset(mm::vals(), static_cast<mm::Preset>(p));
        mm::saveValues();
        refreshBall();
        buildChrome();
        showMessage(T(mm::S::Applied) + mm::presetName(mm::lang(), static_cast<mm::Preset>(p)));
    }

    void onLang(CCObject*) {
        disarm();
        mm::Values& v = mm::vals();
        const int next = (static_cast<int>(v.num(mm::F::Language)) + 1) % mm::kLangCount;
        v.set(mm::F::Language, next);
        mm::saveValues();
        buildChrome();
        rebuild();
    }

    void onResetBall(CCObject*) {
        disarm();
        mm::Values& v = mm::vals();
        v.set(mm::F::BallX, mm::kDefs[mm::idx(mm::F::BallX)].def);
        v.set(mm::F::BallY, mm::kDefs[mm::idx(mm::F::BallY)].def);
        mm::saveValues();
        refreshBall();
        showMessage(T(mm::S::BallReset));
    }

    void onResetStats(CCObject*) {
        disarm();
        mm::rt().stats.reset();
        mm::rt().attempt.reset();
        rebuild(false);
        showMessage(T(mm::S::StatsReset));
    }

    static void refreshBall();   // defined after the ball class

public:
    ~MegaMenuPopup() override { s_open = false; }

    static void open() {
        if (s_open) return;
        auto* ret = new MegaMenuPopup();
        if (ret->setup()) {
            ret->autorelease();
            s_open = true;
            ret->show();
        } else {
            delete ret;
        }
    }
};

// ------------------------------------------------------ the draggable floating ball

// A tap opens the menu, dragging moves the ball and the position is saved. Like GD's own
// overlay buttons it sits above everything else (touch priority -1000) and ignores touches while hidden.
class MMBall : public CCMenu {
protected:
    CCSprite* m_sprite = nullptr;
    bool m_moved = false;
    CCPoint m_start = {0.f, 0.f};   // where the touch began
    CCPoint m_grab = {0.f, 0.f};    // offset between the touch and the ball centre

    float baseScale() const {
        const float w = m_sprite->getContentSize().width;
        return (w > 0.f) ? (70.f * static_cast<float>(mm::vals().num(mm::F::BallSize)) / 100.f) / w : 0.25f;
    }

    float radius() const {
        return m_sprite->getContentSize().width * baseScale() * 0.5f + 4.f;
    }

    void place(float x, float y) {
        auto win = CCDirector::sharedDirector()->getWinSize();
        const float r = radius();
        m_sprite->setPosition({std::clamp(x, r, std::max(r, win.width - r)), std::clamp(y, r, std::max(r, win.height - r))});
    }

    bool setup() {
        if (!CCMenu::init()) return false;
        this->setPosition({0.f, 0.f});   // local coordinates = screen coordinates

        m_sprite = CCSprite::create("ball.png"_spr);
        if (!m_sprite) m_sprite = ButtonSprite::create("MM");   // fallback if the image is missing
        this->addChild(m_sprite);
        applyLook();
        return true;
    }

public:
    static MMBall* create() {
        auto* ret = new MMBall();
        if (ret->setup()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    // Size, opacity and saved position.
    void applyLook() {
        mm::Values& v = mm::vals();
        auto win = CCDirector::sharedDirector()->getWinSize();
        m_sprite->setScale(baseScale());
        m_sprite->setOpacity(static_cast<unsigned char>(v.num(mm::F::BallOpacity) * 2.55));
        place(static_cast<float>(v.num(mm::F::BallX)) / 1000.f * win.width,
              static_cast<float>(v.num(mm::F::BallY)) / 1000.f * win.height);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        if (!this->isVisible() || !m_sprite) return false;
        const CCPoint p = this->convertToNodeSpace(touch->getLocation());
        const CCPoint c = m_sprite->getPosition();
        if (ccpDistance(p, c) > radius()) return false;
        m_moved = false;
        m_start = p;
        m_grab = {c.x - p.x, c.y - p.y};
        m_sprite->setScale(baseScale() * 0.9f);
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        const CCPoint p = this->convertToNodeSpace(touch->getLocation());
        if (!m_moved && ccpDistance(p, m_start) < 8.f) return;   // small wobble is still a tap
        m_moved = true;
        place(p.x + m_grab.x, p.y + m_grab.y);
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        m_sprite->setScale(baseScale());
        if (!m_moved) {
            MegaMenuPopup::open();
        } else {
            auto win = CCDirector::sharedDirector()->getWinSize();
            const CCPoint c = m_sprite->getPosition();
            mm::Values& v = mm::vals();
            v.set(mm::F::BallX, c.x / win.width * 1000.f);
            v.set(mm::F::BallY, c.y / win.height * 1000.f);
            mm::saveValues();
        }
        m_moved = false;
    }

    void ccTouchCancelled(CCTouch*, CCEvent*) override {
        m_sprite->setScale(baseScale());
        m_moved = false;
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(this, -1000, true);
    }
};

void MegaMenuPopup::refreshBall() {
    auto* overlay = OverlayManager::get();
    if (!overlay) return;
    // Only MMBall gets the id "ball", so a plain cast is safe (and avoids RTTI tricks on a custom class).
    if (CCNode* node = overlay->getChildByID("ball"_spr)) static_cast<MMBall*>(node)->applyLook();
}

namespace mm {
    void setBallVisible(bool visible) {
        auto* overlay = OverlayManager::get();
        if (!overlay) return;
        CCNode* ball = overlay->getChildByID("ball"_spr);
        if (ball && ball->isVisible() != visible) ball->setVisible(visible);
    }
} // namespace mm

class $modify(MMMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto* overlay = OverlayManager::get();
        if (overlay && !overlay->getChildByID("ball"_spr)) {
            if (auto* ball = MMBall::create()) {
                ball->setID("ball"_spr);
                overlay->addChild(ball, 1000);
            }
        }
        return true;
    }
};
