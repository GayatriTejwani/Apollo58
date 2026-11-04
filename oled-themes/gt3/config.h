#pragma once

#define QUICK_TAP_TERM 0
#define TAPPING_TERM 100

// Forward each typed character to the half that shows the rain.
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_USER_KEYCHARS

// Make WPM, active layer, and host LED state (Caps Lock) available on both halves.
#define SPLIT_WPM_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE

// Keep both OLEDs awake.
#define OLED_TIMEOUT 0
