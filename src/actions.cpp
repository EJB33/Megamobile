// MegaMod by EJB3 - actions that are shared by the menu and the keyboard:
// short on-screen messages, keys, start positions, macro switches and the click circles.

#include <Geode/Geode.hpp>

#include <algorithm>
#include <string>

#include "state.hpp"

using namespace geode::prelude;

namespace {

    std::string T(mm::S s) {
        return mm::tr(mm::lang(), s);
    }

    // "2/5  x=1234" or the normal start of the level.
    std::string startPosText(int index) {
        mm::Runtime& r = mm::rt();
        if (index < 0) return T(mm::S::SpStart);
        const int count = static_cast<int>(r.startPositions.size());
        std::string text = std::to_string(index + 1) + "/" + std::to_string(count);
        if (index < count && r.startPositions[static_cast<size_t>(index)]) {
            text += "  x=" + std::to_string(static_cast<int>(r.startPositions[static_cast<size_t>(index)]->getPositionX()));
        }
        return text;
    }

    // Do what a key is bound to. Held actions (jump) react to press and release; the rest only to a fresh press.
    void perform(int action, bool down, bool repeat) {
        mm::ActionRef const& ref = mm::actions()[static_cast<size_t>(action)];
        if (ref.fixed) {
            switch (static_cast<mm::FixedAction>(ref.index)) {
                case mm::FixedAction::JumpP1:
                case mm::FixedAction::JumpP2:
                    if (!repeat) {
                        if (PlayLayer* pl = PlayLayer::get()) {
                            pl->queueButton(1, down, ref.index == static_cast<int>(mm::FixedAction::JumpP2), 0.0);
                        }
                    }
                    break;
                case mm::FixedAction::OpenMenu:
                    if (down && !repeat) mm::openMenuNow();
                    break;
                case mm::FixedAction::Panic:
                    if (down && !repeat) mm::applyPanic();
                    break;
                case mm::FixedAction::MacroRecord:
                    if (down && !repeat) mm::toast(mm::macroToggleRecord());
                    break;
                case mm::FixedAction::MacroPlay:
                    if (down && !repeat) mm::toast(mm::macroTogglePlay());
                    break;
                case mm::FixedAction::StartPosPrev:
                    if (down && !repeat) mm::requestStartPos(-1);
                    break;
                case mm::FixedAction::StartPosNext:
                    if (down && !repeat) mm::requestStartPos(1);
                    break;
                default:
                    break;
            }
            return;
        }
        if (down && !repeat) mm::toggleFeatureByKey(ref.index);
    }

} // namespace

namespace mm {

    // ------------------------------------------------------------------- toast

    void toast(std::string const& text) {
        auto* overlay = OverlayManager::get();
        if (!overlay) return;
        if (CCNode* old = overlay->getChildByID("toast"_spr)) old->removeFromParent();

        CCLabelBMFont* label = makeLabel(text.c_str(), false);
        if (!label) return;
        label->setID("toast"_spr);
        auto win = CCDirector::sharedDirector()->getWinSize();
        const float scale = 0.6f * fontScale();
        label->setScale(scale);
        label->limitLabelWidth(win.width - 40.f, scale, 0.2f);
        label->setPosition({win.width / 2.f, win.height - 28.f});
        label->runAction(CCSequence::create(
            CCDelayTime::create(1.6f),
            CCFadeOut::create(0.4f),
            CCCallFunc::create(label, callfunc_selector(CCNode::removeFromParent)),
            nullptr));
        overlay->addChild(label, 3000);
    }

    // ------------------------------------------------------------------- macro

    std::string macroToggleRecord() {
        MacroSession& ms = rt().macro;
        if (ms.mode == MacroMode::Record) {
            ms.stop();
            return T(S::MacroStopped);
        }
        ms.stop();
        releaseMacroInputs();
        ms.mode = MacroMode::Record;
        return T(S::MacroRestartHint);
    }

    std::string macroTogglePlay() {
        MacroSession& ms = rt().macro;
        if (ms.mode == MacroMode::Play) {
            ms.stop();
            releaseMacroInputs();
            return T(S::MacroStopped);
        }
        if (ms.macro.events.empty()) return T(S::MacroEmpty);
        ms.stop();
        ms.mode = MacroMode::Play;
        ms.playIndex = 0;
        std::string text = T(S::MacroRestartHint);
        if (vals().on(F::PracticeOnly)) text += ". " + T(S::MacroNeedPractice);
        return text;
    }

    void applyPanic() {
        rt().macro.stop();
        releaseMacroInputs();
        applyPreset(vals(), Preset::Panic);
        saveValues();
        toast(presetName(lang(), Preset::Panic));
        notifyMenuRefresh();
    }

