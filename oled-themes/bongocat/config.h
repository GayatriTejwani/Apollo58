#pragma once

#define QUICK_TAP_TERM 0
#define TAPPING_TERM 100

// Share WPM from the master half so the cat animates no matter which half
// the OLED is physically on.
#define SPLIT_WPM_ENABLE

// How long (ms) the idle cat keeps animating after you stop typing before the
// OLED sleeps. The bongocat renderer uses this value directly.
#define OLED_TIMEOUT 60000
