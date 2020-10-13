#include <iio.h>
#include <sys/time.h>
#include <stdio.h>

#define PLUTOIP "ip:pluto.local"
//#define PLUTOIP "ip:192.168.2.1"

void setPlutoRxFreq(long long rxfreq);
void initPluto(void);
long long runTimeMs(void);

int plutoPresent;
int plutoFailures;
long long lastClock;
long long elapsedTime;
long progStartTime=0;

struct iio_context *plutoctx;
struct iio_device *plutophy;

void main(void)
{
printf("\n\nLangstone Pluto comms test starting\n");
printf("###################################\n\n");
plutoPresent=0;      //will be set by initPluto if it responds. 
initPluto();
if(plutoPresent==0)
  {
  printf("ERROR... Pluto did not respond at %s\n",PLUTOIP);
  printf("Please check Pluto power, connection and configuration\n");
  return;
  }
 printf("Pluto found at %s\n\n",PLUTOIP);
 printf("Starting throughput test.....\n");
 plutoFailures=0;
 lastClock=runTimeMs();
 for(int n=0;n<1000;n++)
  {
  setPlutoRxFreq(430000000);
  }
  elapsedTime=runTimeMs()-lastClock;
  iio_context_destroy(plutoctx); 
  printf("Throughput test complete\n\n");
  printf("1000 Calls made in %lld ms with %d failures\n",elapsedTime,plutoFailures);
  printf("Pluto throughput measured at %lld calls per second\n",1000000/elapsedTime);
  printf("Typical value on a working system is 1900 calls per second\n");
  if(((1000000/elapsedTime) <1500) | (plutoFailures>0))
    {
    printf("\nPluto performance is below normal.\n");
    printf("It may still work but performance may be reduced.\n");
    printf("Please check USB Connection and Power supply.\n");
    printf("Ensure Pluto is directly connected to Pi and not through a USB Hub\n\n ");
    }
  else
    {
    printf("Pluto performance appears to be OK\n\n");
    }
}



void setPlutoRxFreq(long long rxfreq)
{
   int ret;
    ret=iio_channel_attr_write_longlong(iio_device_find_channel(plutophy, "altvoltage0", true),"frequency", rxfreq); //Rx LO Freq 
    if(ret<0) 
    {
    plutoFailures++;
    }
}


long long runTimeMs(void)
{
struct timeval tt;
gettimeofday(&tt,NULL);
if(progStartTime==0)
  {
    progStartTime=tt.tv_sec;
  }
tt.tv_sec=tt.tv_sec - progStartTime;
return ((tt.tv_sec*1000) + (tt.tv_usec/1000));
}


void initPluto(void)
{
    plutoctx = iio_create_context_from_uri(PLUTOIP);
      if(plutoctx==NULL)
      {
        plutoPresent=0;
        return;
      }
      else
      {
      plutoPresent=1;
      plutophy = iio_context_find_device(plutoctx, "ad9361-phy"); 
      }
}
