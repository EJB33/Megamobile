// MegaMobile by EJB3 - game hooks. Every hook only reads mm::vals(), so a feature that is off
// costs one array lookup.

#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CameraTriggerGameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/ShaderLayer.hpp>

#include "state.hpp"

using namespace geode::prelude;

namespace {

    // The HUD is one label in the top-left corner of the level UI.
    void updateHud(PlayLayer* pl) {
        mm::Runtime& r = mm::rt();
        mm::Values& v = mm::vals();
        if (!pl->m_uiLayer) return;

        mm::HudInput in;
        in.fps = v.on(mm::F::HudFps);
        in.stats = v.on(mm::F::HudStats);
        in.acc = v.on(mm::F::HudAcc);
        in.cheats = v.on(mm::F::HudCheats);
        in.sample = r.lastFps;
        in.session = &r.stats;
        in.accuracy = mm::accuracyPercent(r.attempt.frames, r.attempt.deathFrames);
        const bool playing = r.macro.mode == mm::MacroMode::Play;
        in.cheatsOn = (mm::anyCheatEnabled(v) || playing) && mm::cheatsOk(pl);
        in.lang = mm::lang();
        in.macro = mm::macroHudText(r.macro, playing && !mm::cheatsOk(pl), in.lang);
        const std::string text = mm::hudText(in);

        CCNode* node = pl->m_uiLayer->getChildByID("hud"_spr);
        if (text.empty()) {
            if (node) node->removeFromParent();
            return;
        }

        CCLabelBMFont* label = node ? typeinfo_cast<CCLabelBMFont*>(node) : nullptr;
        // The font depends on the language, so a label made in another language is replaced.
        if (label && r.hudLang != in.lang) {
            label->removeFromParent();
            label = nullptr;
            node = nullptr;
        }
        if (!label) {
            if (node) node->removeFromParent();
            label = mm::makeLabel(text.c_str(), false);
            label->setID("hud"_spr);
            label->setScale(0.45f * mm::fontScale());
            label->setOpacity(210);
            label->setAnchorPoint({0.f, 1.f});
            auto win = CCDirector::sharedDirector()->getWinSize();
            label->setPosition({6.f, win.height - 6.f});
            pl->m_uiLayer->addChild(label, 1000);
            r.hudLang = in.lang;
        }
        label->setString(text.c_str());
    }

    void onFrame(PlayLayer* pl, float dt) {
        if (!(dt > 0.f)) return;
        mm::Runtime& r = mm::rt();
        mm::Values& v = mm::vals();

        r.attempt.frames++;
        if (r.blockedThisFrame) {
            r.attempt.deathFrames++;
            r.blockedThisFrame = false;
        }
        r.stats.addTime(dt);

        mm::FpsSample sample;
        if (r.meter.push(dt, 0.5f, sample)) {
            r.lastFps = sample;
            r.pressure = mm::updatePressure(r.pressure, sample.fps, static_cast<int>(v.num(mm::F::EcoTarget)));
        }
        r.ecoActive = v.on(mm::F::Eco) && r.pressure == mm::Pressure::Low;

        if (r.pendingStart != mm::kNoPendingStart && !pl->m_isPaused) {
            const int target = r.pendingStart;
            r.pendingStart = mm::kNoPendingStart;
            geode::queueInMainThread([target] {
                if (PlayLayer* now = PlayLayer::get()) mm::applyStartIndex(now, target);
            });
        }

        r.hudTimer += dt;
        if (r.hudTimer >= 0.25) {
            r.hudTimer = 0.0;
            updateHud(pl);
            if (!pl->m_isPaused) mm::setBallVisible(false);   // the ball only shows in menus and in the pause menu
        }
    }

} // namespace

namespace mm {
    void releaseMacroInputs() {
        Runtime& r = rt();
        PlayLayer* pl = PlayLayer::get();
        for (size_t i = 0; i < r.macroHeld.size(); ++i) {
            if (!r.macroHeld[i]) continue;
            r.macroHeld[i] = false;
            if (pl) pl->handleButton(false, static_cast<int>(i % 3) + 1, i < 3);
        }
    }
} // namespace mm

// ------------------------------------------------------------ PlayLayer

