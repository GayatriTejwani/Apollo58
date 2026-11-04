# Apollo58 layout (firmware v1.3.0)

This is the layout every Apollo58 ships with. It's generated from [`firmware/via/Apollo58layout.json`](../firmware/via/Apollo58layout.json), which is the same keymap compiled into the firmware.

How to read the tables:

- The left five rows are the left half, the right five are the right half, and **‖** is the gap between them.
- `[` and `]` are the inner keys between the halves on the bottom letter row.
- The last row is the thumb cluster.
- **·** means "same as the base layer". A blank cell does nothing.

**Using the layers:** *tap* Space for a space, *hold* it for layer 1. *Tap* Enter for Enter, *hold* it for layer 2. Let go and you're back on the base layer.

## Base layer

| | | | | | | | ‖ | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Esc | 1 | 2 | 3 | 4 | 5 | &nbsp; | ‖ | &nbsp; | 6 | 7 | 8 | 9 | 0 | ` |
| Tab | Q | W | E | R | T | &nbsp; | ‖ | &nbsp; | Y | U | I | O | P | - |
| Ctrl | A | S | D | F | G | &nbsp; | ‖ | &nbsp; | H | J | K | L | ; | ' |
| Shift | Z | X | C | V | B | [ | ‖ | ] | N | M | , | . | / | Shift |
| &nbsp; | &nbsp; | &nbsp; | Caps Lock | Cmd/Win | Alt | Space / hold: L1 | ‖ | Enter / hold: L2 | Bksp | ↑ | ↓ | &nbsp; | &nbsp; | &nbsp; |

## Layer 1: hold Space

| | | | | | | | ‖ | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| · | ⏮ Prev | ⏯ Play | ⏭ Next | Vol − | Vol + | &nbsp; | ‖ | &nbsp; | Cut* | Copy* | Paste* | &nbsp; | &nbsp; | &nbsp; |
| F1 | F2 | F3 | F4 | F5 | F6 | &nbsp; | ‖ | &nbsp; | F7 | F8 | F9 | F10 | F11 | F12 |
| &nbsp; | 1 | 2 | 3 | 4 | 5 | &nbsp; | ‖ | &nbsp; | 6 | 7 | 8 | 9 | 0 | · |
| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | ‖ | &nbsp; | &nbsp; | ` | + | { | } | · |
| &nbsp; | &nbsp; | &nbsp; | · | · | Bksp | · | ‖ | · | · | · | · | &nbsp; | &nbsp; | &nbsp; |

## Layer 2: hold Enter

| | | | | | | | ‖ | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Browser ← | Browser → | · | · | · | · | &nbsp; | ‖ | &nbsp; | · | · | · | · | · | · |
| F1 | F2 | F3 | F4 | F5 | F6 | &nbsp; | ‖ | &nbsp; | F7 | F8 | F9 | F10 | F11 | F12 |
| · | Refresh | Scroll ↑ | Left click | Mouse ↑ | Right click | &nbsp; | ‖ | &nbsp; | &nbsp; | ← | ↓ | ↑ | → | &nbsp; |
| Scroll ← | Scroll → | Scroll ↓ | Mouse ← | Mouse ↓ | Mouse → | ( | ‖ | ) | + | = | [ | ] | \ | · |
| &nbsp; | &nbsp; | &nbsp; | Browser ← | Browser → | · | · | ‖ | · | Del | Home | End | &nbsp; | &nbsp; | &nbsp; |

## Layer 3: home-row modifiers (not reachable yet)

| | | | | | | | ‖ | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | ‖ | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; |
| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | ‖ | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; |
| &nbsp; | Shift | Ctrl | Cmd/Win | Alt | &nbsp; | &nbsp; | ‖ | &nbsp; | &nbsp; | Alt | Cmd/Win | Ctrl | Shift | &nbsp; |
| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | ‖ | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; |
| &nbsp; | &nbsp; | &nbsp; | · | · | · | · | ‖ | · | · | · | · | &nbsp; | &nbsp; | &nbsp; |

\* **Cut, Copy and Paste** send the dedicated keyboard Cut/Copy/Paste keys. Linux understands them, but Windows and macOS mostly ignore them. If they don't work on your computer, remap them in VIA to `Ctrl+X` / `Ctrl+C` / `Ctrl+V` (Windows) or `Cmd+X` / `Cmd+C` / `Cmd+V` (macOS). See [via.md](via.md).

**Layer 3** has Shift, Ctrl, Cmd/Win and Alt on the home row, but no key switches to it yet. To use it, assign `MO(3)` or `LT(3, …)` to a key in VIA.

## Change it

Every key on every layer can be changed in your browser with VIA, with no flashing needed. See [via.md](via.md).
