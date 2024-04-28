#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _SYM,
    _NUM,
    _NAV,
    _FUN,
};

// Aliases for readability
#define SYM      MO(_SYM)
#define NUM      MO(_NUM)
#define NAV      MO(_NAV)
#define FUN      MO(_FUN)

#define _T       KC_TRNS

enum {
    ESC_SPC, // Tap = Escape, Hold = Space
};

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(ESC_SPC):  // list all tap dance keycodes with tap-hold configurations
            action = &tap_dance_actions[TD_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

tap_dance_action_t tap_dance_actions[] = {
    [ESC_SPC] = ACTION_TAP_DANCE_TAP_HOLD(KC_ESC, KC_SPC),
};


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Ctrl  |   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |  Enter |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |  Fun | Nav  |  | Fun  |  Sym |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | LCtl | LAlt | LGui |  Sym | Num  |  | Nav  |Space/| Esc/ | RAlt | RCtl |
 *                        |      |      |      |      |      |  |      | Num  | Space|      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_QWERTY] = LAYOUT(
    KC_TAB  , KC_Q ,  KC_W ,  KC_E ,  KC_R  ,   KC_T ,                                 KC_Y       ,   KC_U , KC_I   , KC_O   , KC_P   , KC_BSPC ,
    KC_LCTL , KC_A ,  KC_S ,  KC_D ,  KC_F  ,   KC_G ,                                 KC_H       ,   KC_J , KC_K   , KC_L   , KC_SCLN, KC_ENT  ,
    KC_LSFT , KC_Z ,  KC_X ,  KC_C ,  KC_V  ,   KC_B , FUN, NAV, FUN, SYM            , KC_N       ,   KC_M , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT ,
                            KC_LCTL, KC_LALT, KC_LGUI, SYM, NUM, NAV, LT(_NUM,KC_SPC), TD(ESC_SPC), KC_RALT, KC_RCTL
),

/*
 * Sym Layer: Symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    ~   |  `   |  &   |  \   |  #   |  !   |                              |   #  |  $   |  %   |  ^   |  &   |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  @   |  {   |  |   |  }   |  ?   |                              |   '  |  -   |  =   |  :   |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |    [   |      |  <   |      |  >   |      |      |      |  |      |      |   "  |  _   |  +   |  *   |      |   ]    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_SYM] = LAYOUT(
    KC_TILD        , KC_GRV , KC_AMPR, KC_BSLS, KC_HASH, KC_EXLM,                 KC_HASH, KC_DLR , KC_PERC, KC_CIRC, KC_AMPR, _T,
    _T             , KC_AT  , KC_LCBR, KC_PIPE, KC_RCBR, KC_QUES,                 KC_QUOT, KC_MINS, KC_EQL , KC_COLN, _T     , _T,
    LSFT_T(KC_LBRC), _T     , KC_LT  , _T     , KC_GT  , _T     , _T, _T, _T, _T, KC_DQUO, KC_UNDS, KC_PLUS, KC_ASTR, _T     , RSFT_T(KC_RBRC),
                                       _T     , _T     , _T     , _T, _T, _T, _T, _T     , _T     , _T
),


/*
 * Num Layer: Numbers
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  LGui  | LGui | Ctl S|  F3  | Ctl  |  F6  |                              |      |      |      |      |  0   |        |
 * |   `    |  1   | Tab  |      | Tab  |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  1   |  2   |  3   |  4   |  5   |                              |  6   |  7   |   8  |   9  |  0   |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LSGui/ |  6   |  7   |  8   |  9   |  0   |      |      |  |      |      |      |      |      |      |      | RShift/|
 * |   [    |      |      |      |      |      |                              |      |      |      |      |      |   ]    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_NUM] = LAYOUT(
    LGUI(KC_GRV), LGUI(KC_1), LCTL(LSFT(KC_TAB)), KC_F3, LCTL(KC_TAB), KC_F6,                 KC_NO, KC_NO, KC_NO, KC_NO, KC_0 , _T             ,
    _T          , KC_1      , KC_2              , KC_3 , KC_4        , KC_5 ,                 KC_6 , KC_7 , KC_8 , KC_9 , KC_0 , _T             ,
    SGUI_T(KC_LBRC), KC_6   , KC_7              , KC_8 , KC_9        , KC_0 , _T, _T, _T, _T, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RSFT_T(KC_RBRC),
                                                  _T   , _T          , _T   , _T, _T, _T, _T, _T   , _T   , _T
),


/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      | Mute | VolU | BrUp |                              | LGui | Ctl S|  F3  | Ctl  | LGui | Sleep  |
 * |        |      |      |      |      |      |                              |   C  | Tab  |      | Tab  |   V  |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |  F6  | VolD | BrDn |                              |  ←   |   ↓  |   ↑  |   →  |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |      |      |      |      |      |      |      |  |      |      |      | PgDn | PgUp |      |      | RShift |
 * |   [    |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |    ]   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_NAV] = LAYOUT(
    KC_NO          , KC_NO, KC_NO, KC_MUTE, KC_VOLU, KC_BRIU,                 LGUI(KC_C), RCTL(RSFT(KC_TAB)), KC_F3, RCTL(KC_TAB), LGUI(KC_V), KC_SLEP,
    KC_NO          , KC_NO, KC_NO, KC_F6  , KC_VOLD, KC_BRID,                 KC_LEFT   , KC_DOWN, KC_UP           , KC_RGHT, KC_NO, KC_NO,
    LSFT_T(KC_LBRC), KC_NO, KC_NO, KC_NO  , KC_NO  , KC_NO  , _T, _T, _T, _T, KC_NO     , KC_PGDN, KC_PGUP         , KC_NO, KC_NO, RSFT_T(KC_RBRC),
                                   _T     , _T     , _T     , _T, _T, _T, _T, _T        , _T     , _T
),

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |  F1  |  F2  |  F3  |  F4  |  F5  |                              |  F6  |  F7  |  F8  |  F9  |  F10 | PgUp   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F11 |  F12 |  F13 |  F14 |  F15 |                              |  F16 |  F17 |  F18 |  F19 |  F20 | PgDn   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_FUN] = LAYOUT(
    KC_ESC, KC_F1 , KC_F2 , KC_F3 , KC_F4 , KC_F5 ,                 KC_F6 , KC_F7 , KC_F8 , KC_F9 , KC_F10, KC_PGUP,
    KC_NO , KC_F11, KC_F12, KC_F13, KC_F14, KC_F15,                 KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, KC_PGDN,
    KC_NO , KC_NO , KC_NO , KC_NO , KC_NO , KC_NO , _T, _T, _T, _T, KC_NO , KC_NO , KC_NO , KC_NO , KC_NO , KC_NO  ,
                            _T    , _T    , _T    , _T, _T, _T, _T, _T    , _T    , _T
),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};