class $modify(MMPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        mm::Runtime& r = mm::rt();
        r.stats.reset();
        r.attempt.reset();
        r.blockedThisFrame = false;
        r.deathCounted = false;
        r.meter.reset();
        r.pressure = mm::Pressure::None;
        r.ecoActive = false;
        r.startPositions.clear();
        r.startIndex = -1;
        r.pendingStart = mm::kNoPendingStart;
        mm::setBallVisible(false);
        return PlayLayer::init(level, useReplay, dontCreateObjects);
    }

    void onQuit() {
        mm::Runtime& r = mm::rt();
        r.startPositions.clear();   // the objects die with the level
        r.startIndex = -1;
        r.pendingStart = mm::kNoPendingStart;
        mm::setBallVisible(true);
        PlayLayer::onQuit();
    }

    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        mm::Runtime& r = mm::rt();
        std::sort(r.startPositions.begin(), r.startPositions.end(),
                  [](StartPosObject* a, StartPosObject* b) { return a->getPositionX() < b->getPositionX(); });
        r.startIndex = -1;
        if (m_startPosObject) {
            for (size_t i = 0; i < r.startPositions.size(); ++i) {
                if (r.startPositions[i] == m_startPosObject) {
                    r.startIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    }

    void resetLevel() {
        mm::Runtime& r = mm::rt();
        r.stats.onAttempt();
        r.macro.startAttempt();                 // recording starts over, playback goes back to the first click
        if (r.macro.mode == mm::MacroMode::Play) r.macroHeld.fill(false);
        r.attempt.reset();
        r.blockedThisFrame = false;
        r.deathCounted = false;
        PlayLayer::resetLevel();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        onFrame(this, dt);
    }

    void addObject(GameObject* obj) {
        if (obj && mm::vals().on(mm::F::NoGlow)) obj->m_hasNoGlow = true;
        if (obj && obj->m_objectID == 31) mm::rt().startPositions.push_back(static_cast<StartPosObject*>(obj));
        PlayLayer::addObject(obj);
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        mm::Runtime& r = mm::rt();
        mm::Values& v = mm::vals();

        if (v.on(mm::F::Noclip) && mm::cheatsOk(this)) {
            r.blockedThisFrame = true;
            if (mm::noclipBlocksDeath(v.num(mm::F::NoclipMinAcc), r.attempt.frames, r.attempt.deathFrames)) {
                return;   // survive
            }
        }

        const double percent = this->getCurrentPercent();
        PlayLayer::destroyPlayer(player, obj);

        if (!r.deathCounted) {
            r.deathCounted = true;
            r.stats.onDeath(percent);
        }

        if (v.on(mm::F::Respawn)) {
            if (auto* respawn = this->getActionByTag(0x10)) {
                this->stopAction(respawn);
                auto* delayed = CCSequence::create(
                    CCDelayTime::create(static_cast<float>(v.num(mm::F::RespawnTime))),
                    CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr);
                delayed->setTag(0x10);
                this->runAction(delayed);
            }
        }
    }
};

// ------------------------------------------------- jump hack (inside a level)

class $modify(MMBaseGameLayer, GJBaseGameLayer) {
    // Records your clicks while a macro is recording; while one is playing (and allowed), real clicks are ignored.
    void handleButton(bool down, int button, bool isPlayer1) {
        mm::Runtime& r = mm::rt();
        PlayLayer* pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this) {
            if (r.macro.mode == mm::MacroMode::Play && mm::cheatsOk(pl)) return;
            if (r.macro.mode == mm::MacroMode::Record) {
                mm::Values& v = mm::vals();
                const int shift = v.on(mm::F::MacroExtra) ? static_cast<int>(v.num(mm::F::MacroExtraTicks)) : 0;
                r.macro.record(mm::tickFromTime(m_gameState.m_levelTime), button, !isPlayer1, down, shift);
            }
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

    // Plays the recorded clicks at the start of every full physics tick.
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        mm::Runtime& r = mm::rt();
        if (!isHalfTick && r.macro.mode == mm::MacroMode::Play) {
            PlayLayer* pl = PlayLayer::get();
            if (pl && static_cast<GJBaseGameLayer*>(pl) == this && mm::cheatsOk(pl)) {
                const uint32_t tick = mm::tickFromTime(m_gameState.m_levelTime);
                for (auto const& e : r.macro.due(tick)) {
                    r.macroHeld[static_cast<size_t>((e.button - 1) + (e.player2 ? 3 : 0))] = e.down;
                    GJBaseGameLayer::handleButton(e.down, e.button, !e.player2);
                }
            }
        }
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
    }

    void update(float dt) {
        PlayLayer* pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this && mm::vals().on(mm::F::JumpHack) && mm::cheatsOk(pl)) {
            if (m_player1) m_player1->m_isOnGround = true;
            if (m_player2) m_player2->m_isOnGround = true;
        }
        GJBaseGameLayer::update(dt);
    }

    bool canBeActivatedByPlayer(PlayerObject* player, EffectGameObject* effect) {
        if (effect && mm::vals().on(mm::F::NoMirror) && effect->m_objectType == GameObjectType::InverseMirrorPortal) {
            return false;
        }
        return GJBaseGameLayer::canBeActivatedByPlayer(player, effect);
    }
};

