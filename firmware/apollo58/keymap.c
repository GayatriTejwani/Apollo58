/* Apollo58 firmware v1.3.0
 * Copyright (C) 2026 Reactiby Studio
 *
 * Based on the Lily58 by kata0510 and built on QMK Firmware.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * v1.3.0 = the v1.2.0 OLED firmware with the shipping layout
 * (via/Apollo58layout.json) built in as the default keymap.
 */
#include QMK_KEYBOARD_H
#include "transactions.h"
#include "os_detection.h"
#include <string.h>
#include <stdio.h>

// ===== Shipping layout (from via/Apollo58layout.json) =====
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* BASE: letters, numbers, Space (hold = layer 1), Enter (hold = layer 2) */
[0] = LAYOUT(
  KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_GRV,
  KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_MINS,
  KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
  KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_LBRC, KC_RBRC, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
                                  KC_CAPS, KC_LGUI, KC_LALT, LT(1,KC_SPC), LT(2,KC_ENT), KC_BSPC, KC_UP, KC_DOWN
),

/* LAYER 1 (hold Space): media, F-keys, numbers, symbols */
[1] = LAYOUT(
  _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_VOLD, KC_VOLU, KC_CUT, KC_COPY, KC_PASTE, XXXXXXX, XXXXXXX, XXXXXXX,
  KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
  XXXXXXX, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, _______,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_GRV, S(KC_EQL), S(KC_LBRC), S(KC_RBRC), _______,
                                  _______, _______, KC_BSPC, _______, _______, _______, _______, _______
),

/* LAYER 2 (hold Enter): mouse, arrows, navigation, brackets */
[2] = LAYOUT(
  KC_WWW_BACK, KC_WWW_FORWARD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
  _______, KC_WWW_REFRESH, MS_WHLU, MS_BTN1, MS_UP, MS_BTN2, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, XXXXXXX,
  MS_WHLL, MS_WHLR, MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, S(KC_9), S(KC_0), S(KC_EQL), KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, _______,
                                  KC_WWW_BACK, KC_WWW_FORWARD, _______, _______, _______, KC_DEL, KC_HOME, KC_END
),

/* LAYER 3: home-row modifiers (no key switches to this layer yet) */
[3] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, KC_LSFT, KC_LCTL, KC_LGUI, KC_LALT, XXXXXXX, XXXXXXX, KC_RALT, KC_RGUI, KC_RCTL, KC_RSFT, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                  _______, _______, _______, _______, _______, _______, _______, _______
)

};

#ifdef OLED_ENABLE

/* ---------------- RIGHT HALF: falling real keystrokes ---------------- */
#define LR_COLS   5
#define LR_ROWS   16
#define MAX_DROPS 16
#define STEP_MS   90

// glyph: 0 = render drop.ch via the font; >0 = render a custom modifier glyph.
enum { GLYPH_NONE = 0, GLYPH_SHIFT = 1, GLYPH_CTRL = 2, GLYPH_GUI = 3 };

typedef struct { uint8_t col; int8_t row; char ch; uint8_t glyph; bool active; } drop_t;

static drop_t   drops[MAX_DROPS];
static uint32_t rain_timer = 0;
static uint16_t rng        = 0xC0DE;
static uint8_t  outbox[24];    // each byte: glyph id (1..3) or an ASCII char (>=0x20)
static uint8_t  outbox_len = 0;

static uint16_t xorshift16(void) {
    rng ^= rng << 7; rng ^= rng >> 9; rng ^= rng << 8; return rng;
}

// 6x8 modifier glyphs (bit5=leftmost column). GUI picks Cmd vs Windows by host OS.
static const uint8_t PROGMEM g_shift[8] = {0x0C,0x1E,0x3F,0x0C,0x0C,0x0C,0x00,0x00};
static const uint8_t PROGMEM g_ctrl[8]  = {0x00,0x0C,0x12,0x21,0x00,0x00,0x00,0x00};
static const uint8_t PROGMEM g_cmd[8]   = {0x00,0x33,0x2D,0x1E,0x1E,0x2D,0x33,0x00}; // Mac Command
static const uint8_t PROGMEM g_win[8]   = {0x00,0x36,0x36,0x00,0x36,0x36,0x00,0x00}; // Windows logo

