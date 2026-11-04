# Changelog

## v1.3.0: shipping firmware

v1.2.0's OLED firmware, with the shipping VIA layout built in as the default.

- **Layout:** [`firmware/via/Apollo58layout.json`](firmware/via/Apollo58layout.json) is now the layout compiled into the firmware, so a freshly flashed board needs no VIA import.
  - Space: tap for space, hold for layer 1 (media, F-keys, numbers, symbols).
  - Enter: tap for Enter, hold for layer 2 (mouse, arrows, Home/End/Del, brackets).
- **Media and browser keys now work.** v1.2.0 was built without QMK's "extra keys" feature, so play/pause, previous/next, volume and browser back/forward/refresh did nothing. v1.3.0 switches it on.
- **Caps Lock fixed.** The VIA layout used *Locking* Caps Lock (`KC_LCAP`), a code meant for old mechanically latching keys that most computers ignore. It's now the normal Caps Lock (`KC_CAPS`), in both the firmware and the VIA file.
- The OLED code is unchanged from v1.2.0.

## v1.2.0: OS-aware icons

- Shift, Ctrl and GUI fall as icons on the right OLED instead of letters.
- The GUI icon shows the Command symbol on macOS and the Windows logo on Windows (QMK OS detection).

## Earlier builds

The development history (first Apollo58 build → GT → GT2 → GT3) and the alternative animations are kept in [`oled-themes/`](oled-themes/).
