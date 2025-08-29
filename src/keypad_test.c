#include "keypad_test.h"
#include "uart_test.h"

// Test the keypad_scan() function
void test_keypad_scan(void) {
    send_uart_message("Testing keypad_scan()...");
    send_uart_message("Press any key - test runs continuously");
    
    u8 last_key = 0;
    
    while (1) {
        u8 current_key = keypad_scan();
        
        if (current_key != last_key) {
            if (current_key != 0) {
                // Key pressed
                send_uart_message("Key detected:");
                
                // Send key name
                switch(current_key) {
                    case KEY_MENU: send_uart_message("MENU"); break;
                    case KEY_UP: send_uart_message("UP"); break;
                    case KEY_DOWN: send_uart_message("DOWN"); break;
                    case KEY_EXIT: send_uart_message("EXIT"); break;
                    case KEY_PTTA: send_uart_message("PTT"); break;
                    case KEY_VFO: send_uart_message("VFO"); break;
                    case KEY_BL: send_uart_message("BL"); break;
                    case KEY_AB: send_uart_message("AB"); break;
                    case KEY_1: send_uart_message("1"); break;
                    case KEY_2: send_uart_message("2"); break;
                    case KEY_3: send_uart_message("3"); break;
                    case KEY_4: send_uart_message("4"); break;
                    case KEY_5: send_uart_message("5"); break;
                    case KEY_6: send_uart_message("6"); break;
                    case KEY_7: send_uart_message("7"); break;
                    case KEY_8: send_uart_message("8"); break;
                    case KEY_9: send_uart_message("9"); break;
                    case KEY_0: send_uart_message("0"); break;
                    case KEY_STAR: send_uart_message("*"); break;
                    case KEY_HASH: send_uart_message("#"); break;
                    case KEY_SIDE1: send_uart_message("SIDE1"); break;
                    case KEY_SIDE2: send_uart_message("SIDE2"); break;
                    case KEY_FLSH: send_uart_message("FLSH"); break;
                    case KEY_FLSH_PLUS_1: send_uart_message("FLSH+1"); break;
                    case KEY_FLSH_PLUS_2: send_uart_message("FLSH+2"); break;
                    case KEY_FLSH_PLUS_3: send_uart_message("FLSH+3"); break;
                    case KEY_FLSH_PLUS_4: send_uart_message("FLSH+4"); break;
                    case KEY_FLSH_PLUS_5: send_uart_message("FLSH+5"); break;
                    case KEY_FLSH_PLUS_6: send_uart_message("FLSH+6"); break;
                    case KEY_FLSH_PLUS_7: send_uart_message("FLSH+7"); break;
                    case KEY_FLSH_PLUS_8: send_uart_message("FLSH+8"); break;
                    case KEY_FLSH_PLUS_9: send_uart_message("FLSH+9"); break;
                    default: 
                        send_uart_message("Unknown key code:");
                        uart_pr_send_byte('0' + (current_key / 100));
                        uart_pr_send_byte('0' + ((current_key / 10) % 10));
                        uart_pr_send_byte('0' + (current_key % 10));
                        uart_pr_send_byte('\r');
                        uart_pr_send_byte('\n');
                        break;
                }
                
                // Send hex code
                uart_pr_send_byte('(');
                uart_pr_send_byte('0');
                uart_pr_send_byte('x');
                u8 high = (current_key >> 4) & 0x0F;
                u8 low = current_key & 0x0F;
                uart_pr_send_byte((high > 9) ? ('A' + high - 10) : ('0' + high));
                uart_pr_send_byte((low > 9) ? ('A' + low - 10) : ('0' + low));
                uart_pr_send_byte(')');
                uart_pr_send_byte('\r');
                uart_pr_send_byte('\n');
                
            } else if (current_key == 0 && last_key != 0) {
                // Key released
                send_uart_message("Key released");
            }
            
            last_key = current_key;
        }
        
        delay_ms(0, 20);  // 50ms delay between scans
    }
}

// Test all keypad functionality
void test_keypad_all(void) {
    send_uart_message("=== KEYPAD TEST SUITE ===");
    
    // Just run continuous keypad scanning
    test_keypad_scan();
}