// MegaMobile by EJB3 - game hooks. Every hook only reads mm::vals(), so a feature that is off
// costs one array lookup.

#include <Geode/Geode.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/CameraTriggerGameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
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
        in.cheatsOn = mm::anyCheatEnabled(v) && mm::cheatsOk(pl);
        const std::string text = mm::hudText(in);

        CCNode* node = pl->m_uiLayer->getChildByID("hud"_spr);
        if (text.empty()) {
            if (node) node->removeFromParent();
            return;
        }

        CCLabelBMFont* label = node ? typeinfo_cast<CCLabelBMFont*>(node) : nullptr;
        if (!label) {
            if (node) node->removeFromParent();
            label = CCLabelBMFont::create(text.c_str(), "chatFont.fnt");
            label->setID("hud"_spr);
            label->setScale(0.45f);
            label->setOpacity(210);
            label->setAnchorPoint({0.f, 1.f});
            auto win = CCDirector::sharedDirector()->getWinSize();
            label->setPosition({6.f, win.height - 6.f});
            pl->m_uiLayer->addChild(label, 1000);
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

        r.hudTimer += dt;
        if (r.hudTimer >= 0.25) {
            r.hudTimer = 0.0;
            updateHud(pl);
        }
    }

} // namespace

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
        return PlayLayer::init(level, useReplay, dontCreateObjects);
    }

    void resetLevel() {
        mm::Runtime& r = mm::rt();
        r.stats.onAttempt();
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
