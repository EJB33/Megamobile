# MegaMobile by EJB3 (Geode mod, GD 2.2081, Geode v5)

A touch-friendly mod menu. Mod id: `ejb3.megamobile`. All code is original; it was written by learning
from how existing menus are organized, not by copying them.

## How to use it
Tap the floating **MM** ball to open the menu. **Drag** the ball anywhere; its position is saved
(the **Reset ball** button at the bottom brings it back). While you play a level the ball is hidden;
it comes back when you pause (you can turn that off in Misc) and in every menu.

The menu has 11 tabs in a **horizontal bar** at the top; the arrows at both ends slide the bar left and right.
It works with toggles, +/- buttons and pages (no scrolling):
**Player, Visual, Perf, HUD, Misc, Macro, Clicks, StartPos, Keys, Presets, Stats**. Tap **?** on a row and its explanation appears under the rows.
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

## Features (34 options + 3 hidden settings)
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

## Clicks tab (Show clicks)
Draws a circle where you tap the screen, in menus and in levels. It fades out and grows a little.
- **Page 1:** Show clicks on/off, circle size, opacity, time, and a row of 8 ready-made colors (tap one).
- **Page 2:** red, green and blue (0-255, steps of 5) and a preview circle with the real color and size.
The circle never blocks your tap: the touch always goes through to the game.

## StartPos tab (change start position)
Shows the start positions (StartPos objects) of the level you are in and lets you move between them
(Previous / Next). The first one after the last is the normal start of the level.
- It only works inside a level, so open the menu from the pause menu (the ball shows there).
- If the level is paused, the change is applied as soon as you resume. With a key (see Keys) it is immediate.
- Switching starts the level in **test mode**, which does not save progress.
- Levels without StartPos objects say so.

## Keys tab (key binds)
Give any keyboard key to an action: tap **Set**, then press the key (**Esc** cancels). **X** clears it.
Actions: Jump player 1, Jump player 2, Open menu, Panic (all off), Macro record on/off, Macro play on/off,
StartPos previous / next, and **every on/off feature** (noclip, jump hack, no glow, show clicks...).
Toggling by key shows a short message at the top of the screen.
- One key belongs to one action: if you give it to a second one, the first loses it and the menu tells you.
- A bound key stops doing its normal game action (the mod uses it and does not pass it on).
- Safe mode is not on the list on purpose: it always needs the two taps in the menu.
- Needs a keyboard (a physical or Bluetooth one on a phone, or a PC). Keys are saved in the config file.
- Key names follow the game's key codes (letters, numbers, F1-F12, arrows, Space...); an unusual key
  shows as "Key N".

## Name
The mod is now called **MegaMod** (the name shown in Geode). The id stays `ejb3.megamobile`, so your saved
settings, keys and macros are kept. The art and the texts inside the menu are unchanged.

## Macro tab
Records the clicks you make in a level (jump, and left/right in platformer) with the exact physics tick
(240 ticks per second), and plays them back.

**Page 1 - controls**
- **Record**: press it, then *restart the level* (the recording starts at the beginning of every attempt;
  if you die, the new attempt starts a fresh recording, so what is kept is the last attempt). Play the level,
  then open the menu from the pause menu and press **Stop rec**.
- **Play**: press it, then *restart the level*. The macro presses the buttons for you; your own clicks are
  ignored while it plays. Press **Stop** to take control back.
- **Clear**: deletes the macro in memory.
- **Export**: copies the macro to the clipboard **and** saves a file `macro_YYYYMMDD_HHMMSS.mmacro` in the
  `macros` folder of the mod's save directory. Paste the text anywhere to share it.
- **Import**: reads the macro text from the clipboard.
- **Load file**: use the `<` `>` buttons to pick a saved `.mmacro` file, then **Load file**.

**Page 2 - Click extrapolation**
- **Click extrapolation** (off by default) and **Extrapolation ticks** (default 2, up to 12).
  While recording, every click is saved that many ticks earlier than it was detected, to make up for the delay
  between your finger and the game (1 tick = 4.2 ms). The order of the clicks is always kept, and the gap
  between a press and its release does not change.
  I could not find a standard meaning for this name in the menus I studied, so this is my definition.
  If you meant something else (for example sub-tick click timing), tell me and I change it.

**File format** (plain text, one click per line):
```
MMACRO 1
tps 240
120,1,1,1      <- tick, button (1 jump, 2 left, 3 right), player (1 or 2), down (1) or up (0)
135,1,1,0
```
Imports are checked: wrong lines are skipped, files with no valid click, unreasonable sizes or a missing
header are refused with the reason and your current macro is left untouched. A file that says `tps 60`
(or any other rate) is converted to 240 ticks per second when imported.

**Safe mode and macros:** playing a macro counts as a cheat. With Safe mode ON (default) it only plays in Practice
or Test mode; the HUD says so. Recording is always allowed.

**Limits (honest)**
- Inputs only: it does not correct the player's position. A macro can drift apart on levels with random
  triggers, on very different speeds, or if the physics differ; it is not frame-perfect on every level.
- Record from the very start of the level. A macro recorded from a practice checkpoint does not contain
  the earlier part, so it will not reproduce the run from the beginning.
- Timing has the resolution of one physics tick (4.2 ms).
- Only MegaMobile's own `.mmacro` format is supported, not the files of other bots.

## What is deliberately NOT included
I left out features whose purpose is to fake or force results, or to bypass the game's protections:
instant level completion, verify/"anti-cheat" bypasses, and shop or online unlocks. Cheats are limited to
Practice and Test mode by default. If you turn that safety off, remember that cheating in normal runs is
your responsibility.

## Honest limits
- It cannot contain "every function of every menu": the menus I studied have between 8,000 and 41,000
  lines each. This is a solid, organized base with a registry that makes adding features simple.
- Missing compared to big menus: hitboxes, frame stepper, startpos switcher, video recording, keybinds,
  position correction for macros. Say which ones you want and I add them in the next version.
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
