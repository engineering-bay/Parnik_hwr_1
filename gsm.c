#include <mega2560.h>
#include <parnik.h>
#include <delay.h>
#include <gsm.h>
#include <stdio.h>
#include <string.h>

extern char rx_buffer0[];
extern int rx_index0;

void GSM_PowerOn(void)
{
    // Turn ON GSM module
    PORTH |= BIT3;      // nReset pin
    PORTH |= BIT6;
    delay_ms(1000);
    PORTH &= ~BIT6;
}

void GSM_SendATCommand(char *command)
{
    int i;
    USART0_flush();
    for(i = 0; i < strlen(command); i++)
    {
        USART0_Transmit(command[i]);
    }
}

int GSM_ReadAnswer(char *str,unsigned char n)
{
    int old_index=0;
    long int t=0;
    
    rx_index0 = 0;
    
    while((rx_index0 < n) || (t < GSM_RXBYTE_TIMEOUT))
    {
       delay_us(10);
       t += 10;
       if(rx_index0 > old_index)
       {
         t = 0;
         old_index = rx_index0;
       }
    }
    strncpy(str, rx_buffer0+2, n-2);
    return(rx_index0);
}

int GSM_GetTime(char *str)
{
    int i;
    char tmp[40];
    GSM_SendATCommand("AT+CCLK?\r\n");
    i = GSM_ReadAnswer(tmp, 39);
    if(i < 39) return -1;
    else
    {
        strncpy(str, tmp+17, 11);
        return 0;
    }
}