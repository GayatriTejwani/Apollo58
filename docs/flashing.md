# Flashing firmware

You only need this to install a new firmware version or try an OLED theme. To **change keys**, use [VIA](via.md) instead, which needs no flashing.

> **Both halves need the same firmware.** You flash one half at a time, using the USB-C cable on each half in turn.
>
> **Unplug the TRRS cable before you start,** and only reconnect it when both halves are done and the USB cable is unplugged.

## What you need

- The `.hex` file you want to install. The shipping firmware is [`firmware/apollo58/apollo58-v1.3.0.hex`](../firmware/apollo58/apollo58-v1.3.0.hex). OLED themes are in [`oled-themes/`](../oled-themes/).
- **[QMK Toolbox](https://github.com/qmk/qmk_toolbox/releases)** (Windows/macOS). It's free.
  On Linux, use `avrdude` or the `qmk flash` command instead.

## Steps

1. **Unplug everything:** first USB-C, then TRRS.
2. Open QMK Toolbox. Click **Open** and select the `.hex` file. Set the MCU to **atmega32u4**.
3. Tick **Auto-Flash**.
4. **Put the left half into flashing mode.** Hold the **top outer key** (Esc on the left half) and plug in the USB-C cable. Keep holding for a second, then let go.
   *Alternative:* press the **reset button** on the controller twice quickly.
5. QMK Toolbox shows a yellow "Caterina device connected" line and flashes the board automatically. Wait for **"Flash complete"**.
6. Unplug the USB-C cable.
7. **Repeat for the right half.** Hold its **top outer key** (the top-right corner key) while plugging in the USB-C cable.
8. With USB unplugged, reconnect the TRRS cable, then plug USB-C back into the **left** half.

The Pro Micro only stays in flashing mode for about 8 seconds. If you miss the window, enter flashing mode again (step 4 or 7).

## Reset to factory layout

Holding the top outer key while plugging in also **clears the layout saved by VIA**, so the keyboard returns to the layout built into the firmware. To restore the shipping layout without flashing, see [via.md](via.md#save-and-restore-your-layout).

## Flashing a new firmware version resets VIA

A new firmware build resets the VIA layout to the one built into that firmware. If you've customised your layout, **save it in VIA first** and load it back after flashing.

## If something goes wrong

A Pro Micro is very hard to brick. If the board stops responding after a flash, enter flashing mode again (step 4) and flash the shipping firmware. If that doesn't work, email **reactiby@gmail.com** before opening the case.
