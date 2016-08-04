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
extern void USART_Transmit( unsigned char data );
extern unsigned int read_adc(unsigned char adc_input);

// Declare your global variables here
float WaterTankDiameterSM = 50.0, WaterTankHeightEmptySM = 100.0;
float TempIntThrLow = 20.0, TempIntThrHigh = 60.0, TempExtThrLow = 10.0;
float HumInt = 0.0, TempInt = -99.9, HumExt = 0.0, TempExt = -99.9, SoilHum = 0.0, WaterVolume = 0.0;
// Cicle buffers
float Water[WINDOW_SIZE];
float Soil[WINDOW_SIZE];

void main(void)
{
int i, t, n, tmp1, tmp2;
//int IntSensorPortAdress = 0xC5, ExtSensorPortAdress = 0xC3;
unsigned int adc_res;
float tw, ts;
float WaterTankHeightSM = WaterTankHeightEmptySM;
char str1[20], str2[20];

MCU_init();
delay_ms(100);

RelayCmd(8, RELAY_ON);
delay_ms(1000);
RelayCmd(8, RELAY_OFF);

PORTE.3 = 1;
lcd_clear();
//lcd_puts("  Parnik  2016");
delay_ms(1000);
//lcd_gotoxy(0, 0);
//lcd_putchar('C');

n = 0;
i = 0;
while(1)
    {
        PORTB |= BIT7;
        adc_res = read_adc(0);    
        if(adc_res < SOILSENSOR_HIGH) adc_res = SOILSENSOR_HIGH;
        //SoilHum = (1 - (float)(adc_res - SOILSENSOR_HIGH)/SOILSENSOR_DELTA)*100.0;
        Soil[i] = (1 - (float)(adc_res - SOILSENSOR_HIGH)/SOILSENSOR_DELTA)*100.0;
        Water[i] = GetUltrasonicDistance();
        //if(WaterTankHeightSM < 0) WaterTankHeightSM = 0;
        if(i == 19)
        {
            tmp1 = DHT22_GetTempHum(&HumInt, &TempInt, INT_SENSOR_ADDR);
            tmp2 = DHT22_GetTempHum(&HumExt, &TempExt, EXT_SENSOR_ADDR);
            ts = 0;
            for(t = 0; t < WINDOW_SIZE; t++)
            {
                ts += Soil[i];    
            }
            SoilHum = ts/WINDOW_SIZE;
            tw = 0;
            for(t = 0; t < WINDOW_SIZE; t++)
            {
                tw += Water[i];    
            }
            WaterTankHeightSM = tw/WINDOW_SIZE;
            WaterVolume = (WaterTankHeightEmptySM - WaterTankHeightSM)*PI*pow(WaterTankDiameterSM, 2)/4000;
            
            switch(n)
            {
                case LCD_STATE_COMMON:
                {
                    sprintf(str1, "  Parnik  2016");
                    sprintf(str2, "%02d:%02d", 99, 99);
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
                    sprintf(str2, "Wt: %4.0fL", WaterTankHeightSM);
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_puts(str1);
                    lcd_gotoxy(0, 1);
                    lcd_puts(str2);
                }break;
            }
            n++;
            if(n > LCD_STATE_NUM) n = 0;
            LogicStep();
        }
        i++;
        if(i >= WINDOW_SIZE) i = 0;
        PORTB &= ~BIT7;
        delay_ms(100);       
    }
}

void LogicStep(void)
{
    if(TempInt > TempIntThrHigh)
    {
        // Открыть форточки
         /*RelayCmd(4, RELAY_ON);
         delay_ms(15000);
         RelayCmd(4, RELAY_OFF);*/
    }
    if(TempInt < TempIntThrLow)
    {    
        // Закрыть форточки
         /*RelayCmd(4, RELAY_ON);
         delay_ms(15000);
         RelayCmd(4, RELAY_OFF);*/
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