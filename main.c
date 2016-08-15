#include <mega2560.h>
#include <delay.h>
#include <string.h>
#include <math.h>
// Alphanumeric LCD functions
#include <alcd.h>
#include <stdio.h>
#include <parnik.h>
#include <dht22.h>

extern void MCU_init(void);
extern void USART0_Transmit( unsigned char data );
extern void USART1_Transmit( unsigned char data );
extern unsigned int read_adc(unsigned char adc_input);

// Declare your global variables here
float TimeStamp = 3600.0;
int TimerSyncFlag = 0, VentState = 0;
long int VentLastTS = 0, WateringLastTS = 0;
float WaterTankDiameterSM = 50.0, WaterTankHeightEmptySM = 100.0;
float TempIntThrLow = 22.0, TempIntThrHigh = 35.0, SoilHumThrLow = 40.0;
float HumInt = 0.0, TempInt = -99.9, HumExt = 0.0, TempExt = -99.9, SoilHum = 0.0, WaterVolume = 0.0;
// Cicle buffers
float Water[WINDOW_SIZE];
float Soil[WINDOW_SIZE];

void main(void)
{
int i, t, n, tmp1, tmp2, key;
//int IntSensorPortAdress = 0xC5, ExtSensorPortAdress = 0xC3;
unsigned int adc_res;
float tw, ts;
float WaterTankHeightSM = WaterTankHeightEmptySM;
char str1[20], str2[20];
unsigned char str485[60];

MCU_init();
delay_ms(100);
#asm("sei")

PORTE |= BIT3;
RelayCmd(8, RELAY_ON);
delay_ms(1000);
RelayCmd(8, RELAY_OFF);
//PORTE &= ~BIT3;
//PORTK |= (BIT4|BIT5|BIT6);

lcd_clear();

n = 0;
while(1)
    {
        PORTB |= BIT7;
        
        tmp1 = DHT22_GetTempHum(&HumInt, &TempInt, INT_SENSOR_ADDR);
        tmp2 = DHT22_GetTempHum(&HumExt, &TempExt, EXT_SENSOR_ADDR);
        ts = 0;
        for(t = 0; t < WINDOW_SIZE; t++)
        {
            adc_res = read_adc(0);    
            if(adc_res < SOILSENSOR_HIGH) adc_res = SOILSENSOR_HIGH;
            Soil[t] = (1 - (float)(adc_res - SOILSENSOR_HIGH)/SOILSENSOR_DELTA)*100.0;
            ts += Soil[t];    
        }
        SoilHum = ts/WINDOW_SIZE;
        tw = 0;
        for(t = 0; t < WINDOW_SIZE; t++)
        {
            Water[t] = GetUltrasonicDistance();
            tw += Water[t];    
        }
        WaterTankHeightSM = tw/WINDOW_SIZE;
        WaterVolume = (WaterTankHeightEmptySM - WaterTankHeightSM)*PI*pow(WaterTankDiameterSM, 2)/4000;
            
        switch(n)
        {
            case LCD_STATE_COMMON:
            {
                sprintf(str1, "  Parnik  2016");
                TimestampToTime((long int) TimeStamp, str2);
                //sprintf(str2, "%.2f", TimeStamp);
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
            case LCD_STATE_TH:
            {
                sprintf(str1, "T1:%5.1f H1:%4.1f", TempInt, HumInt);
                sprintf(str2, "T2:%5.1f H2:%4.1f", TempExt, HumExt);
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
            case LCD_STATE_SW:
            {
                sprintf(str1, "HS: %4.1f%%", SoilHum);
                sprintf(str2, "Wt: %4.0fL", WaterVolume);
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
        }
        sprintf(str485, "Ti[%5.1f],Hi[%4.1f],Te[%5.1f],He[%4.1f]\r\n", TempInt, HumInt, TempExt, HumExt);
        SendToRS485(str485, strlen(str485));
        n++;
        if(n > LCD_STATE_NUM) n = 0;
        LogicStep();
        PORTB &= ~BIT7;
        //delay_ms(2000);
        for(i = 0; i < 10; i++)
        {
            key = KeyPressed();
            if(key != -1)
            { 
                if(key == 11) PORTE ^= BIT3;
                if(key == 1)
                {
                    // Принудительный полив
                    WateringLastTS = (long int)TimeStamp;
                    sprintf(str1, "WATERING:");
                    sprintf(str2, "[S]: PROCESS");
                    lcd_clear();
                    lcd_gotoxy(4, 0);
                    lcd_puts(str1);
                    lcd_gotoxy(0, 1);
                    lcd_puts(str2);
                    RelayCmd(1, RELAY_ON);
                    delay_ms(15000);
                    RelayCmd(1, RELAY_OFF);
                }
                if(key == 2)
                {
                    // Принудительное проветривание
                    VentLastTS = (long int)TimeStamp;
                    sprintf(str1, "VENTILATION:");
                    sprintf(str2, "[S]: OPENING");
                    lcd_clear();
                    lcd_gotoxy(2, 0);
                    lcd_puts(str1);
                    lcd_gotoxy(0, 1);
                    lcd_puts(str2);
                    // Открываем форточку №1
                    RelayCmd(4, RELAY_ON);
                    delay_ms(15000);
                    RelayCmd(4, RELAY_OFF);
                    // Открываем форточку №2
                    RelayCmd(5, RELAY_ON);
                    delay_ms(15000);
                    RelayCmd(5, RELAY_OFF);
                    VentState = 1;
                }
                if(key == 3)
                {
                    // Принудительно закрыть форточки
                    sprintf(str1, "VENTILATION:");
                    sprintf(str2, "[S]: CLOSING");
                    lcd_clear();
                    lcd_gotoxy(2, 0);
                    lcd_puts(str1);
                    lcd_gotoxy(0, 1);
                    lcd_puts(str2);
                    // Закрываем форточку №1
                    RelayCmd(4, RELAY_ON);
                    RelayCmd(3, RELAY_ON);
                    RelayCmd(2, RELAY_ON);
                    delay_ms(15000);
                    RelayCmd(4, RELAY_OFF);
                    RelayCmd(3, RELAY_OFF);
                    RelayCmd(2, RELAY_OFF);
                    // Закрываем форточку №2
                    RelayCmd(5, RELAY_ON);
                    RelayCmd(6, RELAY_ON);
                    RelayCmd(7, RELAY_ON);
                    delay_ms(15000);
                    RelayCmd(5, RELAY_OFF);
                    RelayCmd(6, RELAY_OFF);
                    RelayCmd(7, RELAY_OFF);
                    VentState = 0;
                }
            }
            delay_ms(200);
        }       
    }
}

void LogicStep(void)
{
    char str1[16], str2[16];
    if(SoilHum < SoilHumThrLow)
    {
        // Необходим полив
        if(WaterVolume <= 20)
        {
            // ALARM - Нет воды в бочке!
            sprintf(str1, "WATERING:");
            sprintf(str2, "[S]: NO WATER!");
            lcd_clear();
            lcd_gotoxy(4, 0);
            lcd_puts(str1);
            lcd_gotoxy(0, 1);
            lcd_puts(str2);
        }
        else
        {
            // Включаем полив
            if(TimeStamp - WateringLastTS > WATERING_COOLDOWN)
            {
                WateringLastTS = (long int)TimeStamp;
                sprintf(str1, "WATERING:");
                sprintf(str2, "[S]: PROCESS");
                lcd_clear();
                lcd_gotoxy(4, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
                RelayCmd(1, RELAY_ON);
                delay_ms(15000);
                RelayCmd(1, RELAY_OFF);
            }
        }    
    }
    if(TempInt > TempIntThrHigh)
    {
        // Открыть форточки
        if(TimeStamp - VentLastTS > VENT_COOLDOWN && VentState == 0)
        {
            VentLastTS = (long int)TimeStamp;
            sprintf(str1, "VENTILATION:");
            sprintf(str2, "[S]: OPENING");
            lcd_clear();
            lcd_gotoxy(2, 0);
            lcd_puts(str1);
            lcd_gotoxy(0, 1);
            lcd_puts(str2);
            // Открываем форточку №1
            RelayCmd(4, RELAY_ON);
            delay_ms(15000);
            RelayCmd(4, RELAY_OFF);
            // Открываем форточку №2
            RelayCmd(5, RELAY_ON);
            delay_ms(15000);
            RelayCmd(5, RELAY_OFF);
            VentState = 1;
        }
    }
    if(TempInt < TempIntThrLow)
    {    
        // Закрыть форточки
        if(VentState == 1)
        {    
            sprintf(str1, "VENTILATION:");
            sprintf(str2, "[S]: CLOSING");
            lcd_clear();
            lcd_gotoxy(2, 0);
            lcd_puts(str1);
            lcd_gotoxy(0, 1);
            lcd_puts(str2);
            // Закрываем форточку №1
            RelayCmd(4, RELAY_ON);
            RelayCmd(3, RELAY_ON);
            RelayCmd(2, RELAY_ON);
            delay_ms(15000);
            RelayCmd(4, RELAY_OFF);
            RelayCmd(3, RELAY_OFF);
            RelayCmd(2, RELAY_OFF);
            // Закрываем форточку №2
            RelayCmd(5, RELAY_ON);
            RelayCmd(6, RELAY_ON);
            RelayCmd(7, RELAY_ON);
            delay_ms(15000);
            RelayCmd(5, RELAY_OFF);
            RelayCmd(6, RELAY_OFF);
            RelayCmd(7, RELAY_OFF);
            VentState = 0;
        }
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

float GetUltrasonicDistance(void)
{
    float res;
    long int timerval = 0;
    // Send 'Trigger'
    PORTD.7 = 1;
    delay_us(10);
    PORTD.7 = 0;
    // Wait for 'Echo' rising edge
    while(PING.1 != 1)
    {
        delay_us(10);
        timerval += ULTRASONIC_DELAYSTEP;
        if(timerval >= ULTRASONIC_DELAYTIMEOUT)
        {
            // Timeout
            return -1;
        }
    }
    // Wait for 'Echo' falling edge
    timerval= 0;
    while(PING.1 != 0)
    {
        delay_us(10);
        timerval += ULTRASONIC_DELAYSTEP;
        if(timerval >= ULTRASONIC_DELAYTIMEOUT)
        {
            // Timeout
            return -2;
        }
    }
    res = (float)ULTRASONIC_SOUNDVELOCITY*(float)timerval/2;
    return res;
}

//#pragma used+
void SendToRS485(char *ptr, int len)
{
    int i;
    PORTD.0 = 1;
    //delay_us(10);
    for(i = 0; i < len; i++)
    {
        //USART0_Transmit(ptr[i]);
        USART1_Transmit(ptr[i]);                                
    }
    delay_us(100);
    PORTD.0 = 0;
}
//#pragma used-

int KeyPressed(void)
{
    unsigned char pval;
    int res = -1;
    // 1-Col
    PORTK &= ~BIT4;
    pval = PINK;
    if((pval&BIT0) == 0) res = 1;
    if((pval&BIT1) == 0) res = 4;
    if((pval&BIT2) == 0) res = 7;
    if((pval&BIT3) == 0) res = 10;
    PORTK |= BIT4;
    PORTK &= ~BIT5;
    pval = PINK;
    if((pval&BIT0) == 0) res = 2;
    if((pval&BIT1) == 0) res = 5;
    if((pval&BIT2) == 0) res = 8;
    if((pval&BIT3) == 0) res = 0;
    PORTK |= BIT5;
    PORTK &= ~BIT6;
    pval = PINK;
    if((pval&BIT0) == 0) res = 3;
    if((pval&BIT1) == 0) res = 6;
    if((pval&BIT2) == 0) res = 9;
    if((pval&BIT3) == 0) res = 11;
    PORTK |= BIT6;    
    return (res);
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    // Reinitialize Timer 0 value
    TCNT0=0x64;
    // Place your code here
    TimeStamp += TIMERTICK_VAL_SEC;
    if(TimeStamp > TIMERTICK_MAX_DAY_TS)
    {
        TimeStamp = TimeStamp - TIMERTICK_MAX_DAY_TS;
        TimerSyncFlag = 0;
    }
}

int TimestampToTime(long int TS, char *ptrStr)
{
    long int hour, min, sec;
    if(TS > TIMERTICK_MAX_DAY_TS) return -1;
    else
    {
        hour = (TS/3600);
        min = (TS%3600)/60;
        sec = TS - hour*3600 - min*60;
        sprintf(ptrStr, "%02d:%02d:%02d", hour, min, sec);
    }
    return 0;
}