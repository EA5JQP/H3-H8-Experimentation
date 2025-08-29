#include "keypad.h"
#include "uart.h"

// Flag byte to pack small flags (only FLSH for now)
__xdata volatile u8 key_flags;           // bit 0 = FLSH pressed
__xdata volatile u8 key_code;            // current key code
__xdata volatile u8 debounce_counter;
__xdata volatile u8 prev_key_code;
__xdata volatile u8 key_hold_counter;
__xdata volatile u8 last_number_key;     // tracking last number key with FLSH

// Flag helpers
#define FLAG_FLSH_PRESSED 0x01
#define SET_FLSH()        (key_flags |= FLAG_FLSH_PRESSED)
#define CLR_FLSH()        (key_flags &= ~FLAG_FLSH_PRESSED)
#define IS_FLSH()         (key_flags & FLAG_FLSH_PRESSED)

// Keyout setters (assuming KEYOUTx are bit-addressable)
#define SET_ROW1() do { KEYOUT1 = 0; KEYOUT2 = 1; KEYOUT3 = 1; KEYOUT4 = 1; } while(0)
#define SET_ROW2() do { KEYOUT1 = 1; KEYOUT2 = 0; KEYOUT3 = 1; KEYOUT4 = 1; } while(0)
#define SET_ROW3() do { KEYOUT1 = 1; KEYOUT2 = 1; KEYOUT3 = 0; KEYOUT4 = 1; } while(0)
#define SET_ROW4() do { KEYOUT1 = 1; KEYOUT2 = 1; KEYOUT3 = 1; KEYOUT4 = 0; } while(0)

// Macro for detecting multi-row press on a given input (two specific rows: row1 and row2)
#define DETECT_MULTI_ROW_INPUT(input, dest)              \
    do {                                                \
        u8 r1, r2;                                      \
        SET_ROW1(); delay_short(10);                    \
        r1 = !(input);                                  \
        SET_ROW2(); delay_short(10);                    \
        r2 = !(input);                                  \
        dest = (r1 && r2);                              \
    } while (0)

// Read keyin by index (1..5) helper if needed; here we use direct symbols

