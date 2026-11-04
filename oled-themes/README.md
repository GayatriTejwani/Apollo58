# OLED themes and earlier builds

Every OLED experiment we built while developing Apollo58. Each folder has the full QMK source and a ready-to-flash `.hex`. All of them compile against current QMK.

They're here for fun and for reference. **The shipping firmware is [`firmware/apollo58/`](../firmware/apollo58/).**

> **These builds use older layouts, not the shipping one.** The development builds use the Apollo58-GT layout (arrow keys on the right). The early experiments use the stock Lily58 layout (Lower/Raise layers). Flashing one also resets any VIA customisation, so save your layout in VIA first. To go back, flash [`apollo58-v1.3.0.hex`](../firmware/apollo58/apollo58-v1.3.0.hex).

## Development history of the shipping firmware

| Folder | Left OLED | Right OLED |
|---|---|---|
| [`first-apollo58`](first-apollo58/) | Vertical "Apollo58" nameplate, WPM, layer | Real keystrokes falling as rain |
| [`gt`](gt/) | Landscape info panel: name, WPM, layer | Keystroke rain. First build with the Apollo58-GT layout |
| [`gt2`](gt2/) | Adds a Caps Lock indicator | Modifiers fall as letters: `s` Shift, `c` Ctrl, `a` Alt, `g` GUI |
| [`gt3`](gt3/) | Adds the VIA macro indicator (`M0`–`M15`) | Same as gt2 |
| [`v1.2.0-keyrain-info`](v1.2.0-keyrain-info/) | Same as gt3 | Modifiers fall as icons, with Cmd or Windows chosen automatically. **Base of the shipping v1.3.0** |

## Alternative animations

| Folder | What it shows |
|---|---|
| [`keyrain`](keyrain/) | Abstract rain drops that pour harder the faster you type |
| [`letterrain`](letterrain/) | Matrix-style falling letters: the letter you typed, or a random glyph for Space, Enter and modifiers |
| [`keyfall`](keyfall/) | Only real character keys fall. Space, Enter and modifiers are ignored |
| [`wpmvolt`](wpmvolt/) | A bar chart of your typing speed over the last four minutes plus live WPM, with the controller's supply voltage |
| [`bongocat`](bongocat/) | A bongo cat that drums along with your typing speed. OLED code by [HorrorTroll](https://github.com/HorrorTroll) (GPL-2.0-or-later) |

## Build one

```sh
cp -r oled-themes/keyrain <qmk_firmware>/keyboards/lily58/keymaps/keyrain
qmk compile -kb lily58/rev1 -km keyrain
```
