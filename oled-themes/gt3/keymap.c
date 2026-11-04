#include QMK_KEYBOARD_H
#include "transactions.h"
#include <string.h>
#include <stdio.h>

// ===== Layout from Apollo58-GT VIA export (baked in as the default) =====
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[0] = LAYOUT(
  KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_EQL,
  KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_MINS,
  KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_UP, KC_QUOT,
  KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_SCLN, KC_COMM, KC_N, KC_M, KC_DOT, KC_LEFT, KC_DOWN, KC_RGHT,
  KC_LALT, KC_LGUI, MO(1), KC_SPC, KC_ENT, KC_SLSH, KC_BSPC, MO(2)
),

[1] = LAYOUT(
  KC_GRV, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_UP, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MC_1, KC_LBRC, KC_RBRC, XXXXXXX, XXXXXXX, XXXXXXX, MS_LEFT, MS_DOWN, MS_RGHT,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MC_0, XXXXXXX, KC_CAPS
),

[2] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_BTN1, MS_WHLU, MS_BTN2,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_WHLD, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
),

[3] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
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

typedef struct { uint8_t col; int8_t row; char ch; bool active; } drop_t;

static drop_t   drops[MAX_DROPS];
static uint32_t rain_timer = 0;
static uint16_t rng        = 0xC0DE;
static char     outbox[24];
static uint8_t  outbox_len = 0;

static uint16_t xorshift16(void) {
    rng ^= rng << 7; rng ^= rng >> 9; rng ^= rng << 8; return rng;
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
        // Modifiers rain too, as lowercase mnemonics so they read apart from
        // the UPPERCASE typed letters:  s=Shift  c=Ctrl  a=Alt  g=GUI
        case KC_LSFT: case KC_RSFT: return 's';
        case KC_LCTL: case KC_RCTL: return 'c';
        case KC_LALT: case KC_RALT: return 'a';
        case KC_LGUI: case KC_RGUI: return 'g';
        default:      return 0;
    }
}

static void spawn_letter(char ch) {
    if (ch == 0) return;
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) {
            drops[i].col = xorshift16() % LR_COLS;
            drops[i].row = 0; drops[i].ch = ch; drops[i].active = true;
            return;
        }
    }
    uint8_t v = xorshift16() % MAX_DROPS;
    drops[v].col = xorshift16() % LR_COLS; drops[v].row = 0; drops[v].ch = ch;
}

static void keychars_slave_handler(uint8_t in_size, const void *in_data,
                                   uint8_t out_size, void *out_data) {
    if (is_keyboard_left()) return;      // left half is the info panel
    const uint8_t *d = (const uint8_t *)in_data;
    if (in_size < 1) return;
    uint8_t n = d[0];
    for (uint8_t i = 0; i < n && (uint8_t)(i + 1) < in_size; i++) {
        spawn_letter((char)d[i + 1]);
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
        char c = keycode_to_char(keycode);
        if (c) {
            rng ^= timer_read();
            if (!is_keyboard_left()) spawn_letter(c);     // master IS the rain half
            if (outbox_len < sizeof(outbox)) outbox[outbox_len++] = c;
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
        oled_set_cursor(drops[i].col, drops[i].row);
        oled_write_char(drops[i].ch, false);
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
    // Caps Lock: only shown (highlighted) while it is actually ON. (cols 0-8)
    int8_t caps = host_keyboard_led_state().caps_lock ? 1 : 0;
    if (caps != last_caps) {
        oled_set_cursor(0, 3);
        oled_write(caps ? "CAPS LOCK" : "         ", caps ? true : false);
        last_caps = caps;
    }
    // Macro status, adjacent to Caps (cols 11+): shows M<n> briefly when fired.
    int8_t show_macro = (current_macro >= 0 && timer_elapsed32(macro_shown_at) < MACRO_SHOW_MS)
                            ? current_macro : -1;
    if (show_macro != last_macro_shown) {
        char mb[5];
        if (show_macro >= 0) snprintf(mb, sizeof(mb), "M%-2d", show_macro);
        else                 strcpy(mb, "   ");
        oled_set_cursor(11, 3);
        oled_write(mb, show_macro >= 0 ? true : false);   // highlight while active
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
