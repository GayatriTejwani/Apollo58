#pragma once

#define QUICK_TAP_TERM 0
#define TAPPING_TERM 100

// Custom split transaction: master forwards each typed character to the half
// that has the OLED, so it rains the REAL keys (not random ones).
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_USER_KEYCHARS

// Keep the OLED awake.
#define OLED_TIMEOUT 0
