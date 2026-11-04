#pragma once

#define QUICK_TAP_TERM 0
#define TAPPING_TERM 100

// Forward each typed character to the half that shows the rain.
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_USER_KEYCHARS

// Make WPM and the active layer available on BOTH halves, so the left info
// panel shows them whether it is the USB (master) half or not.
#define SPLIT_WPM_ENABLE
#define SPLIT_LAYER_STATE_ENABLE

// Keep both OLEDs awake.
#define OLED_TIMEOUT 0
