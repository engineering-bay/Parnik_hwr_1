#define _DTH22

#define INT_SENSOR_ADDR             0xC5
#define EXT_SENSOR_ADDR             0xC3

#define WAIT_STEP   5
#define DHT22_TIMING_TOLERANCE      (WAIT_STEP*2)
#define DHT22_TIMING_RESPONSE_SHORT 20
#define DHT22_TIMING_RESPONSE_LOW   80
#define DHT22_TIMING_RESPONSE_HIGH  80
#define DHT22_TIMING_DATA_LOW       50
#define DHT22_TIMING_DATA0_HIGH     26
#define DHT22_TIMING_DATA1_HIGH     70

int DHT22_GetTempHum(float *ptrHum, float *ptrTemp, int ptrAPort);
int DHT22_ReadData(unsigned char *ptr, int ptrPort);
unsigned char DHT22_ReadDataByte(int ptrPort);
int DHT22_ReadDataBit(int ptrPort);
int WaitForLineLow(int portAddr, int delay);
int WaitForLineHigh(int portAddr, int delay);
int WriteLine(int value, int portAddr);
int SetLineDir(int dir, int portAddr);