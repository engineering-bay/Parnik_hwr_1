/* HW Config:
Arduino pin     MCU pin         LCD pin     Func
PWM5            PE3            --          LCD_BACKLIGHT
PWM6            PH3            14          D7
PWM7            PH4            13          D6
PWM8            PH5            12          D5
PWM10           PB4            11          D4
PWM11           PB5            6           E
PWM12           PB6            4           RS

PWM9            PH6            --          GSM_KEY

SCL             PD0            --          RS485_DIR

A8              PK0            --          Keypad-1Row
A9              PK1            --          Keypad-2Row
A10             PK2                        Keypad-3Row
A11             PK3                        Keypad-4Row
A12             PK4                        Keypad-1Col
A13             PK5                        Keypad-2Col
A14             PK6                        Keypad-3Col
*/


#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

#define LCD_STATE_NUM               2
#define LCD_STATE_COMMON            0
#define LCD_STATE_TH                1
#define LCD_STATE_SW                2

#define SOILSENSOR_LOW              1023
#define SOILSENSOR_HIGH             250
#define SOILSENSOR_DELTA            (SOILSENSOR_LOW - SOILSENSOR_HIGH)

#define ULTRASONIC_SOUNDVELOCITY    0.0343f
#define ULTRASONIC_DELAYSTEP        10
#define ULTRASONIC_DELAYTIMEOUT     10000

#define WINDOW_SIZE                 20

#define RELAY_ON    1
#define RELAY_OFF   0

#define TIMERTICK_VAL_SEC           0.009984f
#define TIMERTICK_MAX_DAY_TS        86400
#define WATERING_COOLDOWN           3600
#define VENT_COOLDOWN               3600


void LogicStep(void);
int RelayCmd(unsigned char RNum, unsigned char RCmd);
float GetUltrasonicDistance(void);
void SendToRS485(char *ptr, int len);
int KeyPressed(void);
int TimestampToTime(long int TS, char *ptrStr);