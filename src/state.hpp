// MegaMobile by EJB3 - shared runtime state (uses Geode only for the save folder and PlayLayer).
#pragma once

#include <Geode/Geode.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

#include "logic.hpp"

using namespace geode::prelude;

namespace mm {

    struct Runtime {
        Values values;
        bool loaded = false;

        SessionStats stats;
        Attempt attempt;
        bool blockedThisFrame = false;   // a death was blocked by noclip during this frame
        bool deathCounted = false;       // the death of this attempt is already in the stats

        FpsMeter meter;
        FpsSample lastFps;
        Pressure pressure = Pressure::None;
        bool ecoActive = false;
        double hudTimer = 0.0;
        Lang hudLang = Lang::En;     // language the HUD label was created with

        MacroSession macro;
        std::array<bool, 6> macroHeld{};   // buttons the macro is holding down: (button-1) + 3 for player 2
    };

    inline Runtime& rt() {
        static Runtime r;
        return r;
    }

    inline std::filesystem::path configPath() {
        return geode::Mod::get()->getSaveDir() / "megamobile-config.txt";
    }

    inline void loadValues() {
        Runtime& r = rt();
        r.loaded = true;
        std::ifstream in(configPath());
        if (!in) return;   // first run: defaults
        std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        parse(r.values, text);
    }

    inline void saveValues() {
        std::error_code ec;
        std::filesystem::create_directories(geode::Mod::get()->getSaveDir(), ec);
        std::ofstream out(configPath(), std::ios::trunc);
        if (out) out << serialize(rt().values);
    }

    inline Values& vals() {
        Runtime& r = rt();
        if (!r.loaded) loadValues();
        return r.values;
    }

    inline Lang lang() {
        return langOf(vals());
    }

    // Defined in hooks.cpp: lets go of every button the macro was holding (when it stops).
    void releaseMacroInputs();

    // Defined in menu.cpp: shows or hides the floating ball (it is hidden while you play a level).
    void setBallVisible(bool visible);

    // Our own generated font shows Spanish accents and Cyrillic; English keeps the game's fonts.
    // If the generated font is not available for any reason, the game's font is used instead.
    inline CCLabelBMFont* makeLabel(char const* text, bool big) {
        CCLabelBMFont* label = nullptr;
        if (lang() != Lang::En) label = CCLabelBMFont::create(text, "MegaFont.fnt"_spr);
        if (!label) label = CCLabelBMFont::create(text, big ? "bigFont.fnt" : "chatFont.fnt");
        return label;
    }

    inline float fontScale() {
        return lang() == Lang::En ? 1.f : 0.75f;
    }

    inline char const* buttonFont() {
        return lang() == Lang::En ? "bigFont.fnt" : "MegaFont.fnt"_spr;
    }

    // Cheats only act inside a level, and (by default) only in Practice or Test mode.
    inline bool cheatsOk(PlayLayer* pl) {
        if (!pl) return false;
        return cheatsAllowed(vals().on(F::PracticeOnly), pl->m_isPracticeMode, pl->m_isTestMode);
    }

    inline bool effNoShaders() {
        Runtime& r = rt();
        return vals().on(F::NoShaders) || (vals().on(F::Eco) && r.ecoActive);
    }

    inline bool effNoParticles() {
        Runtime& r = rt();
        return vals().on(F::NoParticles) || (vals().on(F::Eco) && r.ecoActive);
    }

} // namespace mm
