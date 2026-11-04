#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE
#    include <avr/io.h>
#    include <util/delay.h>
#endif

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

/* ADJUST  (LOWER + RAISE)  -- mouse keys live here
 *                                         ,-----------------------------------------.
 *                                         |      | Wh L | Wh D | Wh U | Wh R |      |
 *                                         |------+------+------+------+------+------|
 *                                         | Btn1 |Mse L |Mse D |Mse U |Mse R | Btn2 |
 *                                         |------+------+------+------+------+------|
 *                                         | Btn3 |      |      |      |      |      |
 */
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

/* ------------------------------------------------------------------ *
 *  Supply-voltage (Vcc) measurement on the ATmega32U4.
 *  Reads the internal 1.1V bandgap against AVcc and back-computes Vcc.
 *  Returns millivolts.  Blocks ~2ms, so we only call it once a second.
 *
 *  CALIBRATION: the 1.1V bandgap varies part-to-part (typ +/-10%). If the
 *  reading is off, measure real Vcc with a multimeter and adjust the
 *  constant below:  VREF_CONST = measured_mV * raw_adc_reading.
 *  Default 1125300 = 1.1V * 1023 * 1000.
 * ------------------------------------------------------------------ */
#define VREF_CONST 1125300UL

static uint16_t read_vcc_mv(void) {
    uint8_t admux_save  = ADMUX;
    uint8_t adcsrb_save = ADCSRB;

    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable, /128
    // REFS0 = AVcc reference; MUX[5:0] = 0b011110 -> internal 1.1V bandgap
    ADMUX  = (1 << REFS0) | (1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1);
    ADCSRB &= ~(1 << MUX5);

    _delay_ms(2);                       // let Vref settle
    ADCSRA |= (1 << ADSC);              // start conversion
    while (ADCSRA & (1 << ADSC)) { }    // wait
    uint16_t adc = ADC;

    ADMUX  = admux_save;                // restore
    ADCSRB = adcsrb_save;

    if (adc == 0) return 0;
    return (uint16_t)(VREF_CONST / adc);
}

/* ------------------------------------------------------------------ *
 *  WPM history: five bars.  Bars 0..3 are the average WPM of the last
 *  four completed minutes (oldest left). Bar 4 is the live WPM.
 * ------------------------------------------------------------------ */
#define WPM_SCALE_MAX 120   // WPM mapped to full bar height

static uint8_t  wpm_min[4] = {0, 0, 0, 0};
static uint32_t acc_sum    = 0;
static uint16_t acc_cnt    = 0;
static uint32_t min_timer  = 0;
static uint32_t vcc_timer  = 0;
static uint16_t vcc_mv     = 0;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;  // match stock: flip when on the off hand
    }
    return rotation;
}

static void draw_vbar(uint8_t x, uint8_t w, uint8_t top, uint8_t bottom, uint8_t wpm) {
    uint8_t usable = bottom - top;                 // pixels available
    uint16_t h = ((uint16_t)wpm * usable) / WPM_SCALE_MAX;
    if (h > usable) h = usable;
    // outline the slot base so empty bars still show a footprint
    for (uint8_t xi = x; xi < x + w; xi++) {
        oled_write_pixel(xi, bottom, true);
    }
    // filled column from the baseline up
    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xi = x; xi < x + w; xi++) {
            oled_write_pixel(xi, bottom - yy, true);
        }
    }
}

bool oled_task_user(void) {
    // --- sample WPM into the current-minute accumulator ---
    uint8_t w = get_current_wpm();
    acc_sum += w;
    acc_cnt++;

    if (timer_elapsed32(min_timer) >= 60000) {
        uint8_t avg = acc_cnt ? (uint8_t)(acc_sum / acc_cnt) : 0;
        wpm_min[0] = wpm_min[1];
        wpm_min[1] = wpm_min[2];
        wpm_min[2] = wpm_min[3];
        wpm_min[3] = avg;
        acc_sum = 0;
        acc_cnt = 0;
        min_timer = timer_read32();
    }

    // --- refresh voltage once a second ---
    if (vcc_mv == 0 || timer_elapsed32(vcc_timer) >= 1000) {
        vcc_mv = read_vcc_mv();
        vcc_timer = timer_read32();
    }

    // --- draw ---
    oled_clear();

    // graph area: y 0 (top) .. 22 (baseline). text row at y24..31 (row 3).
    const uint8_t baseline = 22;
    const uint8_t top      = 1;
    // 5 bars, width 20, gap 6, left margin 2 -> x = 2,28,54,80,106
    uint8_t bars[5] = { wpm_min[0], wpm_min[1], wpm_min[2], wpm_min[3], w };
    for (uint8_t i = 0; i < 5; i++) {
        draw_vbar(2 + i * 26, 20, top, baseline, bars[i]);
    }

    // bottom line: live WPM + supply voltage
    char line[22];
    uint8_t v_int  = vcc_mv / 1000;
    uint8_t v_frac = (vcc_mv % 1000) / 10;   // two decimals
    snprintf(line, sizeof(line), "%3uwpm   %u.%02uV", w, v_int, v_frac);
    oled_set_cursor(0, 3);
    oled_write(line, false);

    return false;
}
#endif // OLED_ENABLE
