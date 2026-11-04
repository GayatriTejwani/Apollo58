# Troubleshooting

Start with the quick checks, then find your symptom below. If nothing here helps, email **reactiby@gmail.com** with what you tried.

## Quick checks

1. **The USB-C cable goes into the left half.**
2. Unplug USB, then **reseat the TRRS cable** firmly in both halves (it should click), then plug USB back in. *Never move the TRRS while USB is connected.*
3. Try a **different USB port** directly on the computer, not a hub.
4. Try **another USB-C cable**. Some cables only carry power and no data.

## The right half doesn't type, but the left half does

- The TRRS link is usually the cause. Unplug USB, then reseat the TRRS cable at both ends.
- Make sure it's a TRRS cable (4 contacts, three rings on the plug), not a stereo TRS cable (3 contacts, two rings). The cable in the box is TRRS.
- If you've flashed firmware recently, check that **both halves** have the same version. See [flashing.md](flashing.md).

## The keyboard isn't detected through a dock or USB hub

Some docks, including Thunderbolt docks, don't power up split keyboards reliably, because both halves have to start talking to each other before the computer sees the keyboard.

- Plug the keyboard **directly into the computer** and use the dock for everything else.
- If it worked through the dock before and suddenly stopped, **unplug the dock's power** for 30 seconds and reconnect it. This resets the dock's internal USB hub.
- A powered USB hub is usually more reliable than a dock.

## The OLED is blank, or shows on one side only

- Each screen is driven by its own half, so a blank right screen usually means the right half isn't connected. Check the TRRS cable.
- The screens are set to stay on, so a blank screen is never a sleep timeout.
- If you flashed a theme from `oled-themes/`, some themes only use one screen. That's expected.

## Media keys, volume or browser back/forward don't work

These need firmware **v1.3.0 or newer**. v1.2.0 was built without media key support, so these keys did nothing on it. Flash [`apollo58-v1.3.0.hex`](../firmware/apollo58/apollo58-v1.3.0.hex). See [flashing.md](flashing.md).

## Cut, Copy or Paste does nothing

These keys send the dedicated keyboard Cut/Copy/Paste codes, which Windows and macOS mostly ignore. Remap them in VIA to `LCTL(KC_X)`, `LCTL(KC_C)` and `LCTL(KC_V)` on Windows or Linux, or to `LGUI(KC_X)`, `LGUI(KC_C)` and `LGUI(KC_V)` on macOS. See [via.md](via.md).

## Space or Enter sometimes types nothing, or triggers a layer

Space and Enter are dual-purpose: tap for the key, hold for a layer. A *tap* has to be quick, under 100 ms in the shipping firmware. If you rest on the thumb keys while typing, you may see this.

- Release Space and Enter crisply rather than resting on them.
- In VIA, you can move layer access to separate thumb keys (`MO(1)`, `MO(2)`) and give Space and Enter back their plain keycodes.

## A single key doesn't work

1. Unplug USB. Pull the keycap and switch (see the [manual](Apollo58_User_Manual.pdf)).
2. Check the switch's two metal pins are **straight**. A bent pin is the most common cause.
3. Re-seat the switch, pressing straight down until it clicks.
4. Still dead? Swap in one of the spare switches from the box to rule out the switch itself.

## VIA doesn't find the keyboard

- Use **Chrome, Edge or Brave**. Firefox and Safari don't support the connection VIA uses.
- Look for **Lily58** in the device list, not "Apollo58".
- On Linux, you may need a udev rule that allows access to the keyboard. Search "VIA udev rule".
- Close any other app that's connected to the keyboard, such as a second VIA tab.
