#include <mega2560.h>
#include <delay.h>
#include <parnik.h>
#include <dht22.h>

int DHT22_GetTempHum(float *ptrHum, float *ptrTemp, int ptrAPort)
{
    char dht_resp[5];
    int tmp, dht_res;
    dht_res = DHT22_ReadData(dht_resp, ptrAPort);
    if(dht_res == 0)
    {
        tmp = ((dht_resp[0] << 8) | dht_resp[1]);
        *ptrHum = (float)tmp / 10;
        tmp = ((dht_resp[2] << 8) | dht_resp[3]);
        *ptrTemp = (float)tmp / 10;
        
        return 0;
    }
    else
    {
        return dht_res;
    }
}

int DHT22_ReadData(unsigned char *ptr, int ptrPort)
{
    int i, sts;
    unsigned char CRCByte = 0;
    
    // Set start signal to line
    // Set port direction to OUTPUT
    //DDRC |= BIT5;
    sts = SetLineDir(1, ptrPort);
    if(sts != 0) return -50; 
    // Set line to LOW 
    //PORTC &= ~BIT5;
    sts = WriteLine(0, ptrPort);
    if(sts != 0) return -51;   
    // Wait for 1ms
    delay_us(1000);
    // Set port direction to INPUT 
    //DDRC &= ~BIT5;
    sts = SetLineDir(0, ptrPort);
    if(sts != 0) return -52;
    delay_us(2);
    // Wait for sensor response signal
    sts = WaitForLineLow(ptrPort, DHT22_TIMING_RESPONSE_SHORT);
    // Error! Response "SHORT" timed out
    if(sts < 0) return -53;    
    sts = WaitForLineHigh(ptrPort, DHT22_TIMING_RESPONSE_LOW);
    // Error! Response "HIGH" timed out
    if(sts < 0) return -54;
    sts = WaitForLineLow(ptrPort, DHT22_TIMING_RESPONSE_HIGH);
    // Error! Response "SHORT" timed out
    if(sts < 0) return -55;    
    
    for(i = 0; i < 5; i++)
    {
        ptr[i] = DHT22_ReadDataByte(ptrPort);
    }
    // Calculate CRC
    CRCByte = (unsigned char) (ptr[0] + ptr[1] + ptr[2] + ptr[3]);
    if(CRCByte != ptr[4]) return -4;
    else return 0;
}

unsigned char DHT22_ReadDataByte(int ptrPort)
{
    int i, tmp;
    unsigned char res = 0x00;
    
    for(i = 7; i >= 0; i--)
    {
        tmp = DHT22_ReadDataBit(ptrPort);
        if(tmp >= 0)
        {
            res |= (unsigned char)(tmp << i);     
        }
        else
        {
            res = 0xFF;
        }
    }
    return res;
}

int DHT22_ReadDataBit(int ptrPort)
{
    int count;
    count = WaitForLineHigh(ptrPort, DHT22_TIMING_DATA_LOW);
    if(count < 0) return -1;
    count = WaitForLineLow(ptrPort, DHT22_TIMING_DATA1_HIGH);
    if(count < 0) return -2;    
    
    if(count - DHT22_TIMING_DATA0_HIGH <= DHT22_TIMING_TOLERANCE)
    {
        // This is '0'
        return 0;
    }
    else
    {
        // This is '1'
        return 1;
    }
}

int WaitForLineLow(int portAddr, int delay)
{
    int count = 0;
    
    if(portAddr == INT_SENSOR_ADDR)
    {
        while(PINC.5 != 0)
        {
            //wait for line to LOW;
            delay_us(WAIT_STEP);
            count += WAIT_STEP;
            if(count - delay > DHT22_TIMING_TOLERANCE)
            {
                // Response "SHORT" timed out
                return -1;
            }
        }
        return count;
    }
    else
    {
        while(PINC.3 != 0)
        {
            //wait for line to LOW;
            delay_us(WAIT_STEP);
            count += WAIT_STEP;
            if(count - delay > DHT22_TIMING_TOLERANCE)
            {
                // Response "SHORT" timed out
                return -2;
            }
        }
        return count;
    }
}

int WaitForLineHigh(int portAddr, int delay)
{
    int count = 0;
    
    if(portAddr == INT_SENSOR_ADDR)
    {
        while(PINC.5 != 1)
        {
            //wait for line to HIGH;
            delay_us(WAIT_STEP);
            count += WAIT_STEP;
            if(count - delay > DHT22_TIMING_TOLERANCE)
            {
                // Response "SHORT" timed out
                return -1;
            }
        }
        return count;
    }
    else if(portAddr == EXT_SENSOR_ADDR)
    {
        while(PINC.3 != 1)
        {
            //wait for line to HIGH;
            delay_us(WAIT_STEP);
            count += WAIT_STEP;
            if(count - delay > DHT22_TIMING_TOLERANCE)
            {
                // Response "SHORT" timed out
                return -2;
            }
        }
        return count;
    }
    else
    {
        return -3;
    }
}

int WriteLine(int value, int portAddr)
{
    int res = -2;
    if((value != 1)&&(value != 0))
    {
        return -1;
    }       
    if(portAddr == INT_SENSOR_ADDR)
    {
        PORTC.5 = value;
        res = 0;
    }
    else if(portAddr == EXT_SENSOR_ADDR)
    {
        PORTC.3 = value;
        res = 0;
    }
    else res = -2;
    return res;
}
int SetLineDir(int dir, int portAddr)
{
    /*if((dir != 1)&&(dir != 0))
    {
        return -1;
    }*/
    if(portAddr == INT_SENSOR_ADDR)
    {   
        if(dir == 0) DDRC &= ~BIT5;
        else DDRC |= BIT5;
    }
    else if(portAddr == EXT_SENSOR_ADDR)
    {
        if(dir == 0) DDRC &= ~BIT3;
        else DDRC |= BIT3;
    }
    else
    {
        return -11;
    }
    return 0;
}