#include QMK_KEYBOARD_H

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

/* ================= Falling letters (Matrix rain) ================= */
#ifdef OLED_ENABLE

// Portrait canvas (rotated): 32 px wide x 128 px tall.
// Text grid with the default 6x8 font: 5 columns, 16 rows.
#define LR_COLS   5
#define LR_ROWS   16
#define MAX_DROPS 16
#define STEP_MS   90        // ms per downward step (fall speed)
#define WPM_SPAWN_THRESH 90  // lower = heavier rain for a given wpm (slave half)

typedef struct {
    uint8_t col;    // 0..LR_COLS-1
    int8_t  row;    // head position, grows downward toward the keycaps
    char    ch;     // the glyph that falls
    bool    active;
} drop_t;

static drop_t   drops[MAX_DROPS];
static uint32_t rain_timer = 0;
static uint16_t wpm_budget = 0;
static uint16_t rng        = 0xC0DE;

static uint16_t xorshift16(void) {
    rng ^= rng << 7;
    rng ^= rng >> 9;
    rng ^= rng << 8;
    return rng;
}

static char keycode_to_char(uint16_t kc) {
    if (kc >= KC_A && kc <= KC_Z) return 'A' + (kc - KC_A);
    if (kc >= KC_1 && kc <= KC_9) return '1' + (kc - KC_1);
    if (kc == KC_0)               return '0';
    // space / enter / mods / everything else: a random glyph so prose still rains
    return 'A' + (xorshift16() % 26);
}

static void spawn_letter(char ch) {
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) {
            drops[i].col    = xorshift16() % LR_COLS;
            drops[i].row    = 0;              // start at the USB-C end
            drops[i].ch     = ch;
            drops[i].active = true;
            return;
        }
    }
    // pool full: recycle a pseudo-random slot
    uint8_t v = xorshift16() % MAX_DROPS;
    drops[v].col = xorshift16() % LR_COLS;
    drops[v].row = 0;
    drops[v].ch  = ch;
}

// Runs on the master, which sees keys from BOTH halves. Covers the case where
// the OLED is on the master half: the actual typed letter falls.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        rng ^= timer_read();
        spawn_letter(keycode_to_char(keycode));
    }
    return true;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // Portrait so letters fall the long way, from the USB-C end to the keys.
    // If it comes out upside-down on your mount, change 270 -> 90.
    return OLED_ROTATION_270;
}

bool oled_task_user(void) {
    if (timer_elapsed32(rain_timer) < STEP_MS) {
        return false;
    }
    rain_timer = timer_read32();

    // If the OLED is on the NON-master half, process_record_user never ran here,
    // so drive the rain from the synced WPM with random glyphs.
    if (!is_keyboard_master()) {
        wpm_budget += get_current_wpm();
        uint8_t spawned = 0;
        while (wpm_budget >= WPM_SPAWN_THRESH && spawned < 2) {
            wpm_budget -= WPM_SPAWN_THRESH;
            spawn_letter('A' + (xorshift16() % 26));
            spawned++;
        }
    }

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
    return false;
}
#endif // OLED_ENABLE