// ----------------------------------------------------------- game speed

class $modify(MMScheduler, cocos2d::CCScheduler) {
    void update(float dt) {
        mm::Values& v = mm::vals();
        if (v.num(mm::F::Speed) < 0.999) {
            PlayLayer* pl = PlayLayer::get();
            if (pl && !pl->m_isPaused) {
                dt *= static_cast<float>(mm::speedFactor(v, mm::cheatsOk(pl)));
            }
        }
        CCScheduler::update(dt);
    }
};

// -------------------------------------------------------------- visuals

class $modify(MMShaderLayer, ShaderLayer) {
    void performCalculations() {
        if (mm::effNoShaders()) {
            m_state.m_usesShaders = false;
            return;
        }
        ShaderLayer::performCalculations();
    }
};

class $modify(MMParticles, CCParticleSystemQuad) {
    static CCParticleSystemQuad* create(const char* plistFile, bool idk) {
        auto* particle = cocos2d::CCParticleSystemQuad::create(plistFile, idk);
        if (particle && mm::effNoParticles()) {
            particle->setTotalParticles(0);
            particle->stopSystem();
            particle->setVisible(false);
        }
        return particle;
    }
};

class $modify(MMPlayerObject, PlayerObject) {
    void playDeathEffect() {
        if (mm::vals().on(mm::F::NoDeathFx)) return;
        PlayerObject::playDeathEffect();
    }

    void toggleVisibility(bool visible) {
        PlayerObject::toggleVisibility(mm::vals().on(mm::F::HidePlayer) ? false : visible);
    }
};

class $modify(MMCameraTrigger, CameraTriggerGameObject) {
    void triggerObject(GJBaseGameLayer* layer, int uniqueID, gd::vector<int> const* remapKeys) {
        if (mm::vals().on(mm::F::NoCamera)) return;
        CameraTriggerGameObject::triggerObject(layer, uniqueID, remapKeys);
    }
};

// ------------------------------------------------------------- performance

class $modify(MMLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (level && mm::vals().on(mm::F::AutoLdm)) level->m_lowDetailModeToggled = true;
        return true;
    }
};

// -------------------------------------------------------------------- misc

class $modify(MMGameManager, GameManager) {
    bool isColorUnlocked(int key, UnlockType type) {
        if (mm::vals().on(mm::F::UnlockIcons)) return true;
        return GameManager::isColorUnlocked(key, type);
    }

    bool isIconUnlocked(int key, IconType type) {
        if (mm::vals().on(mm::F::UnlockIcons)) return true;
        return GameManager::isIconUnlocked(key, type);
    }
};

// ------------------------------------------------ the ball comes back in the pause menu

class $modify(MMPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        if (mm::vals().on(mm::F::BallInPause)) mm::setBallVisible(true);
    }
};

// ------------------------------------------------------- keyboard and touches

// Every key press goes through here first: bound keys are used by the mod and swallowed.
class $modify(MMKeyboard, cocos2d::CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool repeat, double timestamp) {
        if (mm::handleKey(static_cast<int>(key), down, repeat)) return true;
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, timestamp);
    }
};

// Sees every touch that begins (to draw the click circle) and always lets it through.
class $modify(MMTouches, cocos2d::CCTouchDispatcher) {
    void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int type) {
        if (type == cocos2d::CCTOUCHBEGAN) mm::onTouchesBegan(touches);
        CCTouchDispatcher::touches(touches, event, type);
    }
};
