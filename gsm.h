#define GSM_RXBYTE_TIMEOUT  30000
#define GSM_TIMEOUT_STEP    10


void GSM_PowerOn(void);
void GSM_SendATCommand(char *command);
int GSM_ReadAnswer(char *str,unsigned char n, int to);
int GSM_GetTime(char *str);
int GSM_SetTime(int year, int mon, int day, int hour, int min, int sec);
int GSM_SendSMS(char *num, char *text);
int GSM_GetBalance(float *balance);
int UCS2_to_String(char *src, char *dst, int srclen);
int GSM_ReadSMS(int smsnum, char *dst);
