#include <mega2560.h>
#include <parnik.h>
#include <delay.h>
#include <gsm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alcd.h>

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

int GSM_ReadAnswer(char *str, unsigned char n, int to)
{
    int old_index=0;
    long int t=0;
    
    rx_index0 = 0;
    delay_ms(to);
    
    while((rx_index0 < n) && (t < GSM_RXBYTE_TIMEOUT))
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
    i = GSM_ReadAnswer(tmp, 39, 50);
    if(i < 39) return -1;
    else
    {
        strncpy(str, tmp+17, 11);
        return 0;
    }
}

int GSM_SetTime(int year, int mon, int day, int hour, int min, int sec)
{
    int i;
    char tmp[100], str1[16], str2[16];
    
    sprintf(tmp, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d+00\"\r\n", year, mon, day, hour, min, sec);
    GSM_SendATCommand(tmp);
    i = GSM_ReadAnswer(tmp, 50, 40);
    /*sprintf(str1, "set time: [%d]", i);
    strncpy(str2, tmp, 16);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts(str1);
    lcd_gotoxy(0, 1);
    lcd_puts(str2);
    delay_ms(2000);*/
    if(i < 39) return -1;
    else
    {
        //strncpy(str, tmp+17, 11);
        return 0;
    }
}

int GSM_SendSMS(char *num, char *text)
{
    int i;
    char tmp[100];

    sprintf(tmp, "AT+CMGS=\"%s\"\r\n", num);
    GSM_SendATCommand(tmp);
    delay_ms(100);
    GSM_SendATCommand(text);
    USART0_Transmit(26);
    i = GSM_ReadAnswer(tmp, 16, 100);
    if(i == 19) return 0;
    else return -1;
}

int GSM_GetBalance(float *balance)
{
    int i, sts, len;
    char *ptr;
    char tmp[100], str1[30], str2[30];
    
    sprintf(tmp, "AT+CUSD=1,\"*100#\"\r\n");
    GSM_SendATCommand(tmp);
    for(i = 0; i < 100; i++) tmp[i] = 0;
    i = GSM_ReadAnswer(tmp, 100, 2000);
    ptr = strstr(tmp, "0440");
    if(ptr != NULL)
    {   
        len = (int)ptr - (int)(tmp);
        strncpy(str1, tmp+16, len);
        for(i = 0; i < 30; i++) str2[i] = 0;
        sts = UCS2_to_String(str1, str2, len);
        if(sts > 0)
        {
            *balance = atof(str2);
            return 0;
        }
        else return len;
    }
    else
    {
        //strncpy(str2, tmp, 16);
        //lcd_clear();   
        //lcd_gotoxy(0,1);
        //lcd_puts(str2);
        return (i-90);
    }
}

int GSM_ReadSMS(int smsnum, char *dst)
{
    int i, p1, p2;
    char *ptr, *ptr2;
    char tmp[100], str1[30], str2[30];
    
    lcd_clear();
    lcd_gotoxy(0,0);
    lcd_puts("SMS Read:");
    
    sprintf(tmp, "AT+CMGR=%d,1\r\n", smsnum);
    GSM_SendATCommand(tmp);
    for(i = 0; i < 100; i++) tmp[i] = 0;
    i = GSM_ReadAnswer(dst, 50, 100);
    ptr = strstr(tmp, "+CMGR:");
    if(ptr != NULL)
    {
        ptr = strstr(tmp, ",");
        ptr2 = strstr(ptr+1, ",");
        strncpy(str2, ptr, 16);
        //ptr2 = strstr(tmp, "\r");
        //strncpy(str2, tmp+16, 16);
        //lcd_clear();
        //lcd_gotoxy(0,0);
        //lcd_puts(str1);
        lcd_gotoxy(0,1);
        lcd_puts(str2);
        //GSM_SendATCommand("AT+CMGD=1,4\r\n");
        return 0;
    }
    else
    {   
        sprintf(str2, "ERROR: [%d]", i);
        lcd_gotoxy(0,1);
        lcd_puts(str2);
        return -1;
    }
}

int UCS2_to_String(char *src, char *dst, int srclen)
{
    int i, f;
    char tmp[6];
    f = 0;
    for(i = 0; i < srclen; i += 4)
    {
        strncpy(tmp, src + i, 4);
        tmp[5] = 0;
        if(strncmp(tmp, "0030", 4) == 0) { dst[i/4] = '0'; f++; }
        if(strncmp(tmp, "0031", 4) == 0) { dst[i/4] = '1'; f++; }
        if(strncmp(tmp, "0032", 4) == 0) { dst[i/4] = '2'; f++; }
        if(strncmp(tmp, "0033", 4) == 0) { dst[i/4] = '3'; f++; }
        if(strncmp(tmp, "0034", 4) == 0) { dst[i/4] = '4'; f++; }
        if(strncmp(tmp, "0035", 4) == 0) { dst[i/4] = '5'; f++; }
        if(strncmp(tmp, "0036", 4) == 0) { dst[i/4] = '6'; f++; }
        if(strncmp(tmp, "0037", 4) == 0) { dst[i/4] = '7'; f++; }
        if(strncmp(tmp, "0038", 4) == 0) { dst[i/4] = '8'; f++; }
        if(strncmp(tmp, "0039", 4) == 0) { dst[i/4] = '9'; f++; }
        if(strncmp(tmp, "002E", 4) == 0) { dst[i/4] = '.'; f++; }
        if(strncmp(tmp, "002D", 4) == 0) { dst[i/4] = '-'; f++; }
    }
    return f;
}