u8 keypad_scan(void) {
    // Work on local copy to reduce repeated __xdata accesses
    u8 local_key = 0;
    u8 current_number_key = 0;
    u8 flsh = 0;

    // Clear lower nibble of P1 (as original)
    P1 &= 0xF;
    u8 no_keys_pressed = (P3 & 0x1F) == 0x1F;

    if ((RXD232_PTT == 1) && no_keys_pressed) {
        if ((key_code == 0x00) || (debounce_counter < 5)) {
            debounce_counter = 0;
            key_code = 0;
            last_number_key = 0;
            CLR_FLSH();
        }
    } else {
        debounce_counter++;
        if (debounce_counter > 5) {
            debounce_counter = 0;
            prev_key_code = key_code;
            key_code = 0;        // clear early
            CLR_FLSH();          // reset flag
            current_number_key = 0;

            // Detect FLSH (multi-row on col1)
            DETECT_MULTI_ROW_INPUT(KEYIN1, flsh);
            if (flsh) {
                SET_FLSH();
            }

            // --- Check for SIDE keys first (before matrix scan) ---
            // SIDE1 detection (col2 multi-row)
            u8 side1 = 0;
            DETECT_MULTI_ROW_INPUT(KEYIN2, side1);
            if (side1) {
                local_key = KEY_SIDE1;
            }

            // SIDE2 detection (col3 multi-row)
            if (local_key == 0) {
                u8 side2 = 0;
                DETECT_MULTI_ROW_INPUT(KEYIN3, side2);
                if (side2) {
                    local_key = KEY_SIDE2;
                }
            }

            // --- Row 1 Scan (only if no side key detected) ---
            if (local_key == 0) {
                SET_ROW1();
                delay_short(10);

                if (RXD232_PTT != 0x01) {
                    local_key = KEY_PTTA;
                }
                else if (!KEYIN1 && !IS_FLSH()) {
                    local_key = KEY_MENU;
                }
                else if (!KEYIN2) {
                    current_number_key = KEY_1;
                    local_key = IS_FLSH() ? (0x30 + KEY_1) : KEY_1;
                }
                else if (!KEYIN3) {
                    current_number_key = KEY_4;
                    local_key = IS_FLSH() ? (0x30 + KEY_4) : KEY_4;
                }
                else if (!KEYIN4) {
                    current_number_key = KEY_7;
                    local_key = IS_FLSH() ? (0x30 + KEY_7) : KEY_7;
                }
                else if (!KEYIN5) {
                    local_key = KEY_VFO;
                }
            }

            // --- Row 2 Scan ---
            if (local_key == 0) {
                SET_ROW2();
                delay_short(10);

                if (!KEYIN1 && !IS_FLSH()) {
                    local_key = KEY_UP;
                }
                else if (!KEYIN2) {
                    current_number_key = KEY_2;
                    local_key = IS_FLSH() ? (0x30 + KEY_2) : KEY_2;
                }
                else if (!KEYIN3) {
                    current_number_key = KEY_5;
                    local_key = IS_FLSH() ? (0x30 + KEY_5) : KEY_5;
                }
                else if (!KEYIN4) {
                    current_number_key = KEY_8;
                    local_key = IS_FLSH() ? (0x30 + KEY_8) : KEY_8;
                }
                else if (!KEYIN5) {
                    local_key = KEY_BL;
                }
            }

            // --- Row 3 Scan ---
            if (local_key == 0) {
                SET_ROW3();
                delay_short(10);

                if (!KEYIN1 && !IS_FLSH()) {
                    local_key = KEY_DOWN;
                }
                else if (!KEYIN2) {
                    current_number_key = KEY_3;
                    local_key = IS_FLSH() ? (0x30 + KEY_3) : KEY_3;
                }
                else if (!KEYIN3) {
                    current_number_key = KEY_6;
                    local_key = IS_FLSH() ? (0x30 + KEY_6) : KEY_6;
                }
                else if (!KEYIN4) {
                    current_number_key = KEY_9;
                    local_key = IS_FLSH() ? (0x30 + KEY_9) : KEY_9;
                }
                else if (!KEYIN5) {
                    local_key = KEY_AB;
                }
            }

            // --- Row 4 Scan ---
            if (local_key == 0) {
                SET_ROW4();
                delay_short(10);

                if (!KEYIN1 && !IS_FLSH()) {
                    local_key = KEY_EXIT;
                }
                else if (!KEYIN2) {
                    current_number_key = KEY_STAR;
                    local_key = IS_FLSH() ? (0x30 + KEY_STAR) : KEY_STAR;
                }
                else if (!KEYIN3) {
                    current_number_key = KEY_0;
                    local_key = IS_FLSH() ? (0x30 + KEY_0) : KEY_0;
                }
                else if (!KEYIN4) {
                    current_number_key = KEY_HASH;
                    local_key = IS_FLSH() ? (0x30 + KEY_HASH) : KEY_HASH;
                }
            }

            // --- Handle FLSH / number combination state transitions ---
            if (IS_FLSH() && current_number_key != 0) {
                // FLSH + number pressed
                if (current_number_key != last_number_key) {
                    last_number_key = current_number_key;
                    // key already in local_key
                }
            }
            else if (IS_FLSH() && current_number_key == 0 && last_number_key != 0) {
                // number released, FLSH still held
                local_key = KEY_FLSH;
                last_number_key = 0;
            }
            else if (!IS_FLSH() && last_number_key != 0) {
                // FLSH released
                if (current_number_key != 0) {
                    local_key = current_number_key;
                } else {
                    local_key = 0;
                }
                last_number_key = 0;
            }

            // --- Fallback check for FLSH alone when still no key ---
            if (local_key == 0 && IS_FLSH()) {
                local_key = KEY_FLSH;
            }

            // Commit local_key to the volatile storage
            key_code = local_key;
        }
    }

    return key_code;
}
