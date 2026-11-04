#include QMK_KEYBOARD_H
#include "transactions.h"
#include <string.h>
#include <stdio.h>

enum layer_number {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _ADJUST,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY */
[_QWERTY] = LAYOUT(
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_GRV,
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, KC_LBRC,  KC_RBRC,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_RSFT,
                        KC_LALT, KC_LGUI, MO(_LOWER), KC_SPC, KC_ENT, MO(_RAISE), KC_BSPC, KC_RGUI
),

/* LOWER */
[_LOWER] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______,_______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  KC_GRV, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_TILD,
  _______, _______, _______, _______, _______, _______, _______, _______, XXXXXXX, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
                             _______, _______, _______, _______, _______,  _______, _______, _______
),

/* RAISE */
[_RAISE] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
  KC_F1,  KC_F2,    KC_F3,   KC_F4,   KC_F5,   KC_F6,                       XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX,
  KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,   _______, _______,  KC_PLUS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
                             _______, _______, _______,  _______, _______,  _______, _______, _______
),

/* ADJUST (LOWER + RAISE) -- mouse keys */
[_ADJUST] = LAYOUT(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   MS_BTN1, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN2,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   MS_BTN3, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                             _______, _______, _______, _______, _______, _______, _______, _______
)
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

#ifdef OLED_ENABLE

/* ---- shared: text grid is 5 cols x 16 rows in portrait (32x128, 6x8 font) ---- */
#define LR_COLS   5
#define LR_ROWS   16

/* =============== RIGHT HALF: falling real keystrokes =============== */
#define MAX_DROPS 16
#define STEP_MS   90

typedef struct {
    uint8_t col;
    int8_t  row;
    char    ch;
    bool    active;
} drop_t;

static drop_t   drops[MAX_DROPS];
static uint32_t rain_timer = 0;
static uint16_t rng        = 0xC0DE;

static char    outbox[24];
static uint8_t outbox_len = 0;

static uint16_t xorshift16(void) {
    rng ^= rng << 7;
    rng ^= rng >> 9;
    rng ^= rng << 8;
    return rng;
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
        default:      return 0;
    }
}

static void spawn_letter(char ch) {
    if (ch == 0) return;
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) {
            drops[i].col    = xorshift16() % LR_COLS;
            drops[i].row    = 0;
            drops[i].ch     = ch;
            drops[i].active = true;
            return;
        }
    }
    uint8_t v = xorshift16() % MAX_DROPS;
    drops[v].col = xorshift16() % LR_COLS;
    drops[v].row = 0;
    drops[v].ch  = ch;
}

// Runs on the SLAVE: characters forwarded from the master. Only the rain
// (right) half actually spawns drops.
static void keychars_slave_handler(uint8_t in_size, const void *in_data,
                                   uint8_t out_size, void *out_data) {
    if (is_keyboard_left()) return;      // left half shows the info panel
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

// Runs on the master (sees keys from BOTH halves). Spawn locally if the master
// IS the rain half; always forward to the other half for the slave-rain case.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        char c = keycode_to_char(keycode);
        if (c) {
            rng ^= timer_read();
            if (!is_keyboard_left()) {
                spawn_letter(c);
            }
            if (outbox_len < sizeof(outbox)) {
                outbox[outbox_len++] = c;
            }
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
        if (transaction_rpc_send(RPC_ID_USER_KEYCHARS, n + 1, buf)) {
            outbox_len = 0;
        }
    }
}

static void render_rain(void) {
    oled_clear();
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) continue;
        drops[i].row++;
        if (drops[i].row >= LR_ROWS) {
            drops[i].active = false;
            continue;
        }
        oled_set_cursor(drops[i].col, drops[i].row);
        oled_write_char(drops[i].ch, false);
    }
}

/* =============== LEFT HALF: info panel =============== */
static bool     info_inited = false;
static uint8_t  last_wpm    = 255;
static int8_t   last_layer  = -1;
static uint32_t info_timer  = 0;

static const char *layer_name(uint8_t l) {
    switch (l) {
        case _QWERTY: return "BASE ";
        case _LOWER:  return "LOWER";
        case _RAISE:  return "RAISE";
        case _ADJUST: return "ADJST";
        default:      return "?????";
    }
}

static void render_info(void) {
    if (!info_inited) {
        oled_clear();
        // "Apollo58" as a vertical nameplate down the centre column
        static const char title[] = "Apollo58";
        for (uint8_t i = 0; i < 8; i++) {
            oled_set_cursor(2, i);
            oled_write_char(title[i], false);
        }
        oled_set_cursor(0, 8);  oled_write("-----", false);
        oled_set_cursor(0, 9);  oled_write("WPM", false);
        oled_set_cursor(0, 12); oled_write("LYR", false);
        info_inited = true;
        last_wpm = 255;
        last_layer = -1;
    }

    uint8_t w = get_current_wpm();
    if (w != last_wpm) {
        char b[6];
        snprintf(b, sizeof(b), "%-5u", w);   // left-justified, clears old digits
        oled_set_cursor(0, 10);
        oled_write(b, false);
        last_wpm = w;
    }

    uint8_t lyr = get_highest_layer(layer_state);
    if ((int8_t)lyr != last_layer) {
        oled_set_cursor(0, 13);
        oled_write(layer_name(lyr), false);
        last_layer = lyr;
    }
}

/* =============== init + task =============== */
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // Right (rain) half is known-good at 270; the left OLED is mounted mirrored,
    // so it needs 90 to read upright. Swap 90<->270 if either looks flipped.
    if (is_keyboard_left()) {
        return OLED_ROTATION_90;
    }
    return OLED_ROTATION_270;
}

bool oled_task_user(void) {
    if (is_keyboard_left()) {
        if (timer_elapsed32(info_timer) < 100) {
            return false;
        }
        info_timer = timer_read32();
        render_info();
        return false;
    }

    // right half: rain
    if (timer_elapsed32(rain_timer) < STEP_MS) {
        return false;
    }
    rain_timer = timer_read32();
    render_rain();
    return false;
}
#endif // OLED_ENABLE