static void draw_glyph(uint8_t col, uint8_t row, uint8_t gid) {
    const uint8_t *bm;
    switch (gid) {
        case GLYPH_SHIFT: bm = g_shift; break;
        case GLYPH_CTRL:  bm = g_ctrl;  break;
        case GLYPH_GUI:   bm = (detected_host_os() == OS_WINDOWS) ? g_win : g_cmd; break;
        default: return;
    }
    uint8_t px = col * 6, py = row * 8;
    for (uint8_t gy = 0; gy < 8; gy++) {
        uint8_t bits = pgm_read_byte(&bm[gy]);
        for (uint8_t gx = 0; gx < 6; gx++) {
            if (bits & (1 << (5 - gx))) oled_write_pixel(px + gx, py + gy, true);
        }
    }
}

static char keycode_to_char(uint16_t kc) {
    if (kc >= KC_A && kc <= KC_Z) return 'A' + (kc - KC_A);
    if (kc >= KC_1 && kc <= KC_9) return '1' + (kc - KC_1);
    switch (kc) {
        case KC_0:    return '0';
        case KC_MINS: return '-';
        case KC_EQL:  return '=';
        case KC_LBRC: return '[';
        case KC_RBRC: return ']';
        case KC_BSLS: return '\\';
        case KC_SCLN: return ';';
        case KC_QUOT: return '\'';
        case KC_GRV:  return '`';
        case KC_COMM: return ',';
        case KC_DOT:  return '.';
        case KC_SLSH: return '/';
        default:      return 0;   // modifiers handled separately (as glyphs)
    }
}

static void spawn_drop(char ch, uint8_t glyph) {
    if (ch == 0 && glyph == 0) return;
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) {
            drops[i].col = xorshift16() % LR_COLS;
            drops[i].row = 0; drops[i].ch = ch; drops[i].glyph = glyph; drops[i].active = true;
            return;
        }
    }
    uint8_t v = xorshift16() % MAX_DROPS;   // pool full: recycle
    drops[v].col = xorshift16() % LR_COLS; drops[v].row = 0; drops[v].ch = ch; drops[v].glyph = glyph;
}

static void keychars_slave_handler(uint8_t in_size, const void *in_data,
                                   uint8_t out_size, void *out_data) {
    if (is_keyboard_left()) return;      // left half is the info panel
    const uint8_t *d = (const uint8_t *)in_data;
    if (in_size < 1) return;
    uint8_t n = d[0];
    for (uint8_t i = 0; i < n && (uint8_t)(i + 1) < in_size; i++) {
        uint8_t b = d[i + 1];
        if (b >= 0x20) spawn_drop((char)b, GLYPH_NONE);  // ASCII char
        else           spawn_drop(0, b);                 // modifier glyph id
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(RPC_ID_USER_KEYCHARS, keychars_slave_handler);
}

// Which VIA macro is currently flagged on the left panel (-1 = none).
#define MACRO_SHOW_MS 1200
static int8_t   current_macro    = -1;
static uint32_t macro_shown_at   = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (IS_MACRO_KEYCODE(keycode)) {            // VIA macro fired (MC_0..MC_15)
            current_macro  = (int8_t)(keycode - QK_MACRO_0);
            macro_shown_at = timer_read32();
        }
        char    c = 0;
        uint8_t g = 0;
        switch (keycode) {                 // modifiers rain as glyphs; Alt as ASCII '~'
            case KC_LSFT: case KC_RSFT: g = GLYPH_SHIFT; break;
            case KC_LCTL: case KC_RCTL: g = GLYPH_CTRL;  break;
            case KC_LGUI: case KC_RGUI: g = GLYPH_GUI;   break;
            case KC_LALT: case KC_RALT: c = '~';         break;
            default:                    c = keycode_to_char(keycode); break;
        }
        if (c || g) {
            rng ^= timer_read();
            if (!is_keyboard_left()) spawn_drop(c, g);        // master IS the rain half
            uint8_t b = g ? g : (uint8_t)c;                   // glyph id (1..3) or ASCII
            if (outbox_len < sizeof(outbox)) outbox[outbox_len++] = b;
        }
    }
    return true;
}

