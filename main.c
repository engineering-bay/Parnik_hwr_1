#include <mega2560.h>
#include <delay.h>
#include <string.h>
// Alphanumeric LCD functions
#include <alcd.h>
#include <stdio.h>
#include <parnik.h>
#include <dht22.h>

extern void MCU_init(void);
extern void USART_Transmit( unsigned char data );
extern unsigned int read_adc(unsigned char adc_input);

// Declare your global variables here
float HumInt = -99.9, TempInt = -99.9, HumExt = -99.9, TempExt = -99.9;

void main(void)
{
int i, n, tmp1, tmp2;
//int IntSensorPortAdress = 0xC5, ExtSensorPortAdress = 0xC3;
unsigned int adc_res;
char str[50];

MCU_init();
delay_ms(2000);
n = 0;
while(1)
    {
        PORTB |= BIT7;
        /*for(i = 0; i < 8; i++)
        {
             RelayCmd(i+1, RELAY_ON);
             delay_ms(500);
             RelayCmd(i+1, RELAY_OFF);
             delay_ms(500);
        }*/
        adc_res = read_adc(0);
        tmp1 = DHT22_GetTempHum(&HumInt, &TempInt, INT_SENSOR_ADDR);
        tmp2 = DHT22_GetTempHum(&HumExt, &TempExt, EXT_SENSOR_ADDR);
        sprintf(str, "[%2d] R[%2d,%2d] H1[%5.1f] T1[%5.1f] H2[%5.1f] T2[%5.1f]\n\r", n, tmp1, tmp2, HumInt, TempInt, HumExt, TempExt);
        for(i = 0; i < strlen(str); i++)
        {    
            USART_Transmit(str[i]);    
        }
        n++;
        if(n > 99) n = 0;
        PORTB &= ~BIT7;
        delay_ms(2000);        
    }
}

int RelayCmd(unsigned char RNum, unsigned char RCmd)
{
    switch(RNum)
    {
        case 1:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT0;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT0;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 2:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT1;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT1;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 3:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT2;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT2;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 4:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT3;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT3;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 5:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT4;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT4;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 6:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT5;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT5;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 7:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT6;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT6;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        case 8:
        {
            if(RCmd == RELAY_OFF)
            {
                PORTA |= BIT7;
            }
            else if(RCmd == RELAY_ON)
            {
                PORTA &= ~BIT7;
            }
            else
            {
                // Wrong command!
                return -2;
            }
        };break;
        default:
        {
            // Wrong relay number
            return -1;
        };
    }
    return 0;
}