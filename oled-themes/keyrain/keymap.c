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

/* ================= Typing rain ================= */
#ifdef OLED_ENABLE

#define RAIN_W      128
#define RAIN_H      32
#define MAX_DROPS   28
#define TRAIL       3        // pixels of streak behind the head
#define FRAME_MS    40       // ~25 fps
#define WPM_SPAWN_THRESH 80  // lower = heavier rain for a given wpm (slave half)

typedef struct {
    uint8_t x;
    int8_t  y;       // head position; starts at 0 and falls
    uint8_t speed;   // pixels per frame
    bool    active;
} drop_t;

static drop_t   drops[MAX_DROPS];
static uint32_t rain_timer   = 0;
static uint16_t wpm_budget   = 0;
static uint16_t rng          = 0xACE1;

static uint16_t xorshift16(void) {
    rng ^= rng << 7;
    rng ^= rng >> 9;
    rng ^= rng << 8;
    return rng;
}

static void spawn_drop(uint8_t x) {
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) {
            drops[i].x      = x % RAIN_W;
            drops[i].y      = 0;
            drops[i].speed  = 1 + (xorshift16() % 3);   // 1..3 px/frame
            drops[i].active = true;
            return;
        }
    }
    // pool full: recycle a pseudo-random slot so heavy typing still registers
    uint8_t v = xorshift16() % MAX_DROPS;
    drops[v].x     = x % RAIN_W;
    drops[v].y     = 0;
    drops[v].speed = 1 + (xorshift16() % 3);
}

// Per-keypress rain. process_record_user runs on the master and sees keys from
// BOTH halves, so this covers the case where the OLED is on the master half.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        rng ^= timer_read();   // stir entropy from timing
        uint8_t x = (uint8_t)((keycode * 41u + xorshift16()) & 0x7F);
        spawn_drop(x);
    }
    return true;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;  // match stock off-hand flip
    }
    return rotation;
}

bool oled_task_user(void) {
    if (timer_elapsed32(rain_timer) < FRAME_MS) {
        return false;  // hold the frame rate steady
    }
    rain_timer = timer_read32();

    // If the OLED is on the NON-master half, process_record_user never ran here,
    // so drive the rain from the synced WPM instead.
    if (!is_keyboard_master()) {
        wpm_budget += get_current_wpm();
        uint8_t spawned = 0;
        while (wpm_budget >= WPM_SPAWN_THRESH && spawned < 2) {
            wpm_budget -= WPM_SPAWN_THRESH;
            spawn_drop((uint8_t)(xorshift16() & 0x7F));
            spawned++;
        }
    }

    // advance + draw
    oled_clear();
    for (uint8_t i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) continue;
        drops[i].y += drops[i].speed;
        if (drops[i].y - TRAIL > RAIN_H) {
            drops[i].active = false;
            continue;
        }
        for (uint8_t t = 0; t <= TRAIL; t++) {
            int16_t yy = drops[i].y - t;
            if (yy >= 0 && yy < RAIN_H) {
                oled_write_pixel(drops[i].x, (uint8_t)yy, true);
            }
        }
    }
    return false;
}
#endif // OLED_ENABLE
