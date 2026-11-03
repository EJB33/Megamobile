// MegaMobile by EJB3 - pure C++ logic (no Geode types), unit tested on any PC.
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
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
        UnlockIcons, PracticeOnly, BallCorner, BallSize, BallOpacity,
        Count
    };
    constexpr int kCount = static_cast<int>(F::Count);
    constexpr int idx(F f) { return static_cast<int>(f); }

    enum class Cat { Player, Visual, Perf, Hud, Misc };
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
        {"practice_only", "Cheats only in practice", "Noclip, jump hack and game speed only work in Practice or Test mode.", Cat::Misc, Kind::Toggle, 1, 0, 1, 1},
        {"ball_corner", "Ball corner", "0 top-left, 1 top-right, 2 bottom-left, 3 bottom-right.", Cat::Misc, Kind::Number, 2, 0, 3, 1},
        {"ball_size", "Ball size %", "Size of the floating menu ball.", Cat::Misc, Kind::Number, 60, 30, 120, 10},
        {"ball_opacity", "Ball opacity %", "How solid the floating menu ball is.", Cat::Misc, Kind::Number, 80, 20, 100, 10},
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

    // ----------------------------------------------------------------- presets

    enum class Preset { Panic = 0, Practice = 1, Performance = 2, MinimalVisuals = 3 };
    constexpr int kPresetCount = 4;

    inline const char* presetName(Preset p) {
        switch (p) {
            case Preset::Panic: return "Panic: everything off";
            case Preset::Practice: return "Practice";
            case Preset::Performance: return "Performance";
            case Preset::MinimalVisuals: return "Minimal visuals";
        }
        return "";
    }

    inline const char* presetDesc(Preset p) {
        switch (p) {
            case Preset::Panic: return "Turns every feature off";
            case Preset::Practice: return "Custom respawn 0.5 s, stats and accuracy HUD";
            case Preset::Performance: return "Auto LDM, no glow, no shaders, no particles, eco, FPS";
            case Preset::MinimalVisuals: return "No death effect, no particles, no glow";
        }
        return "";
    }

    // Every preset starts from "all off" but keeps the menu ball and the practice-only safety.
    inline void applyPreset(Values& vals, Preset p) {
        const double corner = vals.num(F::BallCorner), size = vals.num(F::BallSize), opacity = vals.num(F::BallOpacity);
        const double practiceOnly = vals.num(F::PracticeOnly);
        vals.reset();
        vals.set(F::BallCorner, corner);
        vals.set(F::BallSize, size);
        vals.set(F::BallOpacity, opacity);
        vals.set(F::PracticeOnly, practiceOnly);
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

    inline std::vector<std::string> statsLines(SessionStats const& s, double accPct) {
        std::vector<std::string> out;
        out.push_back("Attempts: " + std::to_string(s.attempts) + "   Deaths: " + std::to_string(s.deaths));
        out.push_back("Furthest death: " + fmt1(s.furthest) + "%   Time: " + formatTime(s.seconds));
        out.push_back("Noclip accuracy this attempt: " + fmt1(accPct) + "%");
        std::string last = "Last deaths: ";
        if (s.last.empty()) {
            last += "none yet";
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
            out.push_back("Hot zone: none yet");
        } else {
            out.push_back("Hot zone: " + std::to_string(zone * 5) + "-" + std::to_string(zone * 5 + 5) + "% (" +
                          std::to_string(count) + " deaths)");
        }
        return out;
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
    };

    inline std::string hudText(HudInput const& in) {
        std::vector<std::string> lines;
        if (in.fps) {
            char buf[40];
            std::snprintf(buf, sizeof(buf), "%.0f FPS  %.1f ms", in.sample.fps, in.sample.ms);
            lines.push_back(buf);
        }
        if (in.stats && in.session) {
            lines.push_back("Att " + std::to_string(in.session->attempts) + "  Deaths " +
                            std::to_string(in.session->deaths) + "  Best " + fmt1(in.session->furthest) + "%");
        }
        if (in.acc) lines.push_back("Acc " + fmt1(in.accuracy) + "%");
        if (in.cheats && in.cheatsOn) lines.push_back("CHEATS ON");
        std::string out;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i) out += "\n";
            out += lines[i];
        }
        return out;
    }

    struct CornerPos {
        float x = 0.f, y = 0.f;
    };

    // 0 top-left, 1 top-right, 2 bottom-left, 3 bottom-right
    inline CornerPos cornerPos(int corner, float winW, float winH, float margin) {
        const bool right = (corner == 1 || corner == 3);
        const bool bottom = (corner == 2 || corner == 3);
        return {right ? winW - margin : margin, bottom ? margin : winH - margin};
    }

} // namespace mm
