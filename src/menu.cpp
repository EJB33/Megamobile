// MegaMobile by EJB3 - floating ball and the tabbed menu popup.
//
// The menu is deliberately made only of toggles, +/- buttons and pages: no scrolling or sliders,
// so it works the same with a finger on any screen size.

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Popup.hpp>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "state.hpp"

using namespace geode::prelude;

namespace {

    // Places the ball according to the "Ball corner / size / opacity" features.
    void refreshBall() {
        auto* overlay = OverlayManager::get();
        if (!overlay) return;
        CCNode* menu = overlay->getChildByID("ball-menu"_spr);
        if (!menu) return;
        CCNode* node = menu->getChildByID("ball"_spr);
        if (!node) return;

        mm::Values& v = mm::vals();
        auto win = CCDirector::sharedDirector()->getWinSize();
        const mm::CornerPos pos = mm::cornerPos(static_cast<int>(v.num(mm::F::BallCorner)), win.width, win.height, 34.f);
        node->setScale(static_cast<float>(v.num(mm::F::BallSize)) / 100.f);
        node->setPosition({pos.x, pos.y});

        if (auto* item = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            if (auto* image = typeinfo_cast<CCSprite*>(item->getNormalImage())) {
                image->setOpacity(static_cast<unsigned char>(v.num(mm::F::BallOpacity) * 2.55));
            }
        }
    }

    std::string valueText(mm::FeatureDef const& d, double x) {
        char buf[24];
        if (d.step >= 1.0) std::snprintf(buf, sizeof(buf), "%.0f", x);
        else if (d.step < 0.1) std::snprintf(buf, sizeof(buf), "%.2f", x);
        else std::snprintf(buf, sizeof(buf), "%.1f", x);
        return buf;
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
    std::vector<CCNode*> m_dyn;       // everything rebuilt when the tab or page changes

    static char const* tabName(int t) {
        static char const* names[kTabs] = {"Player", "Visual", "Perf", "HUD", "Misc", "Presets", "Stats"};
        return names[std::clamp(t, 0, kTabs - 1)];
    }

    std::vector<int> featuresOf(int tab) const {
        std::vector<int> out;
        for (int i = 0; i < mm::kCount; ++i) {
            if (static_cast<int>(mm::kDefs[i].cat) == tab) out.push_back(i);
        }
        return out;
    }

    void clearDyn() {
        for (CCNode* n : m_dyn) n->removeFromParent();
        m_dyn.clear();
    }

    void addLabel(std::string const& text, char const* font, float x, float y, float scale, bool left) {
        auto* label = CCLabelBMFont::create(text.c_str(), font);
        label->setScale(scale);
        if (left) label->setAnchorPoint({0.f, 0.5f});
        m_mainLayer->addChildAtPosition(label, Anchor::Center, {x, y});
        m_dyn.push_back(label);
    }

    CCMenuItemSpriteExtra* addButton(char const* text, float scale, SEL_MenuHandler handler, int tag, float x, float y) {
        auto* sprite = ButtonSprite::create(text);
        sprite->setScale(scale);
        auto* btn = CCMenuItemSpriteExtra::create(sprite, this, handler);
        btn->setTag(tag);
        m_buttonMenu->addChildAtPosition(btn, Anchor::Center, {x, y});
        m_dyn.push_back(btn);
        return btn;
    }

    void buildFeatureRows() {
        const std::vector<int> list = featuresOf(m_tab);
        const int pages = std::max(1, (static_cast<int>(list.size()) + kRows - 1) / kRows);
        m_page = std::clamp(m_page, 0, pages - 1);
        mm::Values& v = mm::vals();

        for (int row = 0; row < kRows; ++row) {
            const int k = m_page * kRows + row;
            if (k >= static_cast<int>(list.size())) break;
            const int i = list[k];
            mm::FeatureDef const& d = mm::kDefs[i];
            const float y = 55.f - 30.f * static_cast<float>(row);

            addLabel(d.name, "bigFont.fnt", -205.f, y, 0.42f, true);
            addButton("?", 0.45f, menu_selector(MegaMenuPopup::onInfo), i, 205.f, y);

            if (d.kind == mm::Kind::Toggle) {
                auto* toggler = CCMenuItemToggler::createWithStandardSprites(
                    this, menu_selector(MegaMenuPopup::onToggle), 0.55f);
                toggler->toggle(v.v[i] >= 0.5);
                toggler->setTag(i);
                m_buttonMenu->addChildAtPosition(toggler, Anchor::Center, {160.f, y});
                m_dyn.push_back(toggler);
            } else {
                addLabel(valueText(d, v.v[i]), "bigFont.fnt", 62.f, y, 0.45f, false);
                addButton("-", 0.5f, menu_selector(MegaMenuPopup::onStep), i * 2, 105.f, y);
                addButton("+", 0.5f, menu_selector(MegaMenuPopup::onStep), i * 2 + 1, 150.f, y);
            }
        }

        if (pages > 1) {
            addButton("<", 0.5f, menu_selector(MegaMenuPopup::onPage), 0, -60.f, -105.f);
            addButton(">", 0.5f, menu_selector(MegaMenuPopup::onPage), 1, 60.f, -105.f);
            const std::string text = std::to_string(m_page + 1) + "/" + std::to_string(pages);
            addLabel(text, "bigFont.fnt", 0.f, -105.f, 0.45f, false);
        }
    }

    void buildPresets() {
        for (int p = 0; p < mm::kPresetCount; ++p) {
            const auto preset = static_cast<mm::Preset>(p);
            const float y = 55.f - 30.f * static_cast<float>(p);
            addLabel(mm::presetName(preset), "bigFont.fnt", -205.f, y + 6.f, 0.42f, true);
            addLabel(mm::presetDesc(preset), "chatFont.fnt", -205.f, y - 8.f, 0.6f, true);
            addButton("Apply", 0.5f, menu_selector(MegaMenuPopup::onPreset), p, 165.f, y);
        }
    }

    void buildStats() {
        mm::Runtime& r = mm::rt();
        const double acc = mm::accuracyPercent(r.attempt.frames, r.attempt.deathFrames);
        const std::vector<std::string> lines = mm::statsLines(r.stats, acc);
        for (size_t i = 0; i < lines.size(); ++i) {
            addLabel(lines[i], "chatFont.fnt", -205.f, 55.f - 26.f * static_cast<float>(i), 0.75f, true);
        }
        addButton("Reset stats", 0.5f, menu_selector(MegaMenuPopup::onResetStats), 0, 0.f, -105.f);
    }

    void rebuild() {
        clearDyn();
        addLabel(std::string("- ") + tabName(m_tab) + " -", "chatFont.fnt", 0.f, 78.f, 0.8f, false);
        if (m_tab < 5) buildFeatureRows();
        else if (m_tab == 5) buildPresets();
        else buildStats();
    }

    // Not called "init": that name would override CCLayer::init() and could be called by the engine itself.
    bool setup() {
        if (!Popup::init(460.f, 280.f)) return false;
        this->setTitle("MegaMobile by EJB3");

        // The tab buttons never change, so they live outside the rebuilt group.
        for (int t = 0; t < kTabs; ++t) {
            auto* sprite = ButtonSprite::create(tabName(t));
            sprite->setScale(0.5f);
            auto* btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MegaMenuPopup::onTab));
            btn->setTag(t);
            m_buttonMenu->addChildAtPosition(btn, Anchor::Center, {-189.f + 63.f * static_cast<float>(t), 102.f});
        }

