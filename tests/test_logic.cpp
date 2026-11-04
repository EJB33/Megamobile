#include "../src/logic.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
using namespace mm;
int main() {
    // ---- registry integrity
    static_assert(kCount == 28, "feature count changed: update the menu/docs");
    std::set<std::string> keys;
    for (int i = 0; i < kCount; ++i) {
        auto const& d = kDefs[i];
        assert(d.key && d.name && d.desc);
        assert(keys.insert(d.key).second);                         // unique keys
        assert(d.def >= d.min && d.def <= d.max);                  // default inside range
        assert(d.kind == Kind::Number || (d.min == 0 && d.max == 1));
        assert(clampStep(d, d.def) == d.def);                      // default is a valid stepped value
    }
    // enum order == table order (spot checks on both ends and every category start)
    assert(std::string(kDefs[idx(F::Noclip)].key) == "noclip");
    assert(std::string(kDefs[idx(F::Speed)].key) == "speed");
    assert(std::string(kDefs[idx(F::RespawnTime)].key) == "respawn_time");
    assert(std::string(kDefs[idx(F::NoGlow)].key) == "no_glow");
    assert(std::string(kDefs[idx(F::NoMirror)].key) == "no_mirror");
    assert(std::string(kDefs[idx(F::AutoLdm)].key) == "auto_ldm");
    assert(std::string(kDefs[idx(F::EcoTarget)].key) == "eco_target");
    assert(std::string(kDefs[idx(F::HudFps)].key) == "hud_fps");
    assert(std::string(kDefs[idx(F::HudCheats)].key) == "hud_cheats");
    assert(std::string(kDefs[idx(F::UnlockIcons)].key) == "unlock_icons");
    assert(std::string(kDefs[idx(F::BallOpacity)].key) == "ball_opacity");
    assert(std::string(kDefs[idx(F::BallInPause)].key) == "ball_in_pause");
    assert(std::string(kDefs[idx(F::BallX)].key) == "ball_x" && std::string(kDefs[idx(F::Language)].key) == "language");
    assert(kDefs[idx(F::Noclip)].cat == Cat::Player && kDefs[idx(F::RespawnTime)].cat == Cat::Player);
    assert(kDefs[idx(F::NoGlow)].cat == Cat::Visual && kDefs[idx(F::NoMirror)].cat == Cat::Visual);
    assert(kDefs[idx(F::AutoLdm)].cat == Cat::Perf && kDefs[idx(F::EcoTarget)].cat == Cat::Perf);
    assert(kDefs[idx(F::HudFps)].cat == Cat::Hud && kDefs[idx(F::HudCheats)].cat == Cat::Hud);
    assert(kDefs[idx(F::UnlockIcons)].cat == Cat::Misc && kDefs[idx(F::BallInPause)].cat == Cat::Misc);
    assert(kDefs[idx(F::BallX)].cat == Cat::Hidden && kDefs[idx(F::Language)].cat == Cat::Hidden);
    int perCat[6] = {0,0,0,0,0,0};
    for (int i = 0; i < kCount; ++i) perCat[static_cast<int>(kDefs[i].cat)]++;
    assert(perCat[0]==6 && perCat[1]==7 && perCat[2]==3 && perCat[3]==4 && perCat[4]==5 && perCat[5]==3);

    // ---- clamp / step
    auto const& sp = kDefs[idx(F::Speed)];
    assert(clampStep(sp, 5.0) == 1.0 && clampStep(sp, -1.0) == 0.1);
    assert(std::fabs(clampStep(sp, 0.52) - 0.5) < 1e-9);
    assert(std::fabs(stepped(sp, 1.0, -1) - 0.95) < 1e-9);
    assert(stepped(sp, 1.0, +1) == 1.0);                           // cannot go above the max
    assert(std::fabs(stepped(sp, 0.1, -1) - 0.1) < 1e-9);          // nor under the min
    assert(clampStep(kDefs[0], 0.7) == 1.0 && clampStep(kDefs[0], 0.2) == 0.0);
    assert(clampStep(sp, NAN) == 1.0);                             // NaN falls back to the default

    // ---- config file round trip + hostile input
    Values a;
    a.set(F::Noclip, 1); a.set(F::Speed, 0.35); a.set(F::RespawnTime, 1.2); a.set(F::BallX, 321); a.set(F::BallY, 777); a.set(F::Language, 2);
    Values b;
    parse(b, serialize(a));
    for (int i = 0; i < kCount; ++i) assert(std::fabs(a.v[i] - b.v[i]) < 1e-6);
    Values c;
    parse(c, "garbage\n=5\nnoclip=abc\nspeed=99\nunknown=1\nball_size=-4\r\nhud_fps = 1\nno_glow=1\n");
    assert(c.num(F::Speed) == 1.0);                                // clamped
    assert(c.num(F::BallSize) == 30.0);                            // clamped to min
    Values old;                                                    // an old config (with the removed ball_corner key) still loads
    parse(old, "noclip=1\nball_corner=3\nlanguage=9\n");
    assert(old.on(F::Noclip) && old.num(F::Language) == 2.0);
    assert(!c.on(F::Noclip));                                      // "abc" ignored
    assert(c.on(F::NoGlow));                                       // valid line still applied
    assert(c.on(F::PracticeOnly));                                 // untouched default stays on

    // ---- presets keep the safety and the ball
    Values p;
    p.set(F::PracticeOnly, 0); p.set(F::BallX, 400); p.set(F::BallY, 600); p.set(F::BallSize, 90); p.set(F::BallOpacity, 40); p.set(F::Language, 1); p.set(F::BallInPause, 0);
    p.set(F::Noclip, 1); p.set(F::JumpHack, 1);
    applyPreset(p, Preset::Panic);
    assert(!p.on(F::Noclip) && !p.on(F::JumpHack) && !anyCheatEnabled(p));
    assert(!p.on(F::PracticeOnly) && p.num(F::BallX) == 400 && p.num(F::BallY) == 600 && p.num(F::BallSize) == 90 && p.num(F::BallOpacity) == 40);
    assert(p.num(F::Language) == 1 && !p.on(F::BallInPause));    // presets keep the language and the ball settings
    applyPreset(p, Preset::Practice);
    assert(p.on(F::Respawn) && p.on(F::HudStats) && p.on(F::HudAcc) && !anyCheatEnabled(p));
    applyPreset(p, Preset::Performance);
    assert(p.on(F::AutoLdm) && p.on(F::NoGlow) && p.on(F::NoShaders) && p.on(F::NoParticles) && p.on(F::Eco) && p.on(F::HudFps));
    assert(!p.on(F::Respawn) && !anyCheatEnabled(p));              // presets never enable cheats
    applyPreset(p, Preset::MinimalVisuals);
    assert(p.on(F::NoDeathFx) && p.on(F::NoParticles) && p.on(F::NoGlow) && !p.on(F::NoShaders));
    for (int li = 0; li < kLangCount; ++li)
        for (int i = 0; i < kPresetCount; ++i)
            assert(presetName(static_cast<Lang>(li), static_cast<Preset>(i))[0] != '\0' && presetDesc(static_cast<Lang>(li), static_cast<Preset>(i))[0] != '\0');

    // ---- cheats gating
    assert(cheatsAllowed(true, true, false) && cheatsAllowed(true, false, true));
    assert(!cheatsAllowed(true, false, false));
    assert(cheatsAllowed(false, false, false));
    Values g;
    assert(!anyCheatEnabled(g));
    g.set(F::Speed, 0.5);  assert(anyCheatEnabled(g));
    assert(speedFactor(g, true) == 0.5 && speedFactor(g, false) == 1.0);
    g.set(F::Speed, 1.0);  g.set(F::JumpHack, 1);  assert(anyCheatEnabled(g));

    // ---- noclip accuracy
    assert(accuracyPercent(0, 0) == 100.0 && accuracyPercent(100, 25) == 75.0);
    assert(accuracyPercent(10, 99) == 0.0);
    assert(noclipBlocksDeath(0.0, 1000, 999));                     // min 0 = never die
    assert(noclipBlocksDeath(90.0, 10, 9));                        // too early to judge
    assert(noclipBlocksDeath(90.0, 100, 5));                       // 94% >= 90%
    assert(!noclipBlocksDeath(90.0, 100, 10));                     // 11/100 -> 89% < 90%
    Attempt at; at.frames = 5; at.deathFrames = 2; at.reset(); assert(at.frames == 0 && at.deathFrames == 0);

    // ---- session stats
    SessionStats s;
    s.onAttempt(); s.onAttempt();
    s.onDeath(42.0); s.onDeath(43.0); s.onDeath(7.0); s.onDeath(250.0); s.onDeath(NAN);
    assert(s.attempts == 2 && s.deaths == 5 && s.furthest == 100.0);
    int cnt = 0;
    assert(s.hotZone(cnt) == 8 && cnt == 2);                       // 40-45% twice
    for (int i = 0; i < 20; ++i) s.onDeath(1.0);
    assert(s.last.size() == 10);
    SessionStats z; assert(z.hotZone(cnt) == -1 && cnt == 0);
    s.addTime(65.4); s.addTime(-3); assert(formatTime(s.seconds) == "1:05");
    auto lines = statsLines(s, 97.25);
    assert(lines.size() == 5);
    assert(lines[2].find("97.2%") != std::string::npos || lines[2].find("97.3%") != std::string::npos);
    assert(statsLines(z, 100.0)[3] == "Last deaths: none yet" && statsLines(z, 100.0)[4] == "Hot zone: none yet");
    s.reset(); assert(s.attempts == 0 && s.deaths == 0 && s.last.empty() && s.hotZone(cnt) == -1);

    // ---- FPS meter, eco
    FpsMeter m; FpsSample smp;
    for (int i = 0; i < 29; ++i) assert(!m.push(1.f / 60.f, 0.5f, smp));
    assert(m.push(1.f / 60.f, 0.5f, smp) && std::fabs(smp.fps - 60.f) < 0.5f);
    assert(!m.push(0.f, 0.5f, smp) && !m.push(-1.f, 0.5f, smp) && !m.push(NAN, 0.5f, smp));
    assert(updatePressure(Pressure::None, 49.f, 50) == Pressure::Low);
    assert(updatePressure(Pressure::Low, 52.f, 50) == Pressure::Low);   // hysteresis
    assert(updatePressure(Pressure::Low, 56.f, 50) == Pressure::None);
    assert(updatePressure(Pressure::None, 52.f, 50) == Pressure::None);

    // ---- HUD
    HudInput h;
    assert(hudText(h).empty());
    h.fps = true; h.sample.fps = 59.6f; h.sample.ms = 16.8f;
    SessionStats hs; hs.onAttempt(); hs.onDeath(12.5);
    h.stats = true; h.session = &hs; h.acc = true; h.accuracy = 88.0; h.cheats = true; h.cheatsOn = false;
    assert(hudText(h) == "60 FPS  16.8 ms\nAtt 1  Deaths 1  Best 12.5%\nAcc 88.0%");
    h.cheatsOn = true;
    assert(hudText(h).ends_with("\nCHEATS ON"));
    h.stats = false;
    assert(hudText(h) == "60 FPS  16.8 ms\nAcc 88.0%\nCHEATS ON");


    // ---- languages: nothing missing, Russian is really Russian, Spanish has its accents
    auto hasCyrillic = [](const char* t) {
        for (const unsigned char* c = reinterpret_cast<const unsigned char*>(t); *c; ++c) if (*c == 0xD0 || *c == 0xD1) return true;
        return false;
    };
    for (int i = 0; i < kStrCount; ++i)
        for (int li = 0; li < kLangCount; ++li) {
            const char* t = kStr[i][li];
            assert(t != nullptr && t[0] != '\0');
        }
    for (int i = 0; i < kStrCount; ++i) {                        // every Russian string except the pure "HUD" one has Cyrillic
        if (std::string(kStr[i][2]) != "HUD") assert(hasCyrillic(kStr[i][2]));
        if (i != static_cast<int>(S::TabHud) && std::string(kStr[i][1]) != std::string(kStr[i][0])) assert(!hasCyrillic(kStr[i][1]));
    }
    for (int i = 0; i < kCount; ++i) {
        assert(kNamesEs[i] && kNamesEs[i][0] && kDescsEs[i] && kDescsEs[i][0]);
        assert(kNamesRu[i] && hasCyrillic(kNamesRu[i]) && kDescsRu[i] && hasCyrillic(kDescsRu[i]));
        assert(std::string(featureName(Lang::En, i)) == kDefs[i].name && std::string(featureDesc(Lang::En, i)) == kDefs[i].desc);
        assert(std::string(featureName(Lang::Es, i)) == kNamesEs[i] && std::string(featureName(Lang::Ru, i)) == kNamesRu[i]);
    }
    assert(std::string(featureName(Lang::Es, idx(F::PracticeOnly))) == "Modo seguro");
    assert(std::string(featureName(Lang::En, idx(F::PracticeOnly))) == "Safe mode");
    assert(std::string(featureName(Lang::Ru, -1)).empty() && std::string(featureName(Lang::Ru, kCount)).empty());
    assert(std::string(tr(Lang::En, S::SafeOn)) == "Safe mode: ON" && std::string(tr(Lang::Es, S::SafeOff)) == "Modo seguro: NO");
    assert(utf8Length("abc") == 3 && utf8Length("\u0418\u0433\u0440\u043e\u043a") == 5 && utf8Length("Espa\u00f1ol") == 7 && utf8Length("") == 0);
    assert(std::string(langName(Lang::En)) == "English" && std::string(langName(Lang::Es)) == "Espa\u00f1ol" && utf8Length(langName(Lang::Ru)) == 7);
    Values lv;
    assert(langOf(lv) == Lang::En);
    lv.set(F::Language, 1); assert(langOf(lv) == Lang::Es);
    lv.set(F::Language, 2); assert(langOf(lv) == Lang::Ru);
    lv.v[idx(F::Language)] = 77; assert(langOf(lv) == Lang::Ru);   // out-of-range falls back safely
    lv.v[idx(F::Language)] = -3; assert(langOf(lv) == Lang::En);
    // translated stats and HUD
    SessionStats ts; ts.onAttempt(); ts.onDeath(12.5);
    auto es = statsLines(ts, 90.0, Lang::Es);
    assert(es[0] == "Intentos: 1   Muertes: 1" && es[4].find("muertes") != std::string::npos && es[3].find("\u00daltimas") == 0);
    auto ru = statsLines(ts, 90.0, Lang::Ru);
    assert(hasCyrillic(ru[0].c_str()) && ru[0].find("1") != std::string::npos);
    HudInput hl; hl.stats = true; hl.session = &ts; hl.cheats = true; hl.cheatsOn = true; hl.lang = Lang::Es;
    assert(hudText(hl) == "Int 1  Muertes 1  Mejor 12.5%\nTRAMPAS ACTIVAS");
    hl.lang = Lang::Ru; assert(hasCyrillic(hudText(hl).c_str()));
    hl.lang = Lang::En; assert(hudText(hl) == "Att 1  Deaths 1  Best 12.5%\nCHEATS ON");
    // the ball: saved position and the safe-mode switch live in the config
    Values bv; bv.set(F::BallX, 5000); bv.set(F::BallY, -9);
    assert(bv.num(F::BallX) == 1000 && bv.num(F::BallY) == 0);
    assert(bv.on(F::PracticeOnly) && bv.on(F::BallInPause));      // safe by default, ball in pause menu by default
    bv.set(F::PracticeOnly, 0);
    assert(cheatsAllowed(bv.on(F::PracticeOnly), false, false));  // safe mode OFF: cheats work in normal runs
    bv.set(F::PracticeOnly, 1);
    assert(!cheatsAllowed(bv.on(F::PracticeOnly), false, false)); // safe mode ON: blocked outside practice/test

    std::cout << "megamobile logic tests passed\n";
}
