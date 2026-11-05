// MegaMobile by EJB3 - pure C++ logic (no Geode types), unit tested on any PC.
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace mm {

    // ------------------------------------------------------------------ features

    // Order here MUST match the order of kDefs below (a test checks it).
    enum class F : int {
        // Player
        Noclip, NoclipMinAcc, JumpHack, Speed, Respawn, RespawnTime,
        // Visual
        NoGlow, NoShaders, NoParticles, NoDeathFx, HidePlayer, NoCamera, NoMirror,
        // Performance
        AutoLdm, Eco, EcoTarget,
        // HUD
        HudFps, HudStats, HudAcc, HudCheats,
        // Misc
        UnlockIcons, PracticeOnly, BallSize, BallOpacity, BallInPause,
        // Macro options
        MacroExtra, MacroExtraTicks,
        // Hidden (not shown as rows: saved position of the ball and the language)
        BallX, BallY, Language,
        Count
    };
    constexpr int kCount = static_cast<int>(F::Count);
    constexpr int idx(F f) { return static_cast<int>(f); }

    enum class Cat { Player, Visual, Perf, Hud, Misc, Macro, Hidden };
    enum class Kind { Toggle, Number };

    struct FeatureDef {
        const char* key;    // key used in the config file
        const char* name;   // short name shown in the menu
        const char* desc;   // text of the "?" button
        Cat cat;
        Kind kind;
        double def, min, max, step;
    };

    inline constexpr std::array<FeatureDef, kCount> kDefs = {{
        // ---- Player
        {"noclip", "Noclip", "The player survives obstacles. Counts as a cheat.", Cat::Player, Kind::Toggle, 0, 0, 1, 1},
        {"noclip_min_acc", "Noclip min accuracy %", "If your noclip accuracy falls under this value you can die again. 0 = never die.", Cat::Player, Kind::Number, 0, 0, 100, 5},
        {"jump_hack", "Jump hack", "Lets you jump in the air. Counts as a cheat.", Cat::Player, Kind::Toggle, 0, 0, 1, 1},
        {"speed", "Game speed", "Slow motion for practice, from 0.10 to 1.00. Anything under 1.00 counts as a cheat.", Cat::Player, Kind::Number, 1.0, 0.1, 1.0, 0.05},
        {"respawn", "Custom respawn time", "Changes how long you wait to respawn after dying.", Cat::Player, Kind::Toggle, 0, 0, 1, 1},
        {"respawn_time", "Respawn time (s)", "Seconds to wait before respawning (needs Custom respawn time).", Cat::Player, Kind::Number, 0.5, 0.1, 3.0, 0.1},
        // ---- Visual
        {"no_glow", "No glow", "Removes glow from objects placed from now on (restart the level to apply).", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"no_shaders", "No shaders", "Turns shader effects off in levels.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"no_particles", "No particles", "Stops particle effects from showing.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"no_death_fx", "No death effect", "Skips the death explosion.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"hide_player", "Hide player", "Hides the player icon.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"no_camera", "No camera triggers", "Ignores camera triggers.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        {"no_mirror", "No mirror", "Disables level mirroring.", Cat::Visual, Kind::Toggle, 0, 0, 1, 1},
        // ---- Performance
        {"auto_ldm", "Auto Low Detail", "Turns on Low Detail Mode when you open a level page.", Cat::Perf, Kind::Toggle, 0, 0, 1, 1},
        {"eco", "Eco mode (auto)", "When your FPS falls under the target, shaders and new particles are switched off until it recovers.", Cat::Perf, Kind::Toggle, 0, 0, 1, 1},
        {"eco_target", "Eco target FPS", "FPS under which Eco mode starts.", Cat::Perf, Kind::Number, 50, 20, 240, 5},
        // ---- HUD
        {"hud_fps", "Show FPS", "FPS and frame time in the corner of the level.", Cat::Hud, Kind::Toggle, 0, 0, 1, 1},
        {"hud_stats", "Show session stats", "Attempts, deaths and furthest death.", Cat::Hud, Kind::Toggle, 0, 0, 1, 1},
        {"hud_acc", "Show noclip accuracy", "Percentage of frames without touching a hazard.", Cat::Hud, Kind::Toggle, 0, 0, 1, 1},
        {"hud_cheats", "Cheat indicator", "Shows CHEATS ON while a cheat feature is active.", Cat::Hud, Kind::Toggle, 1, 0, 1, 1},
        // ---- Misc
        {"unlock_icons", "Unlock all icons", "Makes every icon and color usable on this device.", Cat::Misc, Kind::Toggle, 0, 0, 1, 1},
        {"practice_only", "Safe mode", "ON: noclip, jump hack and game speed only work in Practice or Test mode. OFF: they work everywhere.", Cat::Misc, Kind::Toggle, 1, 0, 1, 1},
        {"ball_size", "Ball size %", "Size of the floating menu ball.", Cat::Misc, Kind::Number, 60, 30, 120, 10},
        {"ball_opacity", "Ball opacity %", "How solid the floating menu ball is.", Cat::Misc, Kind::Number, 80, 20, 100, 10},
        {"ball_in_pause", "Ball in pause menu", "Shows the ball when you pause a level, so you can open the menu. The ball is hidden while you play.", Cat::Misc, Kind::Toggle, 1, 0, 1, 1},
        // ---- Macro
        {"macro_extra", "Click extrapolation", "While recording, your clicks are extrapolated back in time to make up for input delay. Set the amount in the next option.", Cat::Macro, Kind::Toggle, 0, 0, 1, 1},
        {"macro_extra_ticks", "Extrapolation ticks", "How many ticks earlier each recorded click is saved (1 tick = 4.2 ms).", Cat::Macro, Kind::Number, 2, 0, 12, 1},
        // ---- Hidden
        {"ball_x", "Ball position X", "Saved horizontal position of the ball.", Cat::Hidden, Kind::Number, 60, 0, 1000, 1},
        {"ball_y", "Ball position Y", "Saved vertical position of the ball.", Cat::Hidden, Kind::Number, 150, 0, 1000, 1},
        {"language", "Language", "0 English, 1 Spanish, 2 Russian.", Cat::Hidden, Kind::Number, 0, 0, 2, 1},
    }};

    inline double clampStep(FeatureDef const& d, double x) {
        if (!std::isfinite(x)) x = d.def;
        if (d.kind == Kind::Toggle) return x >= 0.5 ? 1.0 : 0.0;
        x = std::clamp(x, d.min, d.max);
        if (d.step > 0) {
            x = d.min + std::round((x - d.min) / d.step) * d.step;
            x = std::clamp(x, d.min, d.max);
        }
        return std::round(x * 1000.0) / 1000.0;
    }

    struct Values {
        std::array<double, kCount> v{};
        Values() { reset(); }
        void reset() {
            for (int i = 0; i < kCount; ++i) v[i] = kDefs[i].def;
        }
        bool on(F f) const { return v[idx(f)] >= 0.5; }
        double num(F f) const { return v[idx(f)]; }
        void set(F f, double x) { v[idx(f)] = clampStep(kDefs[idx(f)], x); }
        void setAt(int i, double x) {
            if (i >= 0 && i < kCount) v[i] = clampStep(kDefs[i], x);
        }
    };

    inline double stepped(FeatureDef const& d, double cur, int dir) {
        return clampStep(d, cur + (dir >= 0 ? d.step : -d.step));
    }

    // ------------------------------------------------------------ config file

    inline std::string serialize(Values const& vals) {
        std::ostringstream o;
        o.imbue(std::locale::classic());
        o.precision(6);
        for (int i = 0; i < kCount; ++i) o << kDefs[i].key << "=" << vals.v[i] << "\n";
        return o.str();
    }

    // Unknown keys and malformed lines are ignored; values are clamped.
    inline void parse(Values& vals, std::string const& text) {
        std::istringstream in(text);
        std::string line;
        while (std::getline(in, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
            const auto eq = line.find('=');
            if (eq == std::string::npos || eq == 0) continue;
            const std::string key = line.substr(0, eq);
            std::istringstream num(line.substr(eq + 1));
            num.imbue(std::locale::classic());
            double x = 0;
            num >> x;
            if (num.fail()) continue;
            for (int i = 0; i < kCount; ++i) {
                if (key == kDefs[i].key) {
                    vals.setAt(i, x);
                    break;
                }
            }
        }
    }

    // ------------------------------------------------------------- languages

    enum class Lang { En = 0, Es = 1, Ru = 2 };
    constexpr int kLangCount = 3;

    inline Lang langOf(Values const& v) {
        const int n = static_cast<int>(v.num(F::Language));
        return static_cast<Lang>(std::clamp(n, 0, kLangCount - 1));
    }

    // Each language written in itself.
    inline const char* langName(Lang l) {
        switch (l) {
            case Lang::En: return "English";
            case Lang::Es: return "Espa\u00f1ol";
            case Lang::Ru: return "\u0420\u0443\u0441\u0441\u043a\u0438\u0439";
        }
        return "English";
    }

    // Number of characters (not bytes) of a UTF-8 string.
    inline int utf8Length(std::string const& s) {
        int n = 0;
        for (unsigned char c : s) {
            if ((c & 0xC0) != 0x80) ++n;
        }
        return n;
    }

    enum class S : int {
        TabPlayer, TabVisual, TabPerf, TabHud, TabMisc, TabPresets, TabStats,
        Apply, Applied, ResetStats, StatsReset,
        SafeOn, SafeOff, SafeConfirm, SafeWarning, SafeDisabled, SafeEnabled,
        LanguageLabel, ResetBall, BallReset, TapForInfo,
        Attempts, Deaths, Furthest, Time, NoclipAcc, LastDeaths, NoneYet, HotZone, DeathsWord,
        HudAtt, HudDeaths, HudBest, HudAcc, CheatsOn,
        TabMacro,
        MacroIdle,
        MacroRecording,
        MacroPlaying,
        MacroRecord,
        MacroStopRec,
        MacroPlay,
        MacroStopPlay,
        MacroClear,
        MacroExport,
        MacroImport,
        MacroLoadFile,
        MacroNoFiles,
        MacroEvents,
        MacroClicks,
        MacroRestartHint,
        MacroNeedPractice,
        MacroExported,
        MacroImported,
        MacroImportFail,
        MacroEmpty,
        MacroCleared,
        MacroLoaded,
        MacroStopped,
        HudRec,
        HudPlay,
        HudBlocked,
        Count
    };
    constexpr int kStrCount = static_cast<int>(S::Count);

    // Order must match enum S (a test checks that nothing is empty and Russian really is Russian).
    inline const std::array<std::array<const char*, kLangCount>, kStrCount> kStr = {{
        /* TabPlayer   */ {{"Player", "Jugador", "\u0418\u0433\u0440\u043e\u043a"}},
        /* TabVisual   */ {{"Visual", "Visual", "\u0412\u0438\u0437\u0443\u0430\u043b"}},
        /* TabPerf     */ {{"Perf", "Rend.", "\u041f\u0440\u043e\u0438\u0437\u0432."}},
        /* TabHud      */ {{"HUD", "HUD", "HUD"}},
        /* TabMisc     */ {{"Misc", "Otros", "\u0420\u0430\u0437\u043d\u043e\u0435"}},
        /* TabPresets  */ {{"Presets", "Perfiles", "\u041f\u0440\u0435\u0441\u0435\u0442\u044b"}},
        /* TabStats    */ {{"Stats", "Datos", "\u0421\u0442\u0430\u0442\u044b"}},
        /* Apply       */ {{"Apply", "Aplicar", "\u041f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c"}},
        /* Applied     */ {{"Applied: ", "Aplicado: ", "\u041f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u043e: "}},
        /* ResetStats  */ {{"Reset stats", "Borrar datos", "\u0421\u0431\u0440\u043e\u0441\u0438\u0442\u044c"}},
        /* StatsReset  */ {{"Stats reset", "Datos borrados", "\u0421\u0442\u0430\u0442\u0438\u0441\u0442\u0438\u043a\u0430 \u0441\u0431\u0440\u043e\u0448\u0435\u043d\u0430"}},
        /* SafeOn      */ {{"Safe mode: ON", "Modo seguro: SI", "\u0411\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439: \u0412\u041a\u041b"}},
        /* SafeOff     */ {{"Safe mode: OFF", "Modo seguro: NO", "\u0411\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439: \u0412\u042b\u041a\u041b"}},
        /* SafeConfirm */ {{"Tap again to turn safe mode OFF", "Toca otra vez para apagar el modo seguro", "\u041d\u0430\u0436\u043c\u0438\u0442\u0435 \u0435\u0449\u0451 \u0440\u0430\u0437, \u0447\u0442\u043e\u0431\u044b \u0432\u044b\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0431\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c"}},
        /* SafeWarning */ {{"Cheats will work in normal runs too. You are responsible for how you use them.", "Las trampas funcionar\u00e1n tambi\u00e9n en partidas normales. T\u00fa eres responsable de c\u00f3mo las uses.", "\u0427\u0438\u0442\u044b \u0431\u0443\u0434\u0443\u0442 \u0440\u0430\u0431\u043e\u0442\u0430\u0442\u044c \u0438 \u0432 \u043e\u0431\u044b\u0447\u043d\u043e\u0439 \u0438\u0433\u0440\u0435. \u0412\u044b \u043d\u0435\u0441\u0451\u0442\u0435 \u043e\u0442\u0432\u0435\u0442\u0441\u0442\u0432\u0435\u043d\u043d\u043e\u0441\u0442\u044c \u0437\u0430 \u0438\u0445 \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u043d\u0438\u0435."}},
        /* SafeDisabled*/ {{"Safe mode is OFF", "Modo seguro DESACTIVADO", "\u0411\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c \u0432\u044b\u043a\u043b\u044e\u0447\u0435\u043d"}},
        /* SafeEnabled */ {{"Safe mode is ON", "Modo seguro ACTIVADO", "\u0411\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c \u0432\u043a\u043b\u044e\u0447\u0451\u043d"}},
        /* LanguageLabel*/ {{"Language", "Idioma", "\u042f\u0437\u044b\u043a"}},
        /* ResetBall   */ {{"Reset ball", "Reiniciar bola", "\u0421\u0431\u0440\u043e\u0441\u0438\u0442\u044c \u0448\u0430\u0440"}},
        /* BallReset   */ {{"Ball position reset", "Posici\u00f3n de la bola reiniciada", "\u041f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u0448\u0430\u0440\u0430 \u0441\u0431\u0440\u043e\u0448\u0435\u043d\u043e"}},
        /* TapForInfo  */ {{"Tap ? to read what a row does", "Toca ? para ver qu\u00e9 hace cada opci\u00f3n", "\u041d\u0430\u0436\u043c\u0438\u0442\u0435 ?, \u0447\u0442\u043e\u0431\u044b \u0443\u0437\u043d\u0430\u0442\u044c, \u0447\u0442\u043e \u0434\u0435\u043b\u0430\u0435\u0442 \u043e\u043f\u0446\u0438\u044f"}},
        /* Attempts    */ {{"Attempts", "Intentos", "\u041f\u043e\u043f\u044b\u0442\u043a\u0438"}},
        /* Deaths      */ {{"Deaths", "Muertes", "\u0421\u043c\u0435\u0440\u0442\u0438"}},
        /* Furthest    */ {{"Furthest death", "Muerte m\u00e1s lejana", "\u0421\u0430\u043c\u0430\u044f \u0434\u0430\u043b\u044c\u043d\u044f\u044f \u0441\u043c\u0435\u0440\u0442\u044c"}},
        /* Time        */ {{"Time", "Tiempo", "\u0412\u0440\u0435\u043c\u044f"}},
        /* NoclipAcc   */ {{"Noclip accuracy this attempt", "Precisi\u00f3n de noclip en este intento", "\u0422\u043e\u0447\u043d\u043e\u0441\u0442\u044c \u043d\u043e\u043a\u043b\u0438\u043f\u0430 \u0432 \u044d\u0442\u043e\u0439 \u043f\u043e\u043f\u044b\u0442\u043a\u0435"}},
        /* LastDeaths  */ {{"Last deaths", "\u00daltimas muertes", "\u041f\u043e\u0441\u043b\u0435\u0434\u043d\u0438\u0435 \u0441\u043c\u0435\u0440\u0442\u0438"}},
        /* NoneYet     */ {{"none yet", "ninguna a\u00fan", "\u043f\u043e\u043a\u0430 \u043d\u0435\u0442"}},
        /* HotZone     */ {{"Hot zone", "Zona caliente", "\u0422\u0440\u0443\u0434\u043d\u044b\u0439 \u0443\u0447\u0430\u0441\u0442\u043e\u043a"}},
        /* DeathsWord  */ {{"deaths", "muertes", "\u0441\u043c\u0435\u0440\u0442\u0435\u0439"}},
        /* HudAtt      */ {{"Att", "Int", "\u041f\u043e\u043f"}},
        /* HudDeaths   */ {{"Deaths", "Muertes", "\u0421\u043c\u0435\u0440\u0442\u0438"}},
        /* HudBest     */ {{"Best", "Mejor", "\u041b\u0443\u0447\u0448\u0430\u044f"}},
        /* HudAcc      */ {{"Acc", "Prec", "\u0422\u043e\u0447\u043d"}},
        /* CheatsOn    */ {{"CHEATS ON", "TRAMPAS ACTIVAS", "\u0427\u0418\u0422\u042b \u0412\u041a\u041b"}},
        /* TabMacro */ {{"Macro", "Macro", "\u041c\u0430\u043a\u0440\u043e"}},
        /* MacroIdle */ {{"Idle", "Inactivo", "\u041e\u0436\u0438\u0434\u0430\u043d\u0438\u0435"}},
        /* MacroRecording */ {{"Recording", "Grabando", "\u0417\u0430\u043f\u0438\u0441\u044c"}},
        /* MacroPlaying */ {{"Playing", "Reproduciendo", "\u0412\u043e\u0441\u043f\u0440\u043e\u0438\u0437\u0432\u0435\u0434\u0435\u043d\u0438\u0435"}},
        /* MacroRecord */ {{"Record", "Grabar", "\u0417\u0430\u043f\u0438\u0441\u044c"}},
        /* MacroStopRec */ {{"Stop rec", "Parar", "\u0421\u0442\u043e\u043f"}},
        /* MacroPlay */ {{"Play", "Reproducir", "\u0418\u0433\u0440\u0430\u0442\u044c"}},
        /* MacroStopPlay */ {{"Stop", "Detener", "\u0421\u0442\u043e\u043f"}},
        /* MacroClear */ {{"Clear", "Borrar", "\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c"}},
        /* MacroExport */ {{"Export", "Exportar", "\u042d\u043a\u0441\u043f\u043e\u0440\u0442"}},
        /* MacroImport */ {{"Import", "Importar", "\u0418\u043c\u043f\u043e\u0440\u0442"}},
        /* MacroLoadFile */ {{"Load file", "Cargar archivo", "\u0417\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044c \u0444\u0430\u0439\u043b"}},
        /* MacroNoFiles */ {{"(no saved macros)", "(sin macros guardadas)", "(\u043d\u0435\u0442 \u0441\u043e\u0445\u0440\u0430\u043d\u0451\u043d\u043d\u044b\u0445 \u043c\u0430\u043a\u0440\u043e\u0441\u043e\u0432)"}},
        /* MacroEvents */ {{"Events", "Eventos", "\u0421\u043e\u0431\u044b\u0442\u0438\u044f"}},
        /* MacroClicks */ {{"Clicks", "Clics", "\u041a\u043b\u0438\u043a\u0438"}},
        /* MacroRestartHint */ {{"Restart the level to begin", "Reinicia el nivel para empezar", "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u0443\u0440\u043e\u0432\u0435\u043d\u044c, \u0447\u0442\u043e\u0431\u044b \u043d\u0430\u0447\u0430\u0442\u044c"}},
        /* MacroNeedPractice */ {{"Safe mode is on: playback only works in Practice or Test mode", "Modo seguro activo: la reproducci\u00f3n solo funciona en modo Pr\u00e1ctica o Test", "\u0412\u043a\u043b\u044e\u0447\u0451\u043d \u0431\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c: \u0432\u043e\u0441\u043f\u0440\u043e\u0438\u0437\u0432\u0435\u0434\u0435\u043d\u0438\u0435 \u0442\u043e\u043b\u044c\u043a\u043e \u0432 \u0440\u0435\u0436\u0438\u043c\u0435 \u043f\u0440\u0430\u043a\u0442\u0438\u043a\u0438 \u0438\u043b\u0438 \u0442\u0435\u0441\u0442\u0430"}},
        /* MacroExported */ {{"Copied to the clipboard. File: ", "Copiado al portapapeles. Archivo: ", "\u0421\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u043e \u0432 \u0431\u0443\u0444\u0435\u0440 \u043e\u0431\u043c\u0435\u043d\u0430. \u0424\u0430\u0439\u043b: "}},
        /* MacroImported */ {{"Macro imported: ", "Macro importada: ", "\u041c\u0430\u043a\u0440\u043e\u0441 \u0438\u043c\u043f\u043e\u0440\u0442\u0438\u0440\u043e\u0432\u0430\u043d: "}},
        /* MacroImportFail */ {{"Could not import: ", "No se pudo importar: ", "\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u0438\u043c\u043f\u043e\u0440\u0442\u0438\u0440\u043e\u0432\u0430\u0442\u044c: "}},
        /* MacroEmpty */ {{"Nothing recorded yet", "A\u00fan no hay nada grabado", "\u041f\u043e\u043a\u0430 \u043d\u0438\u0447\u0435\u0433\u043e \u043d\u0435 \u0437\u0430\u043f\u0438\u0441\u0430\u043d\u043e"}},
        /* MacroCleared */ {{"Macro cleared", "Macro borrada", "\u041c\u0430\u043a\u0440\u043e\u0441 \u043e\u0447\u0438\u0449\u0435\u043d"}},
        /* MacroLoaded */ {{"Macro loaded: ", "Macro cargada: ", "\u041c\u0430\u043a\u0440\u043e\u0441 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d: "}},
        /* MacroStopped */ {{"Macro stopped", "Macro detenida", "\u041c\u0430\u043a\u0440\u043e\u0441 \u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d"}},
        /* HudRec */ {{"REC", "REC", "\u0417\u0410\u041f"}},
        /* HudPlay */ {{"PLAY", "PLAY", "\u0412\u041e\u0421\u041f\u0420"}},
        /* HudBlocked */ {{"blocked by safe mode", "bloqueado por modo seguro", "\u0437\u0430\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d\u043e \u0431\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u043c \u0440\u0435\u0436\u0438\u043c\u043e\u043c"}},
    }};

    inline const char* tr(Lang l, S s) {
        return kStr[static_cast<int>(s)][static_cast<int>(l)];
    }

    // Feature names/descriptions. English lives in kDefs; same order as enum F.
    inline const std::array<const char*, kCount> kNamesEs = {{
        "Noclip", "Precisi\u00f3n m\u00ednima de noclip %", "Salto infinito", "Velocidad de juego", "Reaparici\u00f3n propia", "Tiempo de reaparici\u00f3n (s)",
        "Sin brillo", "Sin shaders", "Sin part\u00edculas", "Sin efecto de muerte", "Ocultar jugador", "Sin triggers de c\u00e1mara", "Sin espejo",
        "Bajo detalle autom\u00e1tico", "Modo Eco (auto)", "FPS objetivo del Eco",
        "Mostrar FPS", "Mostrar estad\u00edsticas", "Mostrar precisi\u00f3n de noclip", "Aviso de trampas",
        "Desbloquear iconos", "Modo seguro", "Tama\u00f1o de la bola %", "Opacidad de la bola %", "Bola en el men\u00fa de pausa",
        "Extrapolaci\u00f3n de clics", "Ticks de extrapolaci\u00f3n",
        "Posici\u00f3n X de la bola", "Posici\u00f3n Y de la bola", "Idioma"}};
    inline const std::array<const char*, kCount> kDescsEs = {{
        "El jugador sobrevive a los obst\u00e1culos. Cuenta como trampa.",
        "Si tu precisi\u00f3n cae por debajo de este valor, puedes volver a morir. 0 = nunca mueres.",
        "Permite saltar en el aire. Cuenta como trampa.",
        "C\u00e1mara lenta para practicar, de 0.10 a 1.00. Menos de 1.00 cuenta como trampa.",
        "Cambia cu\u00e1nto esperas para reaparecer tras morir.",
        "Segundos de espera antes de reaparecer (requiere el ajuste anterior).",
        "Quita el brillo de los objetos colocados desde ahora (reinicia el nivel para aplicarlo).",
        "Apaga los efectos de shader en los niveles.",
        "Evita que se muestren las part\u00edculas.",
        "Omite la explosi\u00f3n al morir.",
        "Oculta el icono del jugador.",
        "Ignora los triggers de c\u00e1mara.",
        "Desactiva el efecto espejo del nivel.",
        "Activa el modo de bajo detalle al abrir la p\u00e1gina de un nivel.",
        "Si tus FPS bajan del objetivo, apaga shaders y part\u00edculas nuevas hasta que se recuperen.",
        "FPS por debajo de los cuales empieza el modo Eco.",
        "FPS y tiempo por frame en la esquina del nivel.",
        "Intentos, muertes y la muerte m\u00e1s lejana.",
        "Porcentaje de frames sin tocar un peligro.",
        "Muestra TRAMPAS ACTIVAS mientras una trampa est\u00e1 activa.",
        "Permite usar todos los iconos y colores en este dispositivo.",
        "Activado: noclip, salto infinito y velocidad solo funcionan en modo Pr\u00e1ctica o Test. Desactivado: funcionan siempre.",
        "Tama\u00f1o de la bola flotante del men\u00fa.",
        "Qu\u00e9 tan s\u00f3lida se ve la bola.",
        "Muestra la bola al pausar un nivel, para abrir el men\u00fa. Durante el juego la bola se oculta.",
        "Al grabar, extrapola tus clics hacia atr\u00e1s en el tiempo para compensar el retraso de entrada. La cantidad se ajusta en la siguiente opci\u00f3n.",
        "Cu\u00e1ntos ticks antes se guarda cada clic grabado (1 tick = 4.2 ms).",
        "Posici\u00f3n horizontal guardada de la bola.",
        "Posici\u00f3n vertical guardada de la bola.",
        "0 ingl\u00e9s, 1 espa\u00f1ol, 2 ruso."}};

    inline const std::array<const char*, kCount> kNamesRu = {{
        "\u041d\u043e\u043a\u043b\u0438\u043f", "\u041c\u0438\u043d. \u0442\u043e\u0447\u043d\u043e\u0441\u0442\u044c \u043d\u043e\u043a\u043b\u0438\u043f\u0430 %", "\u041f\u0440\u044b\u0436\u043a\u0438 \u0432 \u0432\u043e\u0437\u0434\u0443\u0445\u0435", "\u0421\u043a\u043e\u0440\u043e\u0441\u0442\u044c \u0438\u0433\u0440\u044b", "\u0421\u0432\u043e\u0451 \u0432\u0440\u0435\u043c\u044f \u0432\u043e\u0437\u0440\u043e\u0436\u0434\u0435\u043d\u0438\u044f", "\u0412\u0440\u0435\u043c\u044f \u0432\u043e\u0437\u0440\u043e\u0436\u0434\u0435\u043d\u0438\u044f (\u0441)",
        "\u0411\u0435\u0437 \u0441\u0432\u0435\u0447\u0435\u043d\u0438\u044f", "\u0411\u0435\u0437 \u0448\u0435\u0439\u0434\u0435\u0440\u043e\u0432", "\u0411\u0435\u0437 \u0447\u0430\u0441\u0442\u0438\u0446", "\u0411\u0435\u0437 \u044d\u0444\u0444\u0435\u043a\u0442\u0430 \u0441\u043c\u0435\u0440\u0442\u0438", "\u0421\u043a\u0440\u044b\u0442\u044c \u0438\u0433\u0440\u043e\u043a\u0430", "\u0411\u0435\u0437 \u0442\u0440\u0438\u0433\u0433\u0435\u0440\u043e\u0432 \u043a\u0430\u043c\u0435\u0440\u044b", "\u0411\u0435\u0437 \u0437\u0435\u0440\u043a\u0430\u043b\u0430",
        "\u0410\u0432\u0442\u043e \u043d\u0438\u0437\u043a\u0430\u044f \u0434\u0435\u0442\u0430\u043b\u0438\u0437\u0430\u0446\u0438\u044f", "\u042d\u043a\u043e-\u0440\u0435\u0436\u0438\u043c (\u0430\u0432\u0442\u043e)", "\u0426\u0435\u043b\u0435\u0432\u043e\u0439 FPS \u0434\u043b\u044f \u044d\u043a\u043e",
        "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c FPS", "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0441\u0442\u0430\u0442\u0438\u0441\u0442\u0438\u043a\u0443", "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0442\u043e\u0447\u043d\u043e\u0441\u0442\u044c \u043d\u043e\u043a\u043b\u0438\u043f\u0430", "\u0418\u043d\u0434\u0438\u043a\u0430\u0442\u043e\u0440 \u0447\u0438\u0442\u043e\u0432",
        "\u041e\u0442\u043a\u0440\u044b\u0442\u044c \u0432\u0441\u0435 \u0438\u043a\u043e\u043d\u043a\u0438", "\u0411\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c", "\u0420\u0430\u0437\u043c\u0435\u0440 \u0448\u0430\u0440\u0430 %", "\u041d\u0435\u043f\u0440\u043e\u0437\u0440\u0430\u0447\u043d\u043e\u0441\u0442\u044c \u0448\u0430\u0440\u0430 %", "\u0428\u0430\u0440 \u0432 \u043c\u0435\u043d\u044e \u043f\u0430\u0443\u0437\u044b",
        "\u042d\u043a\u0441\u0442\u0440\u0430\u043f\u043e\u043b\u044f\u0446\u0438\u044f \u043a\u043b\u0438\u043a\u043e\u0432", "\u0422\u0438\u043a\u0438 \u044d\u043a\u0441\u0442\u0440\u0430\u043f\u043e\u043b\u044f\u0446\u0438\u0438",
        "\u041f\u043e\u0437\u0438\u0446\u0438\u044f \u0448\u0430\u0440\u0430 X", "\u041f\u043e\u0437\u0438\u0446\u0438\u044f \u0448\u0430\u0440\u0430 Y", "\u042f\u0437\u044b\u043a"}};
    inline const std::array<const char*, kCount> kDescsRu = {{
        "\u0418\u0433\u0440\u043e\u043a \u043d\u0435 \u0443\u043c\u0438\u0440\u0430\u0435\u0442 \u043e\u0442 \u043f\u0440\u0435\u043f\u044f\u0442\u0441\u0442\u0432\u0438\u0439. \u0421\u0447\u0438\u0442\u0430\u0435\u0442\u0441\u044f \u0447\u0438\u0442\u043e\u043c.",
        "\u0415\u0441\u043b\u0438 \u0442\u043e\u0447\u043d\u043e\u0441\u0442\u044c \u0443\u043f\u0430\u0434\u0451\u0442 \u043d\u0438\u0436\u0435 \u044d\u0442\u043e\u0433\u043e \u0437\u043d\u0430\u0447\u0435\u043d\u0438\u044f, \u0432\u044b \u0441\u043d\u043e\u0432\u0430 \u0441\u043c\u043e\u0436\u0435\u0442\u0435 \u0443\u043c\u0435\u0440\u0435\u0442\u044c. 0 = \u043d\u0438\u043a\u043e\u0433\u0434\u0430 \u043d\u0435 \u0443\u043c\u0438\u0440\u0430\u0442\u044c.",
        "\u041f\u043e\u0437\u0432\u043e\u043b\u044f\u0435\u0442 \u043f\u0440\u044b\u0433\u0430\u0442\u044c \u0432 \u0432\u043e\u0437\u0434\u0443\u0445\u0435. \u0421\u0447\u0438\u0442\u0430\u0435\u0442\u0441\u044f \u0447\u0438\u0442\u043e\u043c.",
        "\u0417\u0430\u043c\u0435\u0434\u043b\u0435\u043d\u0438\u0435 \u0434\u043b\u044f \u0442\u0440\u0435\u043d\u0438\u0440\u043e\u0432\u043a\u0438, \u043e\u0442 0.10 \u0434\u043e 1.00. \u0417\u043d\u0430\u0447\u0435\u043d\u0438\u0435 \u043d\u0438\u0436\u0435 1.00 \u0441\u0447\u0438\u0442\u0430\u0435\u0442\u0441\u044f \u0447\u0438\u0442\u043e\u043c.",
        "\u041c\u0435\u043d\u044f\u0435\u0442 \u0432\u0440\u0435\u043c\u044f \u043e\u0436\u0438\u0434\u0430\u043d\u0438\u044f \u0432\u043e\u0437\u0440\u043e\u0436\u0434\u0435\u043d\u0438\u044f \u043f\u043e\u0441\u043b\u0435 \u0441\u043c\u0435\u0440\u0442\u0438.",
        "\u0421\u0435\u043a\u0443\u043d\u0434\u044b \u0434\u043e \u0432\u043e\u0437\u0440\u043e\u0436\u0434\u0435\u043d\u0438\u044f (\u043d\u0443\u0436\u043d\u0430 \u043f\u0440\u0435\u0434\u044b\u0434\u0443\u0449\u0430\u044f \u043e\u043f\u0446\u0438\u044f).",
        "\u0423\u0431\u0438\u0440\u0430\u0435\u0442 \u0441\u0432\u0435\u0447\u0435\u043d\u0438\u0435 \u0443 \u043e\u0431\u044a\u0435\u043a\u0442\u043e\u0432, \u0440\u0430\u0437\u043c\u0435\u0449\u0451\u043d\u043d\u044b\u0445 \u0441 \u044d\u0442\u043e\u0433\u043e \u043c\u043e\u043c\u0435\u043d\u0442\u0430 (\u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u0443\u0440\u043e\u0432\u0435\u043d\u044c).",
        "\u041e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0448\u0435\u0439\u0434\u0435\u0440\u043d\u044b\u0435 \u044d\u0444\u0444\u0435\u043a\u0442\u044b \u043d\u0430 \u0443\u0440\u043e\u0432\u043d\u044f\u0445.",
        "\u041d\u0435 \u043f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0435\u0442 \u044d\u0444\u0444\u0435\u043a\u0442\u044b \u0447\u0430\u0441\u0442\u0438\u0446.",
        "\u041f\u0440\u043e\u043f\u0443\u0441\u043a\u0430\u0435\u0442 \u0432\u0437\u0440\u044b\u0432 \u043f\u0440\u0438 \u0441\u043c\u0435\u0440\u0442\u0438.",
        "\u0421\u043a\u0440\u044b\u0432\u0430\u0435\u0442 \u0438\u043a\u043e\u043d\u043a\u0443 \u0438\u0433\u0440\u043e\u043a\u0430.",
        "\u0418\u0433\u043d\u043e\u0440\u0438\u0440\u0443\u0435\u0442 \u0442\u0440\u0438\u0433\u0433\u0435\u0440\u044b \u043a\u0430\u043c\u0435\u0440\u044b.",
        "\u041e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u043e\u0442\u0437\u0435\u0440\u043a\u0430\u043b\u0438\u0432\u0430\u043d\u0438\u0435 \u0443\u0440\u043e\u0432\u043d\u044f.",
        "\u0412\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0440\u0435\u0436\u0438\u043c \u043d\u0438\u0437\u043a\u043e\u0439 \u0434\u0435\u0442\u0430\u043b\u0438\u0437\u0430\u0446\u0438\u0438 \u043f\u0440\u0438 \u043e\u0442\u043a\u0440\u044b\u0442\u0438\u0438 \u0441\u0442\u0440\u0430\u043d\u0438\u0446\u044b \u0443\u0440\u043e\u0432\u043d\u044f.",
        "\u0415\u0441\u043b\u0438 FPS \u043d\u0438\u0436\u0435 \u0446\u0435\u043b\u0438, \u043e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0448\u0435\u0439\u0434\u0435\u0440\u044b \u0438 \u043d\u043e\u0432\u044b\u0435 \u0447\u0430\u0441\u0442\u0438\u0446\u044b, \u043f\u043e\u043a\u0430 FPS \u043d\u0435 \u0432\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u0441\u044f.",
        "FPS, \u043d\u0438\u0436\u0435 \u043a\u043e\u0442\u043e\u0440\u043e\u0433\u043e \u0432\u043a\u043b\u044e\u0447\u0430\u0435\u0442\u0441\u044f \u044d\u043a\u043e-\u0440\u0435\u0436\u0438\u043c.",
        "FPS \u0438 \u0432\u0440\u0435\u043c\u044f \u043a\u0430\u0434\u0440\u0430 \u0432 \u0443\u0433\u043b\u0443 \u0443\u0440\u043e\u0432\u043d\u044f.",
        "\u041f\u043e\u043f\u044b\u0442\u043a\u0438, \u0441\u043c\u0435\u0440\u0442\u0438 \u0438 \u0441\u0430\u043c\u0430\u044f \u0434\u0430\u043b\u044c\u043d\u044f\u044f \u0441\u043c\u0435\u0440\u0442\u044c.",
        "\u0414\u043e\u043b\u044f \u043a\u0430\u0434\u0440\u043e\u0432 \u0431\u0435\u0437 \u043a\u0430\u0441\u0430\u043d\u0438\u044f \u043e\u043f\u0430\u0441\u043d\u044b\u0445 \u043e\u0431\u044a\u0435\u043a\u0442\u043e\u0432.",
        "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0435\u0442 \u0427\u0418\u0422\u042b \u0412\u041a\u041b, \u043f\u043e\u043a\u0430 \u0432\u043a\u043b\u044e\u0447\u0451\u043d \u0447\u0438\u0442.",
        "\u041f\u043e\u0437\u0432\u043e\u043b\u044f\u0435\u0442 \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u0432\u0441\u0435 \u0438\u043a\u043e\u043d\u043a\u0438 \u0438 \u0446\u0432\u0435\u0442\u0430 \u043d\u0430 \u044d\u0442\u043e\u043c \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0435.",
        "\u0412\u043a\u043b: \u043d\u043e\u043a\u043b\u0438\u043f, \u043f\u0440\u044b\u0436\u043a\u0438 \u0432 \u0432\u043e\u0437\u0434\u0443\u0445\u0435 \u0438 \u0441\u043a\u043e\u0440\u043e\u0441\u0442\u044c \u0440\u0430\u0431\u043e\u0442\u0430\u044e\u0442 \u0442\u043e\u043b\u044c\u043a\u043e \u0432 \u0440\u0435\u0436\u0438\u043c\u0435 \u043f\u0440\u0430\u043a\u0442\u0438\u043a\u0438 \u0438\u043b\u0438 \u0442\u0435\u0441\u0442\u0430. \u0412\u044b\u043a\u043b: \u0440\u0430\u0431\u043e\u0442\u0430\u044e\u0442 \u0432\u0435\u0437\u0434\u0435.",
        "\u0420\u0430\u0437\u043c\u0435\u0440 \u043f\u043b\u0430\u0432\u0430\u044e\u0449\u0435\u0433\u043e \u0448\u0430\u0440\u0430 \u043c\u0435\u043d\u044e.",
        "\u041d\u0430\u0441\u043a\u043e\u043b\u044c\u043a\u043e \u043f\u043b\u043e\u0442\u043d\u043e \u0432\u0438\u0434\u0435\u043d \u0448\u0430\u0440.",
        "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0435\u0442 \u0448\u0430\u0440 \u043f\u0440\u0438 \u043f\u0430\u0443\u0437\u0435 \u0443\u0440\u043e\u0432\u043d\u044f, \u0447\u0442\u043e\u0431\u044b \u043e\u0442\u043a\u0440\u044b\u0442\u044c \u043c\u0435\u043d\u044e. \u0412\u043e \u0432\u0440\u0435\u043c\u044f \u0438\u0433\u0440\u044b \u0448\u0430\u0440 \u0441\u043a\u0440\u044b\u0442.",
        "\u041f\u0440\u0438 \u0437\u0430\u043f\u0438\u0441\u0438 \u043a\u043b\u0438\u043a\u0438 \u0441\u0434\u0432\u0438\u0433\u0430\u044e\u0442\u0441\u044f \u043d\u0430\u0437\u0430\u0434 \u0432\u043e \u0432\u0440\u0435\u043c\u0435\u043d\u0438, \u0447\u0442\u043e\u0431\u044b \u043a\u043e\u043c\u043f\u0435\u043d\u0441\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0437\u0430\u0434\u0435\u0440\u0436\u043a\u0443 \u0432\u0432\u043e\u0434\u0430. \u0412\u0435\u043b\u0438\u0447\u0438\u043d\u0430 \u0437\u0430\u0434\u0430\u0451\u0442\u0441\u044f \u0432 \u0441\u043b\u0435\u0434\u0443\u044e\u0449\u0435\u0439 \u043e\u043f\u0446\u0438\u0438.",
        "\u041d\u0430 \u0441\u043a\u043e\u043b\u044c\u043a\u043e \u0442\u0438\u043a\u043e\u0432 \u0440\u0430\u043d\u044c\u0448\u0435 \u0441\u043e\u0445\u0440\u0430\u043d\u044f\u0435\u0442\u0441\u044f \u043a\u0430\u0436\u0434\u044b\u0439 \u0437\u0430\u043f\u0438\u0441\u0430\u043d\u043d\u044b\u0439 \u043a\u043b\u0438\u043a (1 \u0442\u0438\u043a = 4.2 \u043c\u0441).",
        "\u0421\u043e\u0445\u0440\u0430\u043d\u0451\u043d\u043d\u0430\u044f \u0433\u043e\u0440\u0438\u0437\u043e\u043d\u0442\u0430\u043b\u044c\u043d\u0430\u044f \u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0448\u0430\u0440\u0430.",
        "\u0421\u043e\u0445\u0440\u0430\u043d\u0451\u043d\u043d\u0430\u044f \u0432\u0435\u0440\u0442\u0438\u043a\u0430\u043b\u044c\u043d\u0430\u044f \u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0448\u0430\u0440\u0430.",
        "0 \u0430\u043d\u0433\u043b\u0438\u0439\u0441\u043a\u0438\u0439, 1 \u0438\u0441\u043f\u0430\u043d\u0441\u043a\u0438\u0439, 2 \u0440\u0443\u0441\u0441\u043a\u0438\u0439."}};

    inline const char* featureName(Lang l, int i) {
        if (i < 0 || i >= kCount) return "";
        if (l == Lang::Es) return kNamesEs[i];
        if (l == Lang::Ru) return kNamesRu[i];
        return kDefs[i].name;
    }
    inline const char* featureDesc(Lang l, int i) {
        if (i < 0 || i >= kCount) return "";
        if (l == Lang::Es) return kDescsEs[i];
        if (l == Lang::Ru) return kDescsRu[i];
        return kDefs[i].desc;
    }

    // ----------------------------------------------------------------- presets

    enum class Preset { Panic = 0, Practice = 1, Performance = 2, MinimalVisuals = 3 };
    constexpr int kPresetCount = 4;

    inline const char* presetName(Lang l, Preset p) {
        static const char* names[kPresetCount][kLangCount] = {
            {"Panic: everything off", "P\u00e1nico: todo apagado", "\u041f\u0430\u043d\u0438\u043a\u0430: \u0432\u0441\u0451 \u0432\u044b\u043a\u043b\u044e\u0447\u0435\u043d\u043e"},
            {"Practice", "Pr\u00e1ctica", "\u041f\u0440\u0430\u043a\u0442\u0438\u043a\u0430"},
            {"Performance", "Rendimiento", "\u041f\u0440\u043e\u0438\u0437\u0432\u043e\u0434\u0438\u0442\u0435\u043b\u044c\u043d\u043e\u0441\u0442\u044c"},
            {"Minimal visuals", "Visuales m\u00ednimos", "\u041c\u0438\u043d\u0438\u043c\u0443\u043c \u044d\u0444\u0444\u0435\u043a\u0442\u043e\u0432"}};
        const int i = static_cast<int>(p);
        if (i < 0 || i >= kPresetCount) return "";
        return names[i][static_cast<int>(l)];
    }

    inline const char* presetDesc(Lang l, Preset p) {
        static const char* descs[kPresetCount][kLangCount] = {
            {"Turns every feature off", "Apaga todas las funciones", "\u0412\u044b\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0432\u0441\u0435 \u0444\u0443\u043d\u043a\u0446\u0438\u0438"},
            {"Custom respawn 0.5 s, stats and accuracy HUD", "Reaparici\u00f3n 0.5 s, estad\u00edsticas y precisi\u00f3n en pantalla", "\u0412\u043e\u0437\u0440\u043e\u0436\u0434\u0435\u043d\u0438\u0435 0.5 \u0441, \u0441\u0442\u0430\u0442\u0438\u0441\u0442\u0438\u043a\u0430 \u0438 \u0442\u043e\u0447\u043d\u043e\u0441\u0442\u044c \u043d\u0430 \u044d\u043a\u0440\u0430\u043d\u0435"},
            {"Auto LDM, no glow, no shaders, no particles, eco, FPS", "LDM auto, sin brillo, shaders ni part\u00edculas, eco, FPS", "\u0410\u0432\u0442\u043e LDM, \u0431\u0435\u0437 \u0441\u0432\u0435\u0447\u0435\u043d\u0438\u044f, \u0448\u0435\u0439\u0434\u0435\u0440\u043e\u0432 \u0438 \u0447\u0430\u0441\u0442\u0438\u0446, \u044d\u043a\u043e, FPS"},
            {"No death effect, no particles, no glow", "Sin efecto de muerte, part\u00edculas ni brillo", "\u0411\u0435\u0437 \u044d\u0444\u0444\u0435\u043a\u0442\u0430 \u0441\u043c\u0435\u0440\u0442\u0438, \u0447\u0430\u0441\u0442\u0438\u0446 \u0438 \u0441\u0432\u0435\u0447\u0435\u043d\u0438\u044f"}};
        const int i = static_cast<int>(p);
        if (i < 0 || i >= kPresetCount) return "";
        return descs[i][static_cast<int>(l)];
    }

    // Every preset starts from "all off" but keeps the menu ball and the practice-only safety.
    inline void applyPreset(Values& vals, Preset p) {
        const Values keep = vals;
        vals.reset();
        for (F f : {F::BallSize, F::BallOpacity, F::BallInPause, F::BallX, F::BallY, F::Language, F::PracticeOnly}) {
            vals.set(f, keep.num(f));
        }
        switch (p) {
            case Preset::Panic: break;
            case Preset::Practice:
                vals.set(F::Respawn, 1);
                vals.set(F::RespawnTime, 0.5);
                vals.set(F::HudStats, 1);
                vals.set(F::HudAcc, 1);
                break;
            case Preset::Performance:
                vals.set(F::AutoLdm, 1);
                vals.set(F::NoGlow, 1);
                vals.set(F::NoShaders, 1);
                vals.set(F::NoParticles, 1);
                vals.set(F::Eco, 1);
                vals.set(F::HudFps, 1);
                break;
            case Preset::MinimalVisuals:
                vals.set(F::NoDeathFx, 1);
                vals.set(F::NoParticles, 1);
                vals.set(F::NoGlow, 1);
                break;
        }
    }

    // ------------------------------------------------------------------ cheats

    inline bool cheatsAllowed(bool practiceOnly, bool inPractice, bool inTest) {
        return !practiceOnly || inPractice || inTest;
    }

    inline bool anyCheatEnabled(Values const& v) {
        return v.on(F::Noclip) || v.on(F::JumpHack) || v.num(F::Speed) < 0.999;
    }

    inline double speedFactor(Values const& v, bool allowed) {
        return allowed ? std::clamp(v.num(F::Speed), 0.1, 1.0) : 1.0;
    }

    inline double accuracyPercent(int frames, int deathFrames) {
        if (frames <= 0) return 100.0;
        const int d = std::clamp(deathFrames, 0, frames);
        return 100.0 * (1.0 - static_cast<double>(d) / static_cast<double>(frames));
    }

    // minAccPct <= 0 means "never die". Under 30 frames there is too little data to judge.
    inline bool noclipBlocksDeath(double minAccPct, int frames, int deathFramesSoFar) {
        if (minAccPct <= 0.0) return true;
        if (frames < 30) return true;
        return accuracyPercent(frames, deathFramesSoFar + 1) >= minAccPct;
    }

    struct Attempt {
        int frames = 0;
        int deathFrames = 0;
        void reset() { frames = deathFrames = 0; }
    };

    // ------------------------------------------------------------------- stats

    struct SessionStats {
        int attempts = 0;
        int deaths = 0;
        double furthest = 0.0;       // furthest death, in percent
        double seconds = 0.0;
        std::deque<double> last;     // last deaths, newest at the back
        std::array<int, 20> zones{}; // 5% buckets

        void reset() { *this = SessionStats{}; }
        void onAttempt() { ++attempts; }
        void addTime(double dt) {
            if (dt > 0) seconds += dt;
        }
        void onDeath(double pct) {
            pct = std::clamp(std::isfinite(pct) ? pct : 0.0, 0.0, 100.0);
            ++deaths;
            furthest = std::max(furthest, pct);
            last.push_back(pct);
            while (last.size() > 10) last.pop_front();
            ++zones[std::min(19, static_cast<int>(pct / 5.0))];
        }
        // Returns the bucket with most deaths (first one on ties) or -1 when there are none.
        int hotZone(int& count) const {
            int best = -1;
            count = 0;
            for (int i = 0; i < 20; ++i) {
                if (zones[i] > count) {
                    count = zones[i];
                    best = i;
                }
            }
            return best;
        }
    };

    inline std::string formatTime(double sec) {
        const long long s = static_cast<long long>(std::max(0.0, sec));
        char buf[24];
        std::snprintf(buf, sizeof(buf), "%lld:%02lld", s / 60, s % 60);
        return buf;
    }

    inline std::string fmt1(double x) {
        char buf[24];
        std::snprintf(buf, sizeof(buf), "%.1f", x);
        return buf;
    }

    inline std::vector<std::string> statsLines(SessionStats const& s, double accPct, Lang l = Lang::En) {
        std::vector<std::string> out;
        out.push_back(std::string(tr(l, S::Attempts)) + ": " + std::to_string(s.attempts) + "   " + tr(l, S::Deaths) +
                      ": " + std::to_string(s.deaths));
        out.push_back(std::string(tr(l, S::Furthest)) + ": " + fmt1(s.furthest) + "%   " + tr(l, S::Time) + ": " +
                      formatTime(s.seconds));
        out.push_back(std::string(tr(l, S::NoclipAcc)) + ": " + fmt1(accPct) + "%");
        std::string last = std::string(tr(l, S::LastDeaths)) + ": ";
        if (s.last.empty()) {
            last += tr(l, S::NoneYet);
        } else {
            const size_t from = s.last.size() > 6 ? s.last.size() - 6 : 0;
            for (size_t i = from; i < s.last.size(); ++i) {
                if (i > from) last += ", ";
                last += fmt1(s.last[i]) + "%";
            }
        }
        out.push_back(last);
        int count = 0;
        const int zone = s.hotZone(count);
        if (zone < 0) {
            out.push_back(std::string(tr(l, S::HotZone)) + ": " + tr(l, S::NoneYet));
        } else {
            out.push_back(std::string(tr(l, S::HotZone)) + ": " + std::to_string(zone * 5) + "-" +
                          std::to_string(zone * 5 + 5) + "% (" + std::to_string(count) + " " + tr(l, S::DeathsWord) + ")");
        }
        return out;
    }

    // -------------------------------------------------------------------- macro

    constexpr int kMacroTps = 240;                   // physics ticks per second
    constexpr size_t kMacroMaxEvents = 200000;
    constexpr uint32_t kMacroMaxTick = 30u * 60u * 60u * kMacroTps;   // 30 minutes

    struct MacroEvent {
        uint32_t tick = 0;
        int button = 1;          // 1 jump, 2 left, 3 right
        bool player2 = false;
        bool down = false;
    };

    struct Macro {
        std::vector<MacroEvent> events;

        void clear() { events.clear(); }
        int clicks() const {
            int n = 0;
            for (auto const& e : events) {
                if (e.down) ++n;
            }
            return n;
        }
        double seconds() const {
            return events.empty() ? 0.0 : static_cast<double>(events.back().tick) / kMacroTps;
        }
    };

    enum class MacroMode { Idle, Record, Play };

    // Game time -> physics tick (clamped and safe for NaN).
    inline uint32_t tickFromTime(double levelTime) {
        if (!(levelTime > 0.0)) return 0;
        const double t = std::round(levelTime * kMacroTps);
        return t >= static_cast<double>(kMacroMaxTick) ? kMacroMaxTick : static_cast<uint32_t>(t);
    }

    struct MacroSession {
        MacroMode mode = MacroMode::Idle;
        Macro macro;
        size_t playIndex = 0;

        // A new attempt begins: recording starts from scratch, playback from the first event.
        void startAttempt() {
            if (mode == MacroMode::Record) macro.clear();
            playIndex = 0;
        }

        void stop() { mode = MacroMode::Idle; }

        // "Click extrapolation": the click is saved `shiftTicks` earlier than it was detected.
        // Order is kept (a click never lands before the previous one).
        bool record(uint32_t tickNow, int button, bool player2, bool down, int shiftTicks) {
            if (button < 1 || button > 3) return false;
            if (macro.events.size() >= kMacroMaxEvents) return false;
            const uint32_t shift = static_cast<uint32_t>(std::max(0, shiftTicks));
            uint32_t tick = tickNow >= shift ? tickNow - shift : 0;
            if (!macro.events.empty()) tick = std::max(tick, macro.events.back().tick);
            macro.events.push_back({tick, button, player2, down});
            return true;
        }

        // Events that should be applied at or before `tickNow` (each one is returned once).
        std::vector<MacroEvent> due(uint32_t tickNow) {
            std::vector<MacroEvent> out;
            while (playIndex < macro.events.size() && macro.events[playIndex].tick <= tickNow) {
                out.push_back(macro.events[playIndex]);
                ++playIndex;
            }
            return out;
        }
    };

    inline std::string lowerAscii(std::string s) {
        for (char& c : s) {
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
        }
        return s;
    }

    // Text format, one event per line:  tick,button,player,down   (player 1 or 2, down 1 or 0)
    inline std::string macroToText(Macro const& m) {
        std::string out = "MMACRO 1\ntps " + std::to_string(kMacroTps) + "\n";
        for (auto const& e : m.events) {
            out += std::to_string(e.tick) + "," + std::to_string(e.button) + "," + (e.player2 ? "2" : "1") + "," +
                   (e.down ? "1" : "0") + "\n";
        }
        return out;
    }

    // Reads the text format. A macro made at another tick rate ("tps N") is converted to 240.
    inline bool macroFromText(std::string const& text, Macro& out, std::string& error) {
        std::istringstream in(text);
        std::string line;
        bool header = false;
        int tps = kMacroTps;
        Macro result;
        int bad = 0;
        auto toNum = [](std::string s, long long& v) {
            while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
            while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.pop_back();
            if (s.empty() || s.size() > 12) return false;
            for (char c : s) {
                if (c < '0' || c > '9') return false;
            }
            v = std::stoll(s);
            return true;
        };
        while (std::getline(in, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
            if (line.empty() || line[0] == '#') continue;
            if (!header) {
                if (lowerAscii(line).rfind("mmacro", 0) != 0) {
                    error = "not a MegaMobile macro";
                    return false;
                }
                header = true;
                continue;
            }
            if (lowerAscii(line).rfind("tps", 0) == 0) {
                long long v = 0;
                if (toNum(line.substr(3), v) && v >= 30 && v <= 1000) tps = static_cast<int>(v);
                else ++bad;
                continue;
            }
            std::vector<std::string> f;
            std::string cur;
            for (char c : line) {
                if (c == ',') { f.push_back(cur); cur.clear(); }
                else cur.push_back(c);
            }
            f.push_back(cur);
            long long tick = 0, button = 0, player = 0, down = 0;
            if (f.size() != 4 || !toNum(f[0], tick) || !toNum(f[1], button) || !toNum(f[2], player) ||
                !toNum(f[3], down) || button < 1 || button > 3 || player < 1 || player > 2 || down > 1 ||
                tick > static_cast<long long>(kMacroMaxTick) * 4) {
                ++bad;
                continue;
            }
            if (result.events.size() >= kMacroMaxEvents) {
                error = "too many events";
                return false;
            }
            MacroEvent e;
            e.tick = static_cast<uint32_t>(std::min<long long>(
                kMacroMaxTick, std::llround(static_cast<double>(tick) * kMacroTps / static_cast<double>(tps))));
            e.button = static_cast<int>(button);
            e.player2 = player == 2;
            e.down = down == 1;
            result.events.push_back(e);
        }
        if (!header) {
            error = "empty text";
            return false;
        }
        if (result.events.empty()) {
            error = bad > 0 ? "no valid events" : "no events";
            return false;
        }
        std::stable_sort(result.events.begin(), result.events.end(),
                         [](MacroEvent const& a, MacroEvent const& b) { return a.tick < b.tick; });
        out = std::move(result);
        return true;
    }

    // "REC 12" while recording, "PLAY 3/40" while playing, "PLAY blocked by safe mode" when it cannot run.
    inline std::string macroHudText(MacroSession const& s, bool playBlocked, Lang l) {
        if (s.mode == MacroMode::Record) return std::string(tr(l, S::HudRec)) + " " + std::to_string(s.macro.events.size());
        if (s.mode == MacroMode::Play) {
            if (playBlocked) return std::string(tr(l, S::HudPlay)) + ": " + tr(l, S::HudBlocked);
            return std::string(tr(l, S::HudPlay)) + " " + std::to_string(s.playIndex) + "/" + std::to_string(s.macro.events.size());
        }
        return "";
    }

    // ---------------------------------------------------------------- FPS / eco

    struct FpsSample {
        float fps = 0.f;
        float ms = 0.f;
    };

    struct FpsMeter {
        float acc = 0.f;
        int frames = 0;
        bool push(float dt, float windowSec, FpsSample& out) {
            if (!(dt > 0.f)) return false;
            acc += dt;
            ++frames;
            if (acc < windowSec) return false;
            out.fps = static_cast<float>(frames) / acc;
            out.ms = 1000.f * acc / static_cast<float>(frames);
            acc = 0.f;
            frames = 0;
            return true;
        }
        void reset() { acc = 0.f; frames = 0; }
    };

    enum class Pressure { None = 0, Low = 1 };

    // Low under the target FPS; needs 5 FPS above the target to calm down (no flicker).
    inline Pressure updatePressure(Pressure prev, float fps, int target) {
        const float t = static_cast<float>(target);
        if (fps < t) return Pressure::Low;
        if (prev == Pressure::Low && fps < t + 5.f) return Pressure::Low;
        return Pressure::None;
    }

    // ---------------------------------------------------------------------- HUD

    struct HudInput {
        bool fps = false, stats = false, acc = false, cheats = false;
        FpsSample sample;
        SessionStats const* session = nullptr;
        double accuracy = 100.0;
        bool cheatsOn = false;
        Lang lang = Lang::En;
        std::string macro;   // "REC 12" / "PLAY 3/40" (empty when idle)
    };

    inline std::string hudText(HudInput const& in) {
        std::vector<std::string> lines;
        if (!in.macro.empty()) lines.push_back(in.macro);
        if (in.fps) {
            char buf[40];
            std::snprintf(buf, sizeof(buf), "%.0f FPS  %.1f ms", in.sample.fps, in.sample.ms);
            lines.push_back(buf);
        }
        if (in.stats && in.session) {
            lines.push_back(std::string(tr(in.lang, S::HudAtt)) + " " + std::to_string(in.session->attempts) + "  " +
                            tr(in.lang, S::HudDeaths) + " " + std::to_string(in.session->deaths) + "  " +
                            tr(in.lang, S::HudBest) + " " + fmt1(in.session->furthest) + "%");
        }
        if (in.acc) lines.push_back(std::string(tr(in.lang, S::HudAcc)) + " " + fmt1(in.accuracy) + "%");
        if (in.cheats && in.cheatsOn) lines.push_back(tr(in.lang, S::CheatsOn));
        std::string out;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i) out += "\n";
            out += lines[i];
        }
        return out;
    }

} // namespace mm