    // -------------------------------------------------------------- start positions

    // Switches to a StartPos (-1 = the normal start). Same steps the practice/test mode of the game needs.
    void applyStartIndex(PlayLayer* pl, int index) {
        Runtime& r = rt();
        if (!pl || pl->m_levelEndAnimationStarted) return;
        const int count = static_cast<int>(r.startPositions.size());
        if (count == 0) return;
        r.startIndex = std::clamp(index, -1, count - 1);
        StartPosObject* obj = r.startIndex < 0 ? nullptr : r.startPositions[static_cast<size_t>(r.startIndex)];

        const bool practice = pl->m_isPracticeMode;
        if (practice) pl->togglePracticeMode(false);
        pl->m_isTestMode = (obj != nullptr);   // test mode never saves progress
        pl->m_currentCheckpoint = nullptr;
        pl->setStartPosObject(obj);
        pl->resetLevel();
        pl->resetCamera();
        pl->startMusic();
        if (practice) pl->togglePracticeMode(true);
    }

    void requestStartPos(int delta) {
        Runtime& r = rt();
        PlayLayer* pl = PlayLayer::get();
        if (!pl) {
            toast(T(S::SpNeedLevel));
            return;
        }
        const int count = static_cast<int>(r.startPositions.size());
        if (count == 0) {
            toast(T(S::SpNone));
            return;
        }
        const int base = (r.pendingStart != kNoPendingStart) ? r.pendingStart : r.startIndex;
        const int target = nextStartIndex(base, delta, count);

        if (pl->m_isPaused) {
            // The level is paused: remember the choice and apply it as soon as the game is resumed.
            r.pendingStart = target;
            toast(T(S::SpCurrent) + ": " + startPosText(target) + " (" + T(S::SpPending) + ")");
        } else {
            applyStartIndex(pl, target);
            toast(T(S::SpCurrent) + ": " + startPosText(target));
        }
        notifyMenuRefresh();
    }

    // ------------------------------------------------------------------ keyboard

    void toggleFeatureByKey(int featureIndex) {
        if (featureIndex < 0 || featureIndex >= kCount) return;
        Values& v = vals();
        const bool nowOn = !(v.v[static_cast<size_t>(featureIndex)] >= 0.5);
        v.setAt(featureIndex, nowOn ? 1.0 : 0.0);
        saveValues();
        toast(std::string(featureName(lang(), featureIndex)) + ": " + tr(lang(), nowOn ? S::On : S::Off));
        notifyMenuRefresh();
    }

    void startKeyCapture(int action) {
        rt().captureAction = action;
    }

    bool handleKey(int key, bool down, bool repeat) {
        Runtime& r = rt();

        // The menu is waiting for a key: the next press is the new key (Escape cancels). Nothing else sees it.
        if (r.captureAction >= 0) {
            if (down && !repeat) {
                const int action = r.captureAction;
                r.captureAction = -1;
                int displaced = -1;
                int given = 0;
                if (keyAllowed(key)) {
                    displaced = r.keys.assign(action, key);
                    given = key;
                    saveValues();
                }
                notifyKeysChanged(action, given, displaced);
            }
            return true;
        }

        const int action = r.keys.actionFor(key);
        if (action < 0) return false;
        perform(action, down, repeat);
        return true;   // a bound key no longer does its normal game action
    }

    // --------------------------------------------------------------- click circles

    void onTouchesBegan(CCSet* touches) {
        Values& v = vals();
        if (!touches || !v.on(F::ShowClicks)) return;
        auto* overlay = OverlayManager::get();
        if (!overlay) return;
        const ClickLook look = clickLook(v);

        for (auto it = touches->begin(); it != touches->end(); ++it) {
            auto* touch = static_cast<CCTouch*>(*it);
            if (!touch) continue;
            CCSprite* dot = CCSprite::create("click.png"_spr);
            if (!dot) return;   // image missing: nothing to draw

            const float width = dot->getContentSize().width;
            const float base = width > 0.f ? look.diameter / width : 0.3f;
            dot->setPosition(touch->getLocation());
            dot->setColor(ccc3(static_cast<GLubyte>(look.r), static_cast<GLubyte>(look.g), static_cast<GLubyte>(look.b)));
            dot->setOpacity(static_cast<GLubyte>(look.opacity));
            dot->setScale(base);
            dot->runAction(CCSequence::create(
                CCSpawn::create(CCScaleTo::create(look.seconds, base * 1.5f), CCFadeOut::create(look.seconds), nullptr),
                CCCallFunc::create(dot, callfunc_selector(CCNode::removeFromParent)),
                nullptr));
            overlay->addChild(dot, 2500);
        }
    }

} // namespace mm
