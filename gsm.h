#define GSM_RXBYTE_TIMEOUT  100000
#define GSM_TIMEOUT_STEP    10


void GSM_PowerOn(void);
void GSM_SendATCommand(char *command);
int GSM_ReadAnswer(char *str,unsigned char n);
int GSM_GetTime(char *str);