#include "delay.h"
#include "main.h"

void delay100us(void)
{
    u16 j = 65;
    do
    {
        j--;
    }
    while(j != 0);  
}

void delay(u16 ms)
{
    do
    {
        u8 u = 10;
        do
        {
            delay100us();
            u--;
        }
        while(u != 0);
        ms--;
    }
    while(ms != 0);
}

