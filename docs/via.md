# Remapping with VIA

Apollo58 runs VIA-enabled firmware, so you can change any key, layer or macro from a web browser. There's nothing to install and nothing to flash. Changes are saved on the keyboard itself, so they follow it to any computer.

## Open VIA

1. Plug the **left half** into your computer with the USB-C cable. The TRRS cable should already link the two halves.
2. In **Chrome, Edge or Brave**, go to **[usevia.app](https://usevia.app)**. Firefox and Safari can't talk to keyboards.
3. Click **Authorize device** and choose **Lily58** from the list. Apollo58 identifies itself as a Lily58, the open-source board it's based on.
4. Your layout appears. If VIA asks for a definition file, turn on **Settings → Show Design tab**, then load the Lily58 definition from the [VIA keyboards list](https://github.com/the-via/keyboards).

## Change a key

1. Pick the layer (0–3) at the top.
2. Click the key you want to change on the picture.
3. Click its new function in the lower panel. It takes effect immediately.

Useful sections in the lower panel:

- **Basic:** letters, numbers, symbols, modifiers.
- **Media:** play/pause, volume, browser back/forward.
- **Layers:** `MO(n)` holds a layer; `LT(n, key)` taps a key and holds a layer.
- **Special → Any:** type any QMK keycode. For example, `LCTL(KC_C)` is Ctrl+C and `LGUI(KC_C)` is Cmd+C.

## Macros

VIA has 16 macro slots (`M0`–`M15`). Record a sequence in the **Macros** tab, then assign `M0`, `M1` and so on to keys. When a macro fires, the left OLED shows `M0`, `M1` and so on for about a second.

## Save and restore your layout

- **Save:** in VIA, go to **Save + Load → Save current layout**. Keep the `.json` file somewhere safe.
- **Restore the factory layout:** go to **Save + Load → Load saved layout** and choose [`firmware/via/Apollo58layout.json`](../firmware/via/Apollo58layout.json) from this repository.
- **Full reset:** see [flashing.md → Reset to factory layout](flashing.md#reset-to-factory-layout).
