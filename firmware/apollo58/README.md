# Apollo58 shipping firmware: v1.3.0

This is the firmware every Apollo58 ships with.

| File | What it is |
|---|---|
| `apollo58-v1.3.0.hex` | Ready to flash. See [docs/flashing.md](../../docs/flashing.md). |
| `keymap.c` | Layout and OLED code |
| `config.h` | Split-sync and timing settings |
| `rules.mk` | QMK features switched on |

**v1.3.0** is the v1.2.0 OLED firmware ([`oled-themes/v1.2.0-keyrain-info`](../../oled-themes/v1.2.0-keyrain-info/)) with the shipping layout ([`firmware/via/Apollo58layout.json`](../via/Apollo58layout.json)) built in as the default. Changes from v1.2.0 are listed in [CHANGELOG.md](../../CHANGELOG.md).

## Build it yourself

You'll need [QMK set up](https://docs.qmk.fm/newbs_getting_started) on your computer.

```sh
cp -r firmware/apollo58 <qmk_firmware>/keyboards/lily58/keymaps/apollo58
qmk compile -kb lily58/rev1 -km apollo58
```

The build produces `lily58_rev1_apollo58.hex`, at about 26 KB of the Pro Micro's 28 KB. Built with QMK `master` from 29 October 2026.

## How the OLEDs work

- **Left half:** a landscape info panel. It shows the name, WPM, active layer (`BASE`, `FN`, `MOUSE`, `L3`), Caps Lock, and the VIA macro number for about 1.2 s after a macro fires.
- **Right half:** a portrait "rain" of real keystrokes. The half plugged into USB sees every key, then sends each typed character to the right half over a custom split transaction (`RPC_ID_USER_KEYCHARS`). Shift, Ctrl and GUI fall as 6×8 pixel icons. QMK's OS detection chooses a Command icon (macOS) or a Windows logo for GUI.
