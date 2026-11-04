# MegaMobile by EJB3 (Geode mod, GD 2.2081, Geode v5)

A touch-friendly mod menu. Mod id: `ejb3.megamobile`. All code is original; it was written by learning
from how existing menus are organized, not by copying them.

## How to use it
Tap the floating **MM** ball to open the menu. **Drag** the ball anywhere; its position is saved
(the **Reset ball** button at the bottom brings it back). While you play a level the ball is hidden;
it comes back when you pause (you can turn that off in Misc) and in every menu.

The menu has 7 tabs and works with toggles, +/- buttons and pages (no scrolling):
**Player, Visual, Perf, HUD, Misc, Presets, Stats**. Tap **?** on a row and its explanation appears under the rows.
Your choices are saved automatically (`megamobile-config.txt` in the mod's save folder).

**Bottom buttons (always visible):**
- **Safe mode: ON/OFF** (left). ON (default): noclip, jump hack and game speed only work in Practice or Test mode.
  To turn it OFF you tap twice: the first tap only shows a warning. Turning it ON again is one tap.
- **Language** (center): English, Espanol, Russian. Changes the whole menu and the HUD.
- **Reset ball** (right).

## Languages
English, Spanish and Russian. English uses the game's own fonts. For Spanish and Russian the mod
builds its own font from DejaVu Sans Bold (free license, see `LICENSE-DejaVu.txt`) because the game's
fonts do not cover Cyrillic reliably. If that font could not be built, the mod falls back to the game's fonts
(Russian letters could then look empty: use English).

## Features (25 options + 3 hidden settings)
**Player**
- Noclip (+ **Noclip min accuracy %**: if your accuracy in the attempt falls under it, you can die again)
- Jump hack
- Game speed 0.10 to 1.00 (slow motion for practice)
- Custom respawn time (+ Respawn time in seconds)

**Visual**: No glow (applies to objects placed from then on; restart the level), No shaders, No particles,
No death effect, Hide player, No camera triggers, No mirror

**Perf**: Auto Low Detail (turns LDM on when you open a level page), **Eco mode** (when FPS falls under the
Eco target, shaders and new particles switch off until it recovers), Eco target FPS

**HUD** (top-left of the level): Show FPS, Show session stats, Show noclip accuracy, Cheat indicator

**Stats tab**: attempts, deaths, furthest death, last deaths, and your **hot zone** (the 5% stretch of the
level where you die most). Reset button included.

**Presets**: Panic (everything off), Practice, Performance, Minimal visuals. Presets never turn on a cheat
and never change the ball or the practice-only safety.

**Misc**: Unlock all icons (local to your device), **Safe mode** (on by default, it limits cheats to Practice and Test mode),
ball size / opacity / show-in-pause.

## What is deliberately NOT included
I left out features whose purpose is to fake or force results, or to bypass the game's protections:
instant level completion, verify/"anti-cheat" bypasses, and shop or online unlocks. Cheats are limited to
Practice and Test mode by default. If you turn that safety off, remember that cheating in normal runs is
your responsibility.

## Honest limits
- It cannot contain "every function of every menu": the menus I studied have between 8,000 and 41,000
  lines each. This is a solid, organized base with a registry that makes adding features simple.
- Missing compared to big menus: hitboxes, bots/macros, frame stepper, startpos switcher, video recording,
  keybinds. Say which ones you want and I add them in the next version.
- Written for Geode 5.11.2 / GD 2.2081 using function signatures confirmed in a mod that works on that
  version, but I could not compile or run it here. If the build fails, send me the first error lines.
- Speed under 1.00 slows the whole game during levels (not while paused); sound speed is not changed.
- "No glow" and "Auto Low Detail" depend on game fields I verified exist; effects vary per level.

## Build with GitHub Actions (no PC needed)
1. Upload everything in this folder to a repository. If the hidden `.github` folder does not upload from
   your phone, use Add file > Create new file, type `.github/workflows/build.yml` as the name and paste
   that file's content.
2. Open the Actions tab and wait for "Build MegaMobile" (5-10 minutes).
3. Download the "Build Output" artifact, unzip it and put `ejb3.megamobile.geode` in Geode's mods folder.
   The first build also generates the Spanish/Russian font, so it can take a little longer.

## If the build fails
- Geode v5 needs C++23 (already set in CMakeLists.txt).
- "mod.json geode version" mismatch: set `"geode"` in mod.json to the SDK version shown in the log and
  make sure your installed Geode is at least that version.
- A compile error on a function or class name: send the first error lines of the log.

## Tests
`tests/test_logic.cpp` tests the registry, presets, config file, stats and HUD text on any PC:
`g++ -std=c++23 -o t tests/test_logic.cpp && ./t`

## Files
`src/logic.hpp` pure logic and all texts in 3 languages (tested) - `src/state.hpp` shared state and save file -
`src/hooks.cpp` all game hooks - `src/menu.cpp` floating ball and menu.
To add a feature: add one line to `kDefs` and one enum value in `logic.hpp` (same position), then read it
with `mm::vals().on(mm::F::YourFeature)` in a hook. The menu builds its rows from the table by itself.