        rebuild();
        return true;
    }

    // ---------------------------------------------------------------- callbacks

    void onTab(CCObject* sender) {
        m_tab = static_cast<CCNode*>(sender)->getTag();
        m_page = 0;
        rebuild();
    }

    void onPage(CCObject* sender) {
        m_page += (static_cast<CCNode*>(sender)->getTag() == 1) ? 1 : -1;
        rebuild();
    }

    void afterChange(int i) {
        mm::saveValues();
        if (i == mm::idx(mm::F::BallCorner) || i == mm::idx(mm::F::BallSize) || i == mm::idx(mm::F::BallOpacity)) {
            refreshBall();
        }
    }

    void onToggle(CCObject* sender) {
        auto* toggler = static_cast<CCMenuItemToggler*>(sender);
        const int i = toggler->getTag();
        // GD calls this before the toggler flips, so the new value is the opposite of isToggled().
        mm::vals().setAt(i, toggler->isToggled() ? 0.0 : 1.0);
        afterChange(i);
    }

    void onStep(CCObject* sender) {
        const int tag = static_cast<CCNode*>(sender)->getTag();
        const int i = tag / 2;
        const int dir = (tag % 2 == 1) ? 1 : -1;
        mm::Values& v = mm::vals();
        v.setAt(i, mm::stepped(mm::kDefs[i], v.v[i], dir));
        afterChange(i);
        rebuild();
    }

    void onInfo(CCObject* sender) {
        const int i = static_cast<CCNode*>(sender)->getTag();
        if (i < 0 || i >= mm::kCount) return;
        FLAlertLayer::create(mm::kDefs[i].name, std::string(mm::kDefs[i].desc), "OK")->show();
    }

    void onPreset(CCObject* sender) {
        const int p = static_cast<CCNode*>(sender)->getTag();
        if (p < 0 || p >= mm::kPresetCount) return;
        mm::applyPreset(mm::vals(), static_cast<mm::Preset>(p));
        mm::saveValues();
        refreshBall();
        Notification::create(std::string("Applied: ") + mm::presetName(static_cast<mm::Preset>(p)),
                             NotificationIcon::Success, 1.5f)->show();
    }

    void onResetStats(CCObject*) {
        mm::rt().stats.reset();
        mm::rt().attempt.reset();
        rebuild();
    }

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

// ------------------------------------------------------------ the floating ball

// The ball outlives the main menu, so its target cannot be a layer: this tiny object lives forever.
class MMBallTarget : public CCObject {
public:
    static MMBallTarget* get() {
        static MMBallTarget* instance = new MMBallTarget();
        return instance;
    }

    void onOpen(CCObject*) {
        MegaMenuPopup::open();
    }
};

class $modify(MMMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto* overlay = OverlayManager::get();
        if (overlay && !overlay->getChildByID("ball-menu"_spr)) {
            CCSprite* art = CCSprite::create("ball.png"_spr);
            CCNode* shown = art ? static_cast<CCNode*>(art) : static_cast<CCNode*>(ButtonSprite::create("MM"));
            auto* btn = CCMenuItemSpriteExtra::create(shown, MMBallTarget::get(), menu_selector(MMBallTarget::onOpen));
            btn->setID("ball"_spr);

            auto* menu = CCMenu::create();
            menu->setID("ball-menu"_spr);
            menu->setPosition({0.f, 0.f});
            menu->addChild(btn);
            overlay->addChild(menu, 1000);
            refreshBall();
        }
        return true;
    }
};
