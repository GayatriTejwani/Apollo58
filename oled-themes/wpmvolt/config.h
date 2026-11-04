#pragma once

#define QUICK_TAP_TERM 0
#define TAPPING_TERM 100

// Push WPM from the master half to the other half so the OLED (whichever
// half it is physically on) always has a live value to graph.
#define SPLIT_WPM_ENABLE

// Keep the OLED awake; comment out / set a value (ms) if you want it to sleep.
#define OLED_TIMEOUT 0