void housekeeping_task_user(void) {
    if (is_keyboard_master() && outbox_len > 0) {
        uint8_t buf[1 + sizeof(outbox)];
        uint8_t n = outbox_len;
        buf[0] = n;
        memcpy(&buf[1], outbox, n);
        if (transaction_rpc_send(RPC_ID_USER_KEYCHARS, n + 1, buf)) outbox_len = 0;
    }
}

static void render_rain(void) {
    oled_clear();
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) continue;
        drops[i].row++;
        if (drops[i].row >= LR_ROWS) { drops[i].active = false; continue; }
        if (drops[i].glyph) {
            draw_glyph(drops[i].col, drops[i].row, drops[i].glyph);
        } else {
            oled_set_cursor(drops[i].col, drops[i].row);
            oled_write_char(drops[i].ch, false);
        }
    }
}

/* ---------------- LEFT HALF: landscape info panel ---------------- */
static bool     info_inited = false;
static uint8_t  last_wpm    = 255;
static int8_t   last_layer  = -1;
static int8_t   last_caps   = -1;
static int8_t   last_macro_shown = -1;
static uint32_t info_timer  = 0;

static const char *layer_name(uint8_t l) {
    switch (l) {
        case 0:  return "BASE ";
        case 1:  return "FN   ";
        case 2:  return "MOUSE";
        case 3:  return "L3   ";
        default: return "?????";
    }
}

static void render_info(void) {
    if (!info_inited) {
        oled_clear();
        oled_set_cursor(0, 0);
        oled_write("Apollo58", false);          // top line, left corner
        for (uint8_t x = 0; x < 128; x++) {       // full-length underline
            oled_write_pixel(x, 10, true);
        }
        info_inited = true; last_wpm = 255; last_layer = -1; last_caps = -1; last_macro_shown = -1;
    }
    uint8_t w   = get_current_wpm();
    uint8_t lyr = get_highest_layer(layer_state);
    if (w != last_wpm || (int8_t)lyr != last_layer) {
        char b[22];
        snprintf(b, sizeof(b), "WPM %-3u  %-5s", w, layer_name(lyr));  // WPM + layer adjacent
        oled_set_cursor(0, 2);
        oled_write(b, false);
        last_wpm = w; last_layer = lyr;
    }
    // Caps Lock: plain text (same style as WPM/layer), only shown while ON. (cols 0-8)
    int8_t caps = host_keyboard_led_state().caps_lock ? 1 : 0;
    if (caps != last_caps) {
        oled_set_cursor(0, 3);
        oled_write(caps ? "CAPS LOCK" : "         ", false);
        last_caps = caps;
    }
    // Macro status, adjacent to Caps (cols 11+): plain text, M<n> briefly when fired.
    int8_t show_macro = (current_macro >= 0 && timer_elapsed32(macro_shown_at) < MACRO_SHOW_MS)
                            ? current_macro : -1;
    if (show_macro != last_macro_shown) {
        char mb[5];
        if (show_macro >= 0) snprintf(mb, sizeof(mb), "M%-2d", show_macro);
        else                 strcpy(mb, "   ");
        oled_set_cursor(11, 3);
        oled_write(mb, false);
        last_macro_shown = show_macro;
    }
}

/* ---------------- init + task ---------------- */
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_left()) {
        return OLED_ROTATION_0;    // landscape info panel; swap to 180 if upside down
    }
    return OLED_ROTATION_270;      // right-half rain (unchanged)
}

bool oled_task_user(void) {
    if (is_keyboard_left()) {
        if (timer_elapsed32(info_timer) < 100) return false;
        info_timer = timer_read32();
        render_info();
        return false;
    }
    if (timer_elapsed32(rain_timer) < STEP_MS) return false;
    rain_timer = timer_read32();
    render_rain();
    return false;
}
#endif // OLED_ENABLE
