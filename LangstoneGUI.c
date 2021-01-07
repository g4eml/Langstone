
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <iio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "Touch.h"
#include "Mouse.h"
#include "mcp23017.c"
#include "Morse.c"


#define PLUTOIP "ip:pluto.local"

const char *i2cNode1 = "/dev/i2c-1";        //linux i2c node for the Hyperpixel display on Pi 4
const char *i2cNode2 = "/dev/i2c-11";       //linux i2c node for the Hyperpixel display on Pi 4
int  mcp23017_addr = 0x20;	               //MCP23017 I2C Address

void setFreq(double fr);
void displayFreq(double fr);
void setFreqInc();
void setTx(int ptt);
void setMode(int mode);
void setVolume(int vol);
void setSquelch(int sql);
void setSSBMic(int mic);
void setFMMic(int mic);
void setRxFilter(int low,int high);
void setTxFilter(int low,int high);
void setBandBits(int b);
void processTouch();
void processMouse(int mbut);
void initGUI();
void sendTxFifo(char * s);
void sendRxFifo(char * s);
void initFifos();
void initPluto(void);
void setPlutoRxFreq(long long rxfreq);
void setPlutoTxFreq(long long rxfreq);
void setHwRxFreq(double fr);
void setHwTxFreq(double fr);
void PlutoTxEnable(int txon);
void PlutoRxEnable(int rxon);
void detectHw();
int buttonTouched(int bx,int by);
void setKey(int k);
void displayMenu(void);
void displaySetting(int se);
void changeSetting(void);
void processGPIO(void);
void initGPIO(void);
void initMCP23017(int add);
int readConfig(void);
int writeConfig(void);
int satMode(void);
int splitMode(void);
int txvtrMode(void);
int duplexMode(void);
int multMode(void);
void setMoni(int m);
void initSDR(void);
void setFFTPipe(int cntl);
void waterfall(void);
void init_fft_Fifo();
void setRit(int rit);
void setInputMode(int n);
void gen_palette(char colours[][3],int num_grads);
void setPlutoTxAtt(int att);
void setPlutoRxGain(int gain);
int readPlutoRxGain(void);
void setBand(int b);
void setPlutoGpo(int p);
void setTxPin(int v);
long long runTimeMs(void);                                                    
void clearPopUp(void);
void displayPopupMode(void);
void displayPopupBand(void);
void send1750(void);
void displayError(char*st);
int minGain(double freq);
int maxGain(double freq);
void setDialLock(int d);
void setBeacon(int b);
int firstpass=1;
double freq;
double freqInc=0.001;
#define numband 12
int band=3;
double bandFreq[numband] = {70.200,144.200,432.200,1296.200,2320.200,2400.100,3400.100,5760.100,10368.200,24048.200,47088.2,10489.55};
double bandTxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10069.5};
double bandRxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10345.0};
double bandRepShift[numband]={0,-0.6,1.6,-6.0,0,0,0,0,0,0,0,0};
int bandTxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1};
int bandRxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1};
int bandMode[numband]={0,0,0,0,0,0,0,0,0,0,0,0};
int bandBits[numband]={0,1,2,3,4,5,6,7,8,9,10,11};
int bandSquelch[numband]={30,30,30,30,30,30,30,30,30,30,30,30};
int bandFFTRef[numband]={-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10};
int bandTxAtt[numband]={0,0,0,0,0,0,0,0,0,0,0,0};
int bandRxGain[numband]={100,100,100,100,100,100,100,100,100,100,100,100};              //100 is automatic gain
int bandDuplex[numband]={0,0,0,0,0,0,0,0,0,0,0,0};
float bandSmeterZero[numband]={-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80};
int bandSSBFiltLow[numband]={300,300,300,300,300,300,300,300,300,300,300,300};
int bandSSBFiltHigh[numband]={3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000};

int bbits=0;
#define minFreq 0.0
#define maxFreq 99999.99999
#define minHwFreq 69.9
#define maxHwFreq 5999.99999


#define nummode 6
int mode=0;
char * modename[nummode]={"USB","LSB","CW ","CWN","FM ","AM "};
enum {USB,LSB,CW,CWN,FM,AM};

#define numSettings 17

char * settingText[numSettings]={"Rx Gain= ","SSB Mic Gain= ","FM Mic Gain= ","Repeater Shift= "," Rx Offset= ","Rx Harmonic Mixing= "," Tx Offset= ","Tx Harmonic Mixing= ","Band Bits= ","FFT Ref= ","Tx Att= ","S-Meter Zero= ", "SSB Rx Filter Low= ", "SSB Rx Filter High= ","CW Ident= ", "CWID Carrier= ", "CW Break-In Hang Time= "};
enum {RX_GAIN,SSB_MIC,FM_MIC,REP_SHIFT,RX_OFFSET,RX_HARMONIC,TX_OFFSET,TX_HARMONIC,BAND_BITS,FFT_REF,TX_ATT,S_ZERO,SSB_FILT_LOW,SSB_FILT_HIGH,CWID,CW_CARRIER,BREAK_IN_TIME};
int settingNo=RX_GAIN;
int setIndex=0;
int maxSetIndex=10;

enum {FREQ,SETTINGS,VOLUME,SQUELCH,RIT};
int inputMode=FREQ;


//GUI Layout values X and Y coordinates for each group of buttons.

#define volButtonX 660
#define volButtonY 300
#define sqlButtonX 30
#define sqlButtonY 300
#define ritButtonX 670
#define ritButtonY 60
#define funcButtonsY 429
#define funcButtonsX 30
#define buttonHeight 50
#define buttonSpaceY 55
#define buttonWidth 100
#define buttonSpaceX 105
#define freqDisplayX 80
#define freqDisplayY 55
#define freqDisplayCharWidth 48 
#define freqDisplayCharHeight 54
#define txX 600
#define txY 15
#define modeX 200
#define modeY 15
#define txvtrX 400
#define txvtrY 15
#define moniX 500
#define moniY 15
#define settingX 200
#define settingY 390
#define popupX 30
#define popupY 374
#define FFTX 140
#define FFTY 216
#define sMeterX 2
#define sMeterY 5
#define sMeterWidth 170
#define sMeterHeight 40
#define errorX 200
#define errorY 240
#define diallockX 316
#define diallockY 15




int ptt=0;
int ptts=0;
int moni=0;
int fifofd;
int sendBeacon=0;
int dotCount=0;
int transmitting=0;
int dialLock=0;

int keyDownTimer=0;
int CWIDkeyDownTime=1000;                     //time to put key down between CW Idents (100 per second)

#define configDelay 500                              //delay before config is written after tuning (5 Seconds)
int configCounter=configDelay;

int twoButTimer=0;
int lastBut=0;

int breakInTimer=0;
int breakInTime=100;

long long lastLOhz;

long long lastClock;
long progStartTime=0;

int lastKey=1;

int volume=20;
#define maxvol 100

int squelch=20;
#define maxsql 100

int rit=0;
#define minrit -1000
#define maxrit 1000

int SSBMic=50;
#define maxSSBMic 100

int FMMic=50;
#define maxFMMic 100

int TxAtt=0;

int tuneDigit=8;
#define maxTuneDigit 11

#define TXDELAY 10000      //10ms delay between setting Tx output bit and sending tx command to SDR
#define RXDELAY 10000       //10ms delay between sending rx command to SDR and setting Tx output bit low. 

#define BurstLength 500000     //length of 1750Hz Burst   500ms

char mousePath[20];
char touchPath[20];
int mousePresent;
int touchPresent;
int plutoPresent;
int portsdownPresent;
int hyperPixelPresent;
int MCP23017Present;

int popupSel=0;
int popupFirstBand;
enum {NONE,MODE,BAND,BEACON};

#define pttPin 0        // Wiring Pi pin number. Physical pin is 11
#define keyPin 1        //Wiring Pi pin number. Physical pin is 12
#define txPin 29        //Wiring Pi pin number. Physical pin is 40      
#define bandPin1 31     //Wiring Pi pin number. Physical pin is 28
#define bandPin1alt 26  //Wiring Pi pin number. Physical pin is 32     Bandpin1 is copied to both of these pins to retain compatibility with Portsdown.
#define bandPin2 24     //Wiring Pi pin number. Physical pin is 35
#define bandPin3 7      //Wiring Pi pin number. Physical pin is 7
#define bandPin4 6      //Wiring Pi pin number. Physical pin is 22
#define bandPin5 4      //Wiring Pi pin number. Physical pin is 16
#define bandPin6 5      //Wiring Pi pin number. Physical pin is 18
#define bandPin7 12     //Wiring Pi pin number. Physical pin is 19
#define bandPin8 13     //Wiring Pi pin number. Physical pin is 21

#define i2cPttPin 0     //MCP23017 PTT Input if fitted  Port A bit 0
#define i2cKeyPin 1     //MCP23017 Key Input if fitted  Port A bit 1
#define i2cTxPin 7      //MCP23017 TX Output if fitted
                        //MCP23017 uses port B for band bits. 

int plutoGpo=0;

//robs Waterfall

float inbuf[2];
FILE *fftstream;
float buf[512][130];
int points=512;
int rows=130;
int FFTRef = -30;
int spectrum_rows=80;
unsigned char * palette;
int HzPerBin=94;                        //calculated from FFT width and number of samples. Width=48000 number of samples =512
int bwBarStart=3;
int bwBarEnd=34;
float sMeter;                             //peak reading S meter.
float sMeterPeak;

struct iio_context *plutoctx;
struct iio_device *plutophy;


int main(int argc, char* argv[])
{
  fftstream=fopen("/tmp/langstonefft","r");                 //Open FFT Stream from GNU Radio 
  fcntl(fileno(fftstream), F_SETFL, O_RDONLY | O_NONBLOCK);
  
  lastClock=0;
  readConfig();
  detectHw();
  initPluto();
  initFifos();
  init_fft_Fifo();
  initScreen();
  initGPIO();
  if(touchPresent) initTouch(touchPath);
  if(mousePresent) initMouse(mousePath);
  initGUI(); 
  initSDR(); 
  //              RGB Vals   Black >  Blue  >  Green  >  Yellow   >   Red     4 gradients    //number of gradients is varaible
  gen_palette((char [][3]){ {0,0,0},{0,0,255},{0,255,0},{255,255,0},{255,0,0}},4);

  setFFTPipe(1);            //Turn on FFT Stream from GNU RAdio

  
  while(1)
  {
  
    processGPIO();
                                                                                                                    
   if(touchPresent)
     {
       if(getTouch()==1)
        {
         processTouch();
        }
     }
    
    if(mousePresent)
      {
        int but=getMouse();
        if(but>0)
          {
             processMouse(but);
          }
       if(twoButTimer>0) 
       {
        twoButTimer--;
        if(twoButTimer==0)
          {
            lastBut=0;
          }
       }
           
      }
      
      
   
    if(sendBeacon==2)
      {
        dotCount=dotCount+1;
        if(dotCount==1)
          {
            setKey(1);
          }
        if(dotCount==12)
          {
            setKey(0);
          }
        if(dotCount==25)
          {
          dotCount=0;
          }
      } 
      


    if(sendBeacon==1)                                   //sending CWID
    {
      if(keyDownTimer>0)
        {    
          if((keyDownTimer>100) &&( keyDownTimer < CWIDkeyDownTime-100))                                //Key down between Idents
           {
            setKey(1);
           }
           else
           {
            setKey(0);
           }
        keyDownTimer--;
        }    
     else
      {
       int ret=morseKey();                              //get the next key from morse string
       if(ret==-1)                                      // Ident finished
        {
        keyDownTimer=CWIDkeyDownTime;                   //key down for this time between idents
        }
       else
        {  
          setKey(ret);
        }
      }
  
    }




    waterfall();

    if(configCounter>0)                                       //save the config after 5 seconds of inactivity.
    {
      configCounter=configCounter-1;                                                                                                  
      if(configCounter==0)
        {
        writeConfig();
        }
    }
    
   
   if(firstpass==1)
   {
   firstpass=0;
   setTx(1);                                              //seems to be needed to initialise Pluto
   setTx(0);
   }
    
    while(runTimeMs() < (lastClock + 10))                //delay until the next iteration at 100 per second (10ms)
    {
    usleep(100);
    }
    lastClock=runTimeMs();
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

void gen_palette(char colours[][3], int num_grads){
  //allocate some memory, size of palette
  palette = malloc(sizeof(char)*256*3);

  int diff[3];
  float scale=256/(num_grads);
  int pos=0;

  for(int i=0;i<num_grads;i++){
      //get differences in colours for current gradient
      diff[0]=(colours[i+1][0]-colours[i][0])/(scale-1);
      diff[1]=(colours[i+1][1]-colours[i][1])/(scale-1);
      diff[2]=(colours[i+1][2]-colours[i][2])/(scale-1);

      //create the palette built up of multiple gradients
      for(int n=0;n<scale;n++){
          palette[pos*3+2]=colours[i][0]+(n*diff[0]);
          palette[pos*3+1]=colours[i][1]+(n*diff[1]);
          palette[pos*3]=colours[i][2]+(n*diff[2]);
          pos++;
      }

  }
}

void waterfall()
{
  int level,level2;
  int ret;
  int centreShift=0;
  char smStr[10];
  static int sMeterCount=0;
  
      //check if data avilable to read
      ret = fread(&inbuf,sizeof(float),1,fftstream);
      if(ret>0)
    {    
    
        //shift buffer
        for(int r=rows-1;r>0;r--)
        {  
          for(int p=0;p<points;p++)
          {  
            buf[p][r]=buf[p][r-1];
          }
        }
    
        buf[0+points/2][0]=inbuf[0];    //use the read value
    
        //Read in float values, shift centre and store in buffer 1st 'row'
        for(int p=1;p<points;p++)
        {  
        fread(&inbuf,sizeof(float),1,fftstream);
          if(p<points/2)
          {
            buf[p+points/2][0]=inbuf[0];
          }else
          {
            buf[p-points/2][0]=inbuf[0];
          }
        }
 
        //use raw values to calculate S meter value
        //use highest reading within the receiver bandwidth
        
         sMeterPeak=-200;
         for (int p=points/2+bwBarStart;p<points/2+bwBarEnd;p++)
          {
           if(buf[p][0]>sMeterPeak)
             {
             sMeterPeak=buf[p][0];
             }
           
           }       
 
    
        //RF level adjustment
    
        int baselevel=FFTRef-80;
        float scaling = 255.0/(float)(FFTRef-baselevel);
    
        //draw waterfall
        for(int r=0;r<rows;r++)
        {  
          for(int p=0;p<points;p++)
          {                                                                                                           
            //limit values displayed to range specified
            if (buf[p][r]<baselevel) buf[p][r]=baselevel;
            if (buf[p][r]>FFTRef) buf[p][r]=FFTRef;
    
            //scale to 0-255
            level = (buf[p][r]-baselevel)*scaling;   
            setPixel(p+FFTX,FFTY+20+r,palette[level*3+2],palette[level*3+1],palette[level*3]);
          }
        }
    
        //clear spectrum area
        for(int r=0;r<spectrum_rows+1;r++)
        { 
          for(int p=0;p<points;p++)
          {   
            setPixel(p+FFTX,FFTY-r,0,0,0);
          }
        }
    
        //draw spectrum line
        
        scaling = spectrum_rows/(float)(FFTRef-baselevel);
        for(int p=0;p<points-1;p++)
        {  
            //limit values displayed to range specified
            if (buf[p][0]<baselevel) buf[p][0]=baselevel;
            if (buf[p][0]>FFTRef) buf[p][0]=FFTRef;
    
            //scale to display height
            level = (buf[p][0]-baselevel)*scaling;   
            level2 = (buf[p+1][0]-baselevel)*scaling;
            drawLine(p+FFTX, FFTY-level, p+1+FFTX, FFTY-level2,255,255,255);
        }
          
          //draw Bandwidth indicator
          int p=points/2;
          
          if ((mode==CW) || (mode==CWN))
          {
           centreShift=800/HzPerBin;
          }
          else
          {
           centreShift=0;          
          }

          drawLine(p+FFTX+bwBarStart, FFTY-spectrum_rows+5, p+FFTX+bwBarStart, FFTY-spectrum_rows,255,140,0);
          drawLine(p+FFTX+bwBarStart, FFTY-spectrum_rows, p+FFTX+bwBarEnd, FFTY-spectrum_rows,255,140,0);
          drawLine(p+FFTX+bwBarEnd, FFTY-spectrum_rows+5, p+FFTX+bwBarEnd, FFTY-spectrum_rows,255,140,0);  
          //draw centre line (displayed frequency)
          drawLine(p+FFTX+centreShift, FFTY-10, p+FFTX+centreShift, FFTY-spectrum_rows,255,0,0);  
          
          
          //draw Scale. 
          setForeColour(0,255,0);
          textSize=1;
          gotoXY(p+centreShift+FFTX-12,FFTY+8);
          displayStr(" 0 ");
          gotoXY(p+centreShift+FFTX-10000/HzPerBin-24,FFTY+8);
          displayStr(" -10k ");
          gotoXY(p+centreShift+FFTX-20000/HzPerBin-24,FFTY+8);
          displayStr(" -20k ");
          gotoXY(p+centreShift+FFTX+10000/HzPerBin-24,FFTY+8);
          displayStr(" +10k ");                                                                                             
          gotoXY(p+centreShift+FFTX+20000/HzPerBin-24,FFTY+8);
          displayStr(" +20k ");
 


          sMeterPeak=sMeterPeak-bandSmeterZero[band];                   //adjust offset to give positive values for s-meter
          int dbOver=0;
          int sValue=0;
          
          if(bandRxGain[band]==100)                  //if we are in RF AGC mode
            {
             sMeterPeak=sMeterPeak + maxGain(freq) - readPlutoRxGain();       //compensate for reduced gain due to AGC action
            }       
            
          if(sMeterPeak < 0) sMeterPeak=0;
          if(sMeterPeak >= sMeter)
            {
            sMeter=sMeterPeak;                                    //fast attack
            } 
          else
            {
            if(sMeter > 0) sMeter=sMeter-2;                    //slow decay
            }
            
 
          if(sMeter<55)
            {
            sValue=sMeter/6;
            }
          else
            {
            sValue=9;
            dbOver=sMeter-54;
            }
 
          
          
          //Draw S meter
          drawLine(sMeterX,sMeterY,sMeterX+sMeterWidth,sMeterY,255,255,255);
          drawLine(sMeterX,sMeterY,sMeterX,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX,sMeterY+sMeterHeight,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX+sMeterWidth,sMeterY,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);          

          for(int ln=0;ln<10;ln++)
          {
          if(sMeter<55)
          {
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+sMeter*2,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+sMeter*2,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }
          else
          {
          int redbit=sMeterX+6+110+(sMeter-55)*2;
          if(redbit > (sMeterX+6+160)) redbit= sMeterX+6+160;
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+110,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+110,sMeterY+5+ln,redbit,sMeterY+5+ln,255,0,0);
          drawLine(redbit,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }

          }
 
          sMeterCount++;
          if(sMeterCount>5)
          {
              sMeterCount=0;
              textSize=2;
              setForeColour(0,255,0);
              gotoXY(sMeterX+10,sMeterY+20);
              sprintf(smStr,"S%d",sValue);
              displayStr(smStr);
              if(dbOver>0)
                {
                sprintf(smStr,"+%ddB  ",dbOver);
                displayStr(smStr);
                }
              else
                {
                displayStr("       ");
                }
           }
 
   }       
}



void detectHw()
{
  FILE * fp;
  char * ln=NULL;
  size_t len=0;
  ssize_t rd;
  int p;
  char handler[2][10];
  char * found;
  p=0;
  mousePresent=0;
  touchPresent=0;
  portsdownPresent=0;
  fp=fopen("/proc/bus/input/devices","r");
   while ((rd=getline(&ln,&len,fp)!=-1))
    {
      if(ln[0]=='N')        //name of device
      {
        p=0;
        if((strstr(ln,"FT5406")!=NULL) || (strstr(ln,"pi-ts")!=NULL))         //Found Raspberry Pi TouchScreen entry
          {
           p=1;
           hyperPixelPresent=0;
          }
        if(strstr(ln,"Goodix")!=NULL)                                         //Found Hyperpixel TouchScreen entry
          {
          p=1;
          hyperPixelPresent=1;
          }                                                                  
      }
      
      if(ln[0]=='H')        //handlers
      {
         if(strstr(ln,"mouse")!=NULL)
         {
           found=strstr(ln,"event");
           strcpy(handler[p],found);
           handler[p][6]=0;
           if(p==0) 
            {
              sprintf(mousePath,"/dev/input/%s",handler[0]);
              mousePresent=1;
            }
           if(p==1) 
           {
             sprintf(touchPath,"/dev/input/%s",handler[1]);
             touchPresent=1;
           }
         }
      }   
    }
  fclose(fp);
  if(ln)  free(ln);
  
  if ((fp = fopen("/home/pi/rpidatv/bin/rpidatvgui", "r")))                      //test to see if Portsdown file is present. If so we change the exit behaviour. 
  {
    fclose(fp);
    portsdownPresent=1;
  }
  
  if ((fp = fopen("/home/pi/hyperpixel4/install.sh", "r")))                      //test to see if Hyperpixel4 file is present. If so we dont use the GPIO pins. 
  {
    fclose(fp);
    hyperPixelPresent=1;
  }
  else
  {
    hyperPixelPresent=0;
  }
  
    plutoPresent=1;      //this will be reset by setPlutoFreq if Pluto is not present.
  
// try to initialise MCP23017 i2c chip for additonal I/O
// this will set or reset MCP23017Present flag 
  initMCP23017(mcp23017_addr);
}

void displayError(char*st)
{
  gotoXY(errorX,errorY);
  setForeColour(255,0,0);
  textSize=2;
  displayStr(st);
}

void initPluto(void)
{
    plutoctx = iio_create_context_from_uri(PLUTOIP);
      if(plutoctx==NULL)
      {
        plutoPresent=0;
        displayError("Pluto not responding");
        return;
      }
      else
      {
      plutophy = iio_context_find_device(plutoctx, "ad9361-phy"); 
      }
}

void setPlutoRxFreq(long long rxfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=iio_channel_attr_write_longlong(iio_device_find_channel(plutophy, "altvoltage0", true),"frequency", rxfreq); //Rx LO Freq
      if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxFreq(long long txfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=iio_channel_attr_write_longlong(iio_device_find_channel(plutophy, "altvoltage1", true),"frequency", txfreq); //Tx LO Freq
   if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxAtt(int att)
{ 
  if(plutoPresent)
    {
      iio_channel_attr_write_double(iio_device_find_channel(plutophy, "voltage0", true),"hardwaregain", (double)att); //set Tx Attenuator     
    }
}

void setPlutoRxGain(int gain)
{ 
  if(plutoPresent)
    {
     if(gain>maxGain(freq))
        {
          iio_channel_attr_write(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "slow_attack");  //set Auto Gain
        }
        else
        {
        iio_channel_attr_write(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "manual");  //set Manual  Gain control
        iio_channel_attr_write_double(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", (double)gain); //set Rx Gain 
        }
    } 
}


int readPlutoRxGain(void)
{
double ret;
      if(plutoPresent)
      {
        iio_channel_attr_read_double(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", &ret); //Read current Rx Gain
        return (int) ret; 
      }
      else
      {
      return 73;
      }

}



void PlutoTxEnable(int txon)
{
  if(plutoPresent)
    { 
      if(txon==0)
        {
        iio_channel_attr_write_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", true); //turn off TX LO
        }
      else
        {
        iio_channel_attr_write_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", false); //turn on TX LO
        }
    }

}

void PlutoRxEnable(int rxon)
{
  if(plutoPresent)
    {
      if(rxon==0)
        {
        iio_channel_attr_write_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", true); //turn off RX LO
        }
      else
        {
        iio_channel_attr_write_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", false); //turn on RX LO
        }
    }

}

void setPlutoGpo(int p)
{
  char pins[10]; 
   
  sprintf(pins,"0x27 0x%x0",p);
  pins[9]=0;

  if(plutoPresent)
    {
      iio_device_debug_attr_write(plutophy,"direct_reg_access",pins);
    }
}


void initFifos()
{
 if(access("/tmp/langstoneTx",F_OK)==-1)   //does tx fifo exist already?
    {
        mkfifo("/tmp/langstoneTx", 0666);
    }
    
 if(access("/tmp/langstoneRx",F_OK)==-1)   //does rx fifo exist already?
    {
        mkfifo("/tmp/langstoneRx", 0666);
    }
}

void init_fft_Fifo()
{
 if(access("/tmp/langstonefft",F_OK)==-1)   //does fifo exist already?
    {
        mkfifo("/tmp/langstonefft", 0666);
    }
}

void sendTxFifo(char * s)
{
  char fs[50];
  int ret;
  int retry;
  strcpy(fs,s);
  strcat(fs,"\n");
  fifofd=open("/tmp/langstoneTx",O_WRONLY|O_NONBLOCK);
  retry=0;
  do
     {
       ret=write(fifofd,fs,strlen(fs));
       delay(5);
       retry++;
     }
   while((ret==-1)&(retry<10));   
  if(ret==-1)
    {
      displayError("Lang_TX.py Not Responding");
     }
  close(fifofd);
}

void sendRxFifo(char * s)
{
  char fs[50];
  int ret;
  int retry;
  strcpy(fs,s);
  strcat(fs,"\n");
  fifofd=open("/tmp/langstoneRx",O_WRONLY|O_NONBLOCK);
  retry=0;
    do
     {
       ret=write(fifofd,fs,strlen(fs));
       delay(5);
       retry++;
     }
   while((ret==-1)&(retry<10));   
  if(ret==-1)
    {
      displayError("Lang_RX.py Not Responding");
     }
  close(fifofd);
}

void initGPIO(void)
{
  if(hyperPixelPresent==0)
  {
  wiringPiSetup();
  pinMode(pttPin,INPUT);
  pinMode(keyPin,INPUT);
  pinMode(txPin,OUTPUT);
  pinMode(bandPin1,OUTPUT); 
  pinMode(bandPin1alt,OUTPUT);
  pinMode(bandPin2,OUTPUT);  
  pinMode(bandPin3,OUTPUT);  
  pinMode(bandPin4,OUTPUT);  
  pinMode(bandPin5,OUTPUT); 
  pinMode(bandPin6,OUTPUT);  
  pinMode(bandPin7,OUTPUT);  
  pinMode(bandPin8,OUTPUT);  
  digitalWrite(txPin,LOW);
  digitalWrite(bandPin1,LOW);
  digitalWrite(bandPin1alt,LOW);  
  digitalWrite(bandPin2,LOW); 
  digitalWrite(bandPin3,LOW); 
  digitalWrite(bandPin4,LOW); 
  digitalWrite(bandPin5,LOW); 
  digitalWrite(bandPin6,LOW); 
  digitalWrite(bandPin7,LOW); 
  digitalWrite(bandPin8,LOW);       
  lastKey=1;
  }
}

void processGPIO(void)
{
int p1=1;
int p2=1;
int k1=1;
int k2=1;

if(hyperPixelPresent==0)              //can only use GPIO if Hyperpixel Display is not fitted. 
  {
    p1=digitalRead(pttPin);
    k1=digitalRead(keyPin);
  }
  
if(MCP23017Present==1)                //MCP23017 extender chip can be used with any display. 
  {
    p2=mcp23017_readbit(mcp23017_addr,GPIOA,i2cPttPin);
    k2=mcp23017_readbit(mcp23017_addr,GPIOA,i2cKeyPin);
  }    
  
   p1=p1 & p2;        //allow PTT from either source
   k1=k1 & k2;        //allow Key from either source
  
  
    if(p1==0)
        {
          if(ptt==0)
            {
              ptt=1;
              setTx(ptt|ptts);
            }
        }
     else
        {
          if(ptt==1)
            {
              ptt=0;
              setTx(ptt|ptts);
            }
        }

      if(k1!=lastKey)
    {    
    setKey(!k1);
    lastKey=k1;
    } 

    if((mode==CW) ||(mode==CWN))
      {
        if(k1==0)          //key down
          {
            if((ptt|ptts)==0)   //not transmitting
              { 
                setTx(1);    
              }
            breakInTimer=breakInTime;
          }
        else
          {
            if((breakInTimer>0) & ((ptt|ptts)==0))
            {
            breakInTimer--;
            if(breakInTimer==0)
              {
              setTx(0);
              }
            }
          }
       }
     
}


// Configure the MCP23017 GPIO Extender if it is fitted :

void initMCP23017(int add) 
{
 int resp; 
 resp=i2c_init(i2cNode1);	               			//Initialize i2c node for a pi with normal display
 if(resp<0)                                   //not found, try for pi with Hypepixel4 display
 {
  resp=i2c_init(i2cNode2);	               		//Initialize i2c node fo4r a pi with normal display
   if(resp<0)
    {
     MCP23017Present=0;                        //neither found so disable it
     return;
    }
 }
 resp= mcp23017_readport(mcp23017_addr,GPIOA);      //dummy read to check if device is present
 if(resp<0)
 {
   MCP23017Present=0;
   return;
 }
 mcp23017_writereg(add,IODIR,GPIOA,0x03);      //Port A bits 0 and 1 are inputs (PTT and KEY)
 mcp23017_writereg(add,GPPU,GPIOA,0x03);      //Port A pullups enabled
 mcp23017_writereg(add,IODIR,GPIOB,0x00);      //Port B all outputs for Band Bits  
 mcp23017_writeport(add,GPIOA,0);
 mcp23017_writeport(add,GPIOB,0);             //Zero all outputs 
 MCP23017Present=1;
}


void sqlButton(int show)
{
 char sqlStr[5]; 
  gotoXY(sqlButtonX,sqlButtonY);
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
    }
  displayButton("SQL");
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",squelch);
  displayStr(sqlStr);
}

void ritButton(int show)
{
 char ritStr[5]; 
 int to;
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
      gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
      displayButton("Zero"); 
    }
  gotoXY(ritButtonX,ritButtonY);
  displayButton("RIT");
  textSize=2;
  to=0;
  if(abs(rit)>0) to=8;
  if(abs(rit)>9) to=16;
  if(abs(rit)>99) to=24;
  if(abs(rit)>999) to=32;
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr,"0");
  }
  else
  {
    sprintf(ritStr,"%+d",rit);
  }
  displayStr(ritStr);

}

void initGUI()
{
  clearScreen();


// Volume Button
  gotoXY(volButtonX,volButtonY);
  setForeColour(0,255,0);
  displayButton("Vol");
 

 //bottom row of buttons
  displayMenu();
  setBand(band);
  
  if(mode==FM) 
    {
    sqlButton(1);
    }
  else
    {
    sqlButton(0);
    }

    //clear waterfall buffer.
        //shift buffer
    for(int r=0;r<rows;r++){  
      for(int p=0;p<points;p++){  
        buf[p][r]=-100;
      }
    }

}


void initSDR(void)
{
  setBand(band);
  setMode(mode); 
  setVolume(volume);
  setSquelch(squelch);
  setRit(0);
  setSSBMic(SSBMic);
  setFMMic(FMMic);
  setFreqInc();
  lastLOhz=0;
  setFreq(freq);
  setTx(0);
}

void displayMenu()
{
gotoXY(funcButtonsX,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("BAND");
  displayButton("MODE");
  
  if((mode==FM) && (bandRepShift[band]!=0))
  {
  if((ptt | ptts) && (bandDuplex[band]>0))
    {
    displayButton("1750");
    }
  else
    {
    displayButton("DUP ");
    }
  }
  else
  {
  displayButton("    ");
  }

  displayButton("SET");
  if(satMode()==1) 
    {
    displayButton("MONI");
    }
  else
  {
    displayButton("    ");
  }
  if(sendBeacon > 0)
    {
    setForeColour(255,0,0);
    if(sendBeacon==1)
      {
        displayButton("CWID");
      } 
     else
      {
        displayButton("DOTS");
      } 
    }
  else
    {
    setForeColour(0,255,0);
    displayButton("BEACON");
    }

  if(ptt|ptts)
    {
    setForeColour(255,0,0);  
    }
  else
    {
    setForeColour(0,255,0);
    }   
  displayButton("PTT");

}


void displayPopupMode(void)
{
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
  for(int n=0;n<nummode;n++)
  {
    displayButton(modename[n]);
  }
popupSel=MODE;
}

void displayPopupBand(void)
{
char bstr[6];
int b;
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
 displayButton("More..");   
  for(int n=0;n<6;n++)
  {
    b=bandFreq[n+popupFirstBand];
    sprintf(bstr,"%d", b);
    displayButton(bstr);
  }
popupSel=BAND;
}

void displayPopupBeacon(void)
{
clearPopUp();
gotoXY(popupX + buttonSpaceX*5,popupY);
setForeColour(0,255,0);
displayButton("DOTS");
displayButton("CWID");
popupSel=BEACON;
}


void clearPopUp(void)
{
for(int py=popupY;py<popupY+buttonHeight+1;py++)
{
  for(int px=0;px<800;px++)
  {
  setPixel(px,py,0,0,0);
  }
}
popupSel=NONE;
displayMenu();
}

                                                           
void processMouse(int mbut)
{
  if(mbut==128)       //scroll whell turned 
    {
      if((inputMode==FREQ) && (dialLock==0))
      {
        freq=freq+(mouseScroll*freqInc);
        mouseScroll=0;
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) < minHwFreq) freq=(minHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) > maxHwFreq) freq=(maxHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        setFreq(freq);
        return;      
      }
      
      if(mouseScroll>0) mouseScroll=1;                     //prevent large changes when adjusting. 
      if(mouseScroll<0) mouseScroll=-1;
      
      if(inputMode==SETTINGS)
      {
        changeSetting();
        return;
      }
      if(inputMode==VOLUME)
      {
        volume=volume+mouseScroll;
        mouseScroll=0;
        if(volume < 0) volume=0;
        if(volume > maxvol) volume=maxvol;
        setVolume(volume);
        return;      
      }    
      if(inputMode==SQUELCH)
      {
        squelch=squelch+mouseScroll;
        mouseScroll=0;
        if(squelch < 0) squelch=0;
        if(squelch > maxsql) squelch=maxsql;
        bandSquelch[band]=squelch;
        setSquelch(squelch);
        return;      
      }   
      if(inputMode==RIT)
      {
        rit=rit+mouseScroll*10;
        mouseScroll=0;
        if(rit < minrit) rit=minrit;
        if(rit > maxrit) rit=maxrit;
        setRit(rit);
        return;      
      }     
    }
    
  if(mbut==1+128)      //Left Mouse Button down
    {
    
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS)))
       {
         setIndex=setIndex-1;
         if(setIndex<0) setIndex=0;
         displaySetting(settingNo);
       }
      else
       {
        tuneDigit=tuneDigit-1;
        if(tuneDigit<0) tuneDigit=0;
        if(tuneDigit==5) tuneDigit=4;
        if(tuneDigit==9) tuneDigit=8;
        setFreqInc();
        setFreq(freq);
        twoButTimer=20;
        lastBut=lastBut | 1;
        if((inputMode==SETTINGS) && (settingNo==BAND_BITS))
          {
          displaySetting(BAND_BITS);
          }
       }  
    }
    
  if(mbut==2+128)      //Right Mouse Button down
    {
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS)))
       {
         setIndex=setIndex+1;
         if(setIndex>maxSetIndex) setIndex=maxSetIndex;
         displaySetting(settingNo);
       }
      else  
       {
         tuneDigit=tuneDigit+1;
         if(tuneDigit > maxTuneDigit) tuneDigit=maxTuneDigit;
         if(tuneDigit==5) tuneDigit=6;
         if(tuneDigit==9) tuneDigit=10;
         setFreqInc();
         setFreq(freq); 
         twoButTimer=20;
         lastBut=lastBut | 2;
         if((inputMode==SETTINGS) && (settingNo==BAND_BITS))
           {
             displaySetting(BAND_BITS);
           }
        }          
    }
 
  if((mbut==3+128) | (lastBut==3))       //Middle button down or both buttons within 100ms
     {
      if(dialLock==0)
        {
          setDialLock(1);
        }
      else
        {
         setDialLock(0); 
        }
       lastBut=0;
     }
    
  if(mbut==4+128)      //Extra Button down
    {
 
    }   
    
  if(mbut==5+128)      //Side Button down
    {
 
    }
  
    
}

void setFreqInc()
{
  if(tuneDigit==0) freqInc=10000.0;
  if(tuneDigit==1) freqInc=1000.0;
  if(tuneDigit==2) freqInc=100.0;
  if(tuneDigit==3) freqInc=10.0;
  if(tuneDigit==4) freqInc=1.0;
  if(tuneDigit==5) tuneDigit=6;
  if(tuneDigit==6) freqInc=0.1;
  if(tuneDigit==7) freqInc=0.01;
  if(tuneDigit==8) freqInc=0.001;                                    
  if(tuneDigit==9) tuneDigit=10;
  if(tuneDigit==10) freqInc=0.0001;
  if(tuneDigit==11) freqInc=0.00001;   
}
       


void processTouch()
{ 

if(hyperPixelPresent==1)
{
float tempX=touchX;
float tempY=touchY;
tempX=tempX*0.6;                    //convert 0-800 to 0-480
tempY=800-(tempY*1.6666);           //convert 480-0   to 0-800
touchX=tempY;                       //swap X and Y
touchY=tempX;
}

   
// Volume Button   


if(buttonTouched(volButtonX,volButtonY))    //Vol
    {
      if(inputMode==VOLUME)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(VOLUME);
        }
      return;
    }
                                                        


// Squelch Button   


if(buttonTouched(sqlButtonX,sqlButtonY))    //sql
    {
     if(mode==FM)
     {
      if(inputMode==SQUELCH)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(SQUELCH);
        }
      return;
      }
    }


//RIT Button

if(buttonTouched(ritButtonX,ritButtonY))    //rit
    {
     if(mode!=4)
     {
      if(inputMode==RIT)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(RIT);
        }
      return;
      }
    }

//RIT Zero Button

if(buttonTouched(ritButtonX,ritButtonY+buttonSpaceY))    //rit zero
    {
     setRit(0);
     setInputMode(FREQ);
    }
//Function Buttons


if(buttonTouched(funcButtonsX,funcButtonsY))    //Button 1 = BAND or MENU
    {
    if((inputMode==FREQ) && (popupSel!=BAND))
    {
      writeConfig();
      displayPopupBand();
      return;
    }
    else
    {
      setInputMode(FREQ);
      clearPopUp();
      return; 
    }
      
        
    }      
if(buttonTouched(funcButtonsX+buttonSpaceX,funcButtonsY))    //Button 2 = MODE or Blank
    {
     if((inputMode==FREQ) && (popupSel!=MODE))
      {
      displayPopupMode();
      return;
      }
      else
      {
      setInputMode(FREQ);
      clearPopUp();
      return;
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*2,funcButtonsY))  // Button 3 =Blank or DUP or 1750 or NEXT
    {
     if(inputMode==FREQ)
      {
      if((mode==FM) && (bandRepShift[band]!=0))
      {
        if((ptt | ptts) && (bandDuplex[band]>0))
        {  
        send1750(); 
        }
        else
        {
        if(bandDuplex[band]==0) 
          {
          bandDuplex[band]=1;
          setMode(mode);
          }
         else 
          {
          bandDuplex[band]=0;
          setMode(mode);
          }

        }
       
      }      
      return;
      }
      else if(inputMode==SETTINGS)
      {
      settingNo=settingNo+1;
      if(settingNo==numSettings) settingNo=0;
      displaySetting(settingNo);
      return;
      }
      else
      {
      setInputMode(FREQ);
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*3,funcButtonsY))    // Button4 =SET or PREV
    {
     if(inputMode==FREQ)
      {
      setInputMode(SETTINGS);
      return;
      }
    else  if (inputMode==SETTINGS)
      {
      settingNo=settingNo-1;
      if(settingNo<0) settingNo=numSettings-1;
      displaySetting(settingNo);
      return;
      }
    else
      {
      setInputMode(FREQ);
      return;
      }
    }
       
if(buttonTouched(funcButtonsX+buttonSpaceX*4,funcButtonsY))    //Button 5 =MONI (only allowed in Sat mode)  or Blank
    {
    if(inputMode==FREQ)
      {
      if(satMode()==1)
        {
        if(moni==1) setMoni(0); else setMoni(1);
        }      
      return;
      }     
    else
      {
      setInputMode(FREQ);
      }
      
 
    }      

if(buttonTouched(funcButtonsX+buttonSpaceX*5,funcButtonsY))    //Button 6 = BEACON  or Exit to Portsdown
    {
    if(inputMode==FREQ)
      {
      if(sendBeacon==0)
        {
         displayPopupBeacon();
        }
      else
        {
        setBeacon(0);
        }
 
      return;
      }
      else if (inputMode==SETTINGS)
      {
         clearScreen();
         iio_context_destroy(plutoctx);                  
         exit(0);
      }
      else
      {
      setInputMode(FREQ);
      }
    } 
         
if(buttonTouched(funcButtonsX+buttonSpaceX*6,funcButtonsY))   //Button 7 = PTT  or OFF
    {
    if(inputMode==FREQ)
      {
      if(ptts==0)
        {
          ptts=1;
          sendBeacon=0;
          setTx(ptt|ptts);
          gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
          setForeColour(255,0,0);
          displayButton("PTT"); 
        }
      else
        {
          ptts=0;
          if(sendBeacon==1)
          {
          sendBeacon=0;
          setMode(mode);
          gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
          setForeColour(0,255,0);
          displayButton("BEACON");  
          }

          setTx(ptt|ptts);
          gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
          setForeColour(0,255,0);
          displayButton("PTT");
          gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
          setForeColour(0,255,0);
          displayButton("BEACON");   
        }
      return;
      }
      else if (inputMode==SETTINGS)
      {
      sendTxFifo("h");        //unlock the Tx so that it can exit
      sendRxFifo("h");        //and unlock the Rx just in case
      sendTxFifo("Q");       //kill the SDR Tx
      sendRxFifo("Q");       //kill the SDR Rx
      writeConfig();
      iio_context_destroy(plutoctx);
      system("sudo cp /home/pi/Langstone/splash.bgra /dev/fb0");
      sleep(2);
      system("sudo poweroff");                          
      return;      
      }
      else
      {
      setInputMode(FREQ);
      }
      

    }      



   
//Touch on Frequency Digits moves cursor to digit and sets tuning step. Removes dial lock if it is set.  

if((touchY>freqDisplayY) & (touchY < freqDisplayY+freqDisplayCharHeight) & (touchX>freqDisplayX) & (touchX < freqDisplayX+12*freqDisplayCharWidth))   
  {
    if(inputMode==FREQ) setDialLock(0);
    int tx=touchX-freqDisplayX;
    tx=tx/freqDisplayCharWidth;
    tuneDigit=tx;
    setFreqInc();
    setFreq(freq);
    return;
  }


if(popupSel==MODE)
{
  for(int n=0;n<nummode;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       mode=n;
       setMode(mode);
       clearPopUp();
       }
  }
}

if(popupSel==BAND)
{
  if(buttonTouched(popupX,popupY))
  {
  popupFirstBand=popupFirstBand+6;
  if(popupFirstBand>11) popupFirstBand=0;
  displayPopupBand();
  }
  
  for(int n=1;n<7;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       band=n-1+popupFirstBand;
       setBand(band);
       clearPopUp();
       }
  }

 
}


if(popupSel==BEACON)
  {
    if(buttonTouched(popupX+ 5* buttonSpaceX,popupY))       //DOTS
      {
      setBeacon(2);
      clearPopUp();
      }
     if(buttonTouched(popupX+ 6* buttonSpaceX,popupY))       //CWID
      {
      setBeacon(1);
      clearPopUp();
      }     
  }







}


int buttonTouched(int bx,int by)
{
  return ((touchX > bx) & (touchX < bx+buttonWidth) & (touchY > by) & (touchY < by+buttonHeight));
}

void setBand(int b)
{
  freq=bandFreq[band];
  setFreq(freq);
  mode=bandMode[band];
  setMode(mode);
  bbits=bandBits[band];
  setBandBits(bbits);
  squelch=bandSquelch[band];
  setSquelch(squelch);
  FFTRef=bandFFTRef[band];
  TxAtt=bandTxAtt[band];
  setPlutoTxAtt(TxAtt);
  setPlutoRxGain(bandRxGain[band]);
  configCounter=configDelay;
}

void setBeacon(int b)
{
 if(b > 0)
   {
      sendBeacon=b;
      morseReset();
      keyDownTimer=300;
      setMode(2);
      if(!(ptt|ptts))                     //if not already transmitting
        {
          setTx(1);                          //goto transmit
        }
      ptts=1;                             //latch the transmit on
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("PTT");       
   }
  else
    {
      sendBeacon=0;
      ptts=0;
      setTx(0);
      setKey(0);
      setMode(mode);
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("PTT");       
    }

}

 
void setVolume(int vol)
{
  char volStr[10];
  sprintf(volStr,"V%d",vol);
  sendRxFifo(volStr);
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(volButtonX+30,volButtonY-25);
  displayStr("   ");
  gotoXY(volButtonX+30,volButtonY-25);
  sprintf(volStr,"%d",vol);
  displayStr(volStr);
  
  configCounter=configDelay;
}

void setSquelch(int sql)
{
  char sqlStr[10];
  sprintf(sqlStr,"S%d",sql);
  sendRxFifo(sqlStr);
  if(mode==FM)
  {
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",sql);
  displayStr(sqlStr);
  configCounter=configDelay;
  }
}

void setInputMode(int m)

{
if(inputMode==SETTINGS)
  {
  gotoXY(0,settingY);
  setForeColour(255,255,255);
  textSize=2;
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  writeConfig();
  displayMenu();
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(0,255,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(0,255,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(0,255,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(0,0,0);
    displayButton("Zero");
  }
  
inputMode=m;

if(inputMode==FREQ)
  {
  setFreq(freq);
  }

if(inputMode==SETTINGS)
  {
    clearPopUp();
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    displayButton(" ");
    displayButton("NEXT");
    displayButton("PREV");
    displayButton(" ");

    if (portsdownPresent==1)
    {
        setForeColour(255,0,0);
        displayButton2x12("EXIT TO","PORTSDOWN");
    }
    else
    {
        displayButton(" ");
    }
    setForeColour(255,0,0);
    displayButton1x12("SHUTDOWN");
    mouseScroll=0;
    displaySetting(settingNo); 
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(255,0,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(255,0,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(255,0,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(255,0,0);
    displayButton("Zero");
  }

}

void setRit(int ri)
{
  char ritStr[10];
  int to;
  if(!((mode==FM) || (mode==AM)))
  {
  rit=ri;
  setForeColour(0,255,0);
  textSize=2;
  to=0;
  if(abs(rit)>0) to=8;
  if(abs(rit)>9) to=16;
  if(abs(rit)>99) to=24;
  if(abs(rit)>999) to=32; 
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr,"0");
  }
  else
  {
    sprintf(ritStr,"%+d",rit);
  }
  displayStr(ritStr);
  setFreq(freq);
  }
}


void setSSBMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"G%d",mic);
  sendTxFifo(micStr);
}

void setFMMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"g%d",mic);
  sendTxFifo(micStr);
}

void setKey(int k)
{
if(k==0) sendTxFifo("k"); else sendTxFifo("K");
}

void setFFTPipe(int ctrl)
{
if(ctrl==0) sendRxFifo("p"); else sendRxFifo("P");
}

void setRxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"f%d",low);
  sendRxFifo(filtStr);                                             
  sprintf(filtStr,"F%d",high);
  sendRxFifo(filtStr);
  
  bwBarStart=low/HzPerBin;
  bwBarEnd=high/HzPerBin;
  
}

void setTxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"f%d",low);
  sendTxFifo(filtStr);
  sprintf(filtStr,"F%d",high);
  sendTxFifo(filtStr);
  
}


void setMode(int md)
{
  bandMode[band]=md;
  gotoXY(modeX,modeY);
  setForeColour(255,255,0);
  textSize=2;
  displayStr(modename[md]);
  if((md==FM)&&(bandDuplex[band]==1))
    {
    displayStr("DUP");
    }
  else
    {
    displayStr("   ");
    }      
  if(md==USB)
    {
    sendTxFifo("M0");    //USB
    sendRxFifo("M0");    //SSB
    setTxFilter(300,3000);    //USB Filter Setting
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);    //USB Filter Setting    configured in settings.
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(0);
    ritButton(1);
    setRit(0);
    } 
  
  if(md==LSB)
    {
    sendTxFifo("M1");    //LSB
    sendRxFifo("M1");    //LSB
    setTxFilter(-3000,-300); // LSB Filter Setting
    setRxFilter(-1*bandSSBFiltHigh[band],-1*bandSSBFiltLow[band]); // LSB Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(0);
    ritButton(1);
    setRit(0);
    } 
  
  if(md==CW)
    {
    sendTxFifo("M2");    //CW
    sendRxFifo("M2");    //CW
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);   // USB filter settings used for CW Wide Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(0);
    ritButton(1);
    setRit(0);
    } 
    
  if(md==CWN)
    {
    sendTxFifo("M3");    //CWN
    sendRxFifo("M3");    //CWN
    setRxFilter(600,1000);    //CW Narrow Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(0);
    ritButton(1);
    setRit(0);
    } 
  if(md==FM)
    {
    sendTxFifo("M4");    //FM
    sendRxFifo("M4");    //FM
    setRxFilter(-7500,7500);    //FM Filter
    setTxFilter(-7500,7500);    //FM Filter  
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(1);
    ritButton(0);
    setRit(0);
    } 
  if(md==AM)
    {
    sendTxFifo("M5");    //AM
    sendRxFifo("M5");    //AM
    setRxFilter(-5000,5000);    //AM Filter 
    setTxFilter(-5000,5000);    //AM Filter 
    setFreq(freq);    //set the frequency to adjust for CW offset.
    sqlButton(0);
    ritButton(0);
    setRit(0);
    } 

                                                                 
configCounter=configDelay;
}

void setTxPin(int v)
{
  if(hyperPixelPresent==0)                      //cant use GPIO if Hyperpixel display is fitted 
  {
  if(v==1) digitalWrite(txPin,HIGH); else digitalWrite(txPin,LOW);
  }
  
  if(MCP23017Present==1)                        //copy to optional extender chip which can be used with any config
  {
  if(v==1) mcp23017_writebit(mcp23017_addr,GPIOA,i2cTxPin,1); else mcp23017_writebit(mcp23017_addr,GPIOA,i2cTxPin,0);
  }
}

void setTx(int pt)
{
  if((pt==1)&&(transmitting==0))
    {
      setTxPin(1);
      plutoGpo=plutoGpo | 0x10;
      setPlutoGpo(plutoGpo);                               //set the Pluto GPO Pin 
      usleep(TXDELAY);
      setHwTxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq+bandRepShift[band]);
        displayMenu();
        }
      PlutoTxEnable(1);
      if (moni==0) sendRxFifo("U");                        //mute the receiver
      if(satMode()==0)
      {
        setFFTPipe(0);                        //turn off the FFT stream
        setHwRxFreq(freq+10.0);               //offset the Rx frequency to prevent unwanted mixing. (happens even if disabled!) 
        PlutoRxEnable(0);
        sendRxFifo("H");                      //freeze the receive Flowgraph 
      }
      sendTxFifo("h");                        //unfreeze the Tx Flowgraph
      sendTxFifo("T");
      gotoXY(txX,txY);
      setForeColour(255,0,0);
      textSize=2;
      displayStr("Tx");
      transmitting=1;  
    }
  else if((pt==0)&&(transmitting==1))
    {
      sendTxFifo("R");
      sendTxFifo("H");                  //freeze the Tx Flowgraph
      sendRxFifo("h");                  //unfreeze the Rx Flowgraph
      sendRxFifo("u");                  //unmute the receiver
      setHwTxFreq(freq+10.0);           //offset the Tx freq to prevent unwanted spurious
      PlutoTxEnable(0);
      PlutoRxEnable(1);
      setFFTPipe(1);                //turn on the FFT Stream
      setHwRxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq);
        displayMenu();
        }
      gotoXY(txX,txY);
      setForeColour(0,255,0);
      textSize=2;
      displayStr("Rx");
      transmitting=0;
      usleep(RXDELAY);
      setTxPin(0);
      plutoGpo=plutoGpo & 0xEF;
      setPlutoGpo(plutoGpo);                               //clear the Pluto GPO Pin 
    }
}

void setHwRxFreq(double fr)
{
  long long rxoffsethz;
  long long LOrxfreqhz;
  long long rxfreqhz;
  double frRx;
  double frTx;
  
  frRx=fr+bandRxOffset[band];
  
  rxfreqhz=frRx*1000000;
  
  if (rxfreqhz<69900000) rxfreqhz=69900000;         //this is the lowest frequency we can receive with a pluto 
  
  if(rxfreqhz<70100000)
  {
  rxoffsethz=(rxfreqhz-70000000);        //Special case for receiving below 70.100     Use the offset of +-100KHz
  LOrxfreqhz=70000000;
  }
  else
  {
  rxoffsethz=(rxfreqhz % 100000)+50000;        //use just the +50Khz to +150Khz positive side of the sampled spectrum. This avoids seeing the DC hump .
  LOrxfreqhz=rxfreqhz-rxoffsethz;
  }

  if( bandRxHarmonic[band]>1)                                //allow for harmonic mixing for higher bands (10GHz)
    {
      LOrxfreqhz=LOrxfreqhz/bandRxHarmonic[band];
    }
  

  rxoffsethz=rxoffsethz+rit;
  if((mode==CW)|(mode==CWN))
    {
     rxoffsethz=rxoffsethz-800;         //offset  for CW tone of 800 Hz    
    }
  if(LOrxfreqhz!=lastLOhz)         
    {
      setPlutoRxFreq(LOrxfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
      lastLOhz=LOrxfreqhz;
    }
  
  char offsetStr[32];
  sprintf(offsetStr,"O%d",rxoffsethz);   //send the rx offset tuning value 
  sendRxFifo(offsetStr);
}

void setHwTxFreq(double fr)
{                                                                        
  long long txfreqhz;
  double frTx;
  
  frTx=fr+bandTxOffset[band];
  
  if((mode==FM)&&(bandDuplex[band]==1))
    {
    frTx=frTx+bandRepShift[band];
    }
 
  txfreqhz=frTx*1000000;
   
   if(bandTxHarmonic[band]>1)                    //allow for Harmonic mixing for higher bands or for external multiplier such as Hydra
    {
    txfreqhz=txfreqhz/bandTxHarmonic[band];
    } 
  
      setPlutoTxFreq(txfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
}

void displayFreq(double fr)
{
  long long freqhz;
  char digit[16]; 
  
  fr=fr+0.0000001;   // correction for rounding errors.
  freqhz=fr*1000000;    
  freqhz=freqhz+100000000000;     //force it to be 12 digits long
  sprintf(digit,"%lld",freqhz);
  
  gotoXY(freqDisplayX,freqDisplayY);
  setForeColour(0,0,255);
  textSize=6;
  if(digit[1]>'0') displayChar(digit[1]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0')) displayChar(digit[2]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0') | digit[3]>'0')  displayChar(digit[3]); else displayChar(' ');
  displayChar(digit[4]);
  displayChar(digit[5]);
  displayChar('.');
  displayChar(digit[6]);
  displayChar(digit[7]);
  displayChar(digit[8]);
  displayChar('.');
  displayChar(digit[9]);
  displayChar(digit[10]);
  
// Underline the currently selected tuning digit
  
  for(int dtd=0;dtd<12;dtd++)
    {
      gotoXY(freqDisplayX+dtd*freqDisplayCharWidth+4,freqDisplayY+freqDisplayCharHeight+5);
      int bb = 0;
      if (dtd==tuneDigit) bb=255;
      for(int p=0; p < freqDisplayCharWidth; p++)
        {
          setPixel(currentX+p,currentY+1,0,0,bb);
          setPixel(currentX+p,currentY+2,0,0,bb);
          setPixel(currentX+p,currentY+3,0,0,bb);
        }
    }

}

void setFreq(double fr)
{

  
  if(ptt | ptts) 
  {
    setHwTxFreq(fr);        //set Hardware Tx frequency if we are transmitting
  }
  else
  {
  setHwRxFreq(fr);       //set Hardware Rx frequency if we are receiving
  }

displayFreq(fr);
 
//set XVTR, SAT, SPLIT or MULT indication if needed.
  gotoXY(txvtrX,txvtrY);
  setForeColour(0,255,0);
  textSize=2;
  if(multMode()==1)
    {
    displayStr(" MULT  ");
    }  
  else if( txvtrMode()==1)
    {
     displayStr(" XVTR  "); 
    }
  else if(satMode()==1)
    {
    displayStr("  SAT  ");
    }
  else if(splitMode()==1)
    {
    displayStr(" SPLIT ");
    } 
  else
    {
      displayStr("       ");
    } 
 
   gotoXY(funcButtonsX+buttonSpaceX*4,funcButtonsY);
   setForeColour(0,255,0);
   if(inputMode==FREQ)
   {
     if(satMode()==1)
      {
       displayButton("MONI");
       setMoni(moni);
      }  
      else
      {
       displayButton("    ");
       setMoni(0);
      }
    }
 
 configCounter=configDelay;                       //write config after this amount of inactivity    
}

int satMode(void)
{
if(((abs(bandTxOffset[band]-bandRxOffset[band]) > 1 ) & (bandRxOffset[band]!=0) ) & bandRxHarmonic[band]<2  & bandTxHarmonic[band]<2 )     // if we have a differnt Rx and Tx offset and we are not multiplying then we must be in Sat mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int txvtrMode(void)
{
if((abs(bandTxOffset[band]-bandRxOffset[band]) <1) & (abs(bandTxOffset[band]) >1 )  )       //if the tx and rx offset are the same and non zero then we are in Transverter mode
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int splitMode(void)
{
if((abs(bandTxOffset[band])>0) & (bandRxOffset[band]==0))     //  if tx Offset is non zero and rxoffset is zero then we are in split mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int multMode(void)
{
if((bandRxHarmonic[band]>1) || (bandTxHarmonic[band] >1))     //  if either of the Harmonic modes is set then we are in Mult mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

void setDialLock(int d)
{
  if(d==0)
  {
    dialLock=0;
    gotoXY(diallockX,diallockY);
    textSize=2;
    displayStr("    ");
  }
  else
  {
    dialLock=1;
    gotoXY(diallockX,diallockY);
    textSize=2;
    setForeColour(255,0,0);
    displayStr("LOCK");
  }
}

void setMoni(int m)
{
  if(m==1)
    {
     sendRxFifo("u");
     moni=1;
     gotoXY(moniX,moniY);
     textSize=2;
     setForeColour(0,255,0);
     displayStr("MONI");
    } 
  else
    {
     if (ptt | ptts) sendRxFifo("U");
     moni=0;
     gotoXY(moniX,moniY);
     textSize=2;
     displayStr("    ");
    }
}

void send1750(void)
{
sendTxFifo("A");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(255,0,0);
displayButton("1750");
usleep(BurstLength);
sendTxFifo("a");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(0,255,0);
displayButton("1750");
}

void setBandBits(int b)
{
if(hyperPixelPresent==0)                //dont use Raspberry Pi GPIO with Hyperpixel Display
  {
    if(b & 0x01) 
        {
        digitalWrite(bandPin1,HIGH);
        digitalWrite(bandPin1alt,HIGH);
        }
    else
        {
        digitalWrite(bandPin1,LOW);
        digitalWrite(bandPin1alt,LOW);
        }
        
    if(b & 0x02) 
        {
        digitalWrite(bandPin2,HIGH);
        }
    else
        {
        digitalWrite(bandPin2,LOW);
        }   
    
    if(b & 0x04) 
        {
        digitalWrite(bandPin3,HIGH);
        }
    else
        {
        digitalWrite(bandPin3,LOW);
        }   
    
    if(b & 0x08) 
        {
        digitalWrite(bandPin4,HIGH);
        }
    else
        {
        digitalWrite(bandPin4,LOW);
        }   
    
    if(b & 0x10) 
        {
        digitalWrite(bandPin5,HIGH);
        }
    else
        {
        digitalWrite(bandPin5,LOW);
        }   
    
    if(b & 0x20) 
        {
        digitalWrite(bandPin6,HIGH);
        }
    else
        {
        digitalWrite(bandPin6,LOW);
        }   
        
    if(b & 0x40) 
        {
        digitalWrite(bandPin7,HIGH);
        }
    else
        {
        digitalWrite(bandPin7,LOW);
        }   
        
    if(b & 0x80) 
        {
        digitalWrite(bandPin8,HIGH);
        }
    else
        {
        digitalWrite(bandPin8,LOW);
        }       
  }

//  copy bits 0,1 and 2to Pluto GPO Pins

  if(b & 0x01) 
      {
      plutoGpo=plutoGpo | 0x20;
      }
  else
      {
      plutoGpo=plutoGpo & 0xDF;
      }
      
  if(b & 0x02) 
      {
      plutoGpo=plutoGpo | 0x40;
      }
  else
      {
      plutoGpo=plutoGpo & 0xBF;
      }   
  
  if(b & 0x04) 
      {
      plutoGpo=plutoGpo | 0x80;
      }
  else
      {;
      plutoGpo=plutoGpo & 0x7F;
      }   
  setPlutoGpo(plutoGpo);
  
if(MCP23017Present==1)                       //optional extender chip has port b for band bits. 
  {
  mcp23017_writeport(mcp23017_addr,GPIOB,b);     
  }
}


void changeSetting(void)
{
  if(settingNo==SSB_MIC)        //SSB Mic Gain
      {
      SSBMic=SSBMic+mouseScroll;
      mouseScroll=0;
      if(SSBMic<0) SSBMic=0;
      if(SSBMic>maxSSBMic) SSBMic=maxSSBMic;
      setSSBMic(SSBMic);
      displaySetting(settingNo);
      }
   if(settingNo==FM_MIC)        // FM Mic Gain
      {
      FMMic=FMMic+mouseScroll;
      mouseScroll=0;
      if(FMMic<0) FMMic=0;
      if(FMMic>maxFMMic) FMMic=maxFMMic;
      setFMMic(FMMic);
      displaySetting(settingNo);
      }
    if(settingNo==REP_SHIFT)        //Repeater Shift
      {
        bandRepShift[band]=bandRepShift[band]+mouseScroll*freqInc;
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
   if(settingNo==RX_OFFSET)        //Transverter Rx Offset 
      {
        bandRxOffset[band]=bandRxOffset[band]+mouseScroll*freqInc;
        if(bandRxOffset[band] > 99999.9) bandRxOffset[band]= 99999.9;
        if(bandRxOffset[band] < -99999.9) bandRxOffset[band]= -99999.9;
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      } 
 if(settingNo==RX_HARMONIC)        // RX Harmonic mixing number
      {
      if(mouseScroll>0)
      {
        bandRxHarmonic[band]=5;
      }
      if(mouseScroll<0)
      {
         bandRxHarmonic[band]=1;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);  
      }    
   if(settingNo==TX_OFFSET)        //Transverter Tx Offset
      {
        bandTxOffset[band]=bandTxOffset[band]+mouseScroll*freqInc;
        if(bandTxOffset[band] > 99999.9) bandTxOffset[band]= 99999.9;
        if(bandTxOffset[band] < -99999.9) bandTxOffset[band]= -99999.9;
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
 if(settingNo==TX_HARMONIC)        // TX Harmonic mixing number or external multiplier
      {
      bandTxHarmonic[band]=bandTxHarmonic[band]+mouseScroll;
      if(bandTxHarmonic[band]<1) bandTxHarmonic[band]=1;
      if(bandTxHarmonic[band]>5) bandTxHarmonic[band]=5;
      if(mouseScroll>0)
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=5;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=5;
      }
      else
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=2;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=2;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);  
      }      
   if(settingNo==BAND_BITS)        // Band Bits
      {
      if(setIndex==7)
        {
        bandBits[band]=bandBits[band] ^ 0x01; 
        }
      if(setIndex==6)
        {
        bandBits[band]=bandBits[band] ^ 0x02; 
        }
      if(setIndex==5)
        {
        bandBits[band]=bandBits[band] ^ 0x04; 
        }
      if(setIndex==4)
        {
        bandBits[band]=bandBits[band] ^ 0x08; 
        }
       if(setIndex==3)
        {
        bandBits[band]=bandBits[band] ^ 0x10; 
        }
      if(setIndex==2)
        {
        bandBits[band]=bandBits[band] ^ 0x20; 
        }
      if(setIndex==1)
        {
        bandBits[band]=bandBits[band] ^ 0x40; 
        }
      if(setIndex==0)
        {
        bandBits[band]=bandBits[band] ^ 0x80; 
        }
      mouseScroll=0;
      if(bandBits[band]<0) bandBits[band]=0;
      if(bandBits[band]>255) bandBits[band]=255;
      bbits=bandBits[band];
      setBandBits(bbits);
      displaySetting(settingNo);  
      }    
   if(settingNo==FFT_REF)        // FFT Ref Level
      {
      FFTRef=FFTRef+mouseScroll;
      mouseScroll=0;
      if(FFTRef<-80) FFTRef=-80;
      if(FFTRef>30) FFTRef=30;
      bandFFTRef[band]=FFTRef;
      displaySetting(settingNo);  
      }    
    if(settingNo==TX_ATT)        // Tx Attenuator
      {
      TxAtt=TxAtt+mouseScroll;
      mouseScroll=0;
      if(TxAtt<-89) TxAtt=-89;
      if(TxAtt>0) TxAtt=0;
      bandTxAtt[band]=TxAtt;
      setPlutoTxAtt(TxAtt);
      displaySetting(settingNo);  
      }  
     if(settingNo==RX_GAIN)        // Rx Gain Setting
      {
      if (bandRxGain[band] == 100)
        {
        bandRxGain[band]=maxGain(freq)+1+mouseScroll;
        }
      else
      {
        bandRxGain[band]=bandRxGain[band]+mouseScroll;
      }

      mouseScroll=0;
      if(bandRxGain[band]< minGain(freq)) bandRxGain[band]=minGain(freq);
      if(bandRxGain[band]> maxGain(freq)) bandRxGain[band]=100;
      setPlutoRxGain(bandRxGain[band]);
      displaySetting(settingNo);  
      }                   
    if(settingNo==S_ZERO)        // S Meter Zero
      {
      bandSmeterZero[band]=bandSmeterZero[band]+mouseScroll;
      mouseScroll=0;
      if(bandSmeterZero[band]<-140) bandSmeterZero[band]=-140;
      if(bandSmeterZero[band]>-30) bandSmeterZero[band]=-30;
      displaySetting(settingNo);  
      }   
    if(settingNo==SSB_FILT_LOW)        // SSB Filter Low
      {
      bandSSBFiltLow[band]=bandSSBFiltLow[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltLow[band] < 0) bandSSBFiltLow[band]=0;
      if(bandSSBFiltLow[band] >1000) bandSSBFiltLow[band]=1000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }                    
    if(settingNo==SSB_FILT_HIGH)        // SSB Filter High
      {
      bandSSBFiltHigh[band]=bandSSBFiltHigh[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltHigh[band] < 1000) bandSSBFiltHigh[band]=1000;
      if(bandSSBFiltHigh[band] > 5000) bandSSBFiltHigh[band]=5000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }     
    if(settingNo==CW_CARRIER)        // CWID Carrier time
      {
      CWIDkeyDownTime=CWIDkeyDownTime+mouseScroll*100;
      mouseScroll=0;
      if(CWIDkeyDownTime< 500) CWIDkeyDownTime=500;
      if(CWIDkeyDownTime> 12000) CWIDkeyDownTime=12000;
      displaySetting(settingNo);  
      }  
      
     if(settingNo==CWID)            //CW Ident string
     {
     int c;
     c= morseIdent[setIndex];
     c=c+mouseScroll;
     if(mouseScroll>0)
       {
         if(c>95) c=47;
         if((c>57)&&(c<65)) c=65;
         if(c>90) c=95;
       }
     if(mouseScroll<0)
       {
          if(c<47) c=95;
          if((c>90)&&(c<95)) c=90;
          if((c>57)&&(c<65)) c=57;
       }
     
     morseIdent[setIndex] = c;
     mouseScroll=0;
     displaySetting(settingNo); 
     }  
     
  if(settingNo==BREAK_IN_TIME)        // CW Break In Timer
      {
      breakInTime=breakInTime+mouseScroll;
      mouseScroll=0;
      if(breakInTime< 50) breakInTime=50;
      if(breakInTime> 200) breakInTime=200;
      displaySetting(settingNo);  
      }                                                                                                                     
}

               
int minGain(double freq)
{
double rxfreq;

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return -1;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return -3;
  }
if(rxfreq>=4000)
  {
  return -10;
  }
  
return 0;
}

int maxGain(double freq)
{
double rxfreq; 

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return 73;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return 71;
  }
if(rxfreq>=4000)
  {
  return 62;
  }
  
return 73;
}





void displaySetting(int se)
{
  char valStr[30];
  gotoXY(0,settingY);
  textSize=2;                                                            
  setForeColour(255,255,255);
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  gotoXY(settingX,settingY);
  if((se==TX_OFFSET)||(se==RX_OFFSET)||(se==CWID))
  {
  gotoXY(0,settingY);
  }
  displayStr(settingText[se]);
 
 if(se==SSB_MIC)
  {
  sprintf(valStr,"%d",SSBMic);
  displayStr(valStr);
  }
 if(se==FM_MIC)
  {
  sprintf(valStr,"%d",FMMic);
  displayStr(valStr);
  }
if(se==REP_SHIFT)
  {
  sprintf(valStr,"%.5f",bandRepShift[band]);
  displayStr(valStr);
  }
  if(se==RX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandRxOffset[band]);
  displayStr(valStr);
  displayStr(" Rx Freq= ");
  sprintf(valStr,"%.5f",freq+bandRxOffset[band]);
  displayStr(valStr);
  }
  if(se==RX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandRxHarmonic[band]);
  displayStr(valStr);
  } 
  if(se==TX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandTxOffset[band]);
  displayStr(valStr);
  displayStr(" Tx Freq= ");
  sprintf(valStr,"%.5f",freq+bandTxOffset[band]);
  displayStr(valStr);
  }
  if(se==TX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandTxHarmonic[band]);
  displayStr(valStr);
  }   
  if(se==BAND_BITS)
  { 
  maxSetIndex=7;
  setForeColour(255,255,255);
  for(int b=128;b>0;b=b>>1)
    {
      if(((setIndex==7)&&(b==1)) || ((setIndex==6)&&(b==2))  || ((setIndex==5)&&(b==4)) || ((setIndex==4)&&(b==8)) || ((setIndex==3)&&(b==16))  || ((setIndex==2)&&(b==32))  || ((setIndex==1)&&(b==64))  || ((setIndex==0)&&(b==128)))
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(bbits & b)
        {
        displayChar('1');
        }
      else
        {
        displayChar('0');
        }
    } 
  }
  
  if(se==FFT_REF)
  {
  sprintf(valStr,"%d",FFTRef);
  displayStr(valStr);
  }
  if(se==TX_ATT)
  {
  sprintf(valStr,"%d dB",TxAtt);
  displayStr(valStr);
  }
  if(se==RX_GAIN)
  {
    if(bandRxGain[band]>maxGain(freq))
    {
    sprintf(valStr,"Auto");
    }
    else
    {
    sprintf(valStr,"%d dB",bandRxGain[band]);
    }
  displayStr(valStr);
  }
  if(se==S_ZERO)
  {
  sprintf(valStr,"%.0f dB",bandSmeterZero[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_LOW)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltLow[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_HIGH)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltHigh[band]);
  displayStr(valStr);
  }
  if(se==CW_CARRIER)
  {
  sprintf(valStr,"%d Secs",CWIDkeyDownTime/100);
  displayStr(valStr);
  }
  if(se==CWID)
  {
    maxSetIndex=MORSEIDENTLENGTH-2;
   for(int c=0; c<MORSEIDENTLENGTH;c++)
    {
      if(setIndex==c)
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(morseIdent[c]>0)
        {
          if((morseIdent[c]==95) && (setIndex!=c))
            {
              displayChar(32);
            }
          else
            {
              displayChar(morseIdent[c]);
            }
        } 
      else
        {
          if(setIndex>=c)
            {
            setIndex=c;
            morseIdent[c]=95;
            setForeColour(0,255,0);
            displayChar(95);
            morseIdent[c+1]=0;          
            }
          break;
        }   
    }
  }
  
 if(se==BREAK_IN_TIME)
  {
  sprintf(valStr,"%d ms",breakInTime*10);
  displayStr(valStr);
  }  
  
}

int readConfig(void)
{
FILE * conffile;
char variable[50];
char value[100];
char vname[20];

conffile=fopen("/home/pi/Langstone/Langstone.conf","r");

if(conffile==NULL)
  {
    return -1;
  }

while(fscanf(conffile,"%49s %99s [^\n]\n",variable,value) !=EOF)
  {
     
    if(strstr(variable,"CW_IDENT"))
      {
       value[MORSEIDENTLENGTH-1]=0;           //force to maximum length if necessary
       strcpy(morseIdent,value);
      }
    if(strstr(variable,"CWID_KEY_DOWN_TIME"))
      {
        sscanf(value,"%d",&CWIDkeyDownTime);
        CWIDkeyDownTime=CWIDkeyDownTime*100;
      }

    
    for(int b=0;b<numband;b++)
    {
    sprintf(vname,"bandFreq%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandFreq[b]);
    sprintf(vname,"bandMode%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandMode[b]);   
    sprintf(vname,"bandTxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandTxOffset[b]); 
    sprintf(vname,"bandTxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxHarmonic[b]);             
    sprintf(vname,"bandRxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRxOffset[b]);      
    sprintf(vname,"bandRxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxHarmonic[b]);   
    sprintf(vname,"bandRepShift%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRepShift[b]);
    sprintf(vname,"bandDuplex%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandDuplex[b]);     
    sprintf(vname,"bandBits%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandBits[b]);     
    sprintf(vname,"bandFFTRef%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTRef[b]);     
    sprintf(vname,"bandSquelch%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b]);  
    sprintf(vname,"bandTxAtt%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxAtt[b]);
    sprintf(vname,"bandRxGain%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxGain[b]);    
    sprintf(vname,"bandSmeterZero%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%f",&bandSmeterZero[b]);
    sprintf(vname,"bandSSBFiltLow%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltLow[b]); 
    sprintf(vname,"bandSSBFiltHigh%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltHigh[b]);     
    }

    
    if(strstr(variable,"currentBand")) sscanf(value,"%d",&band);
    if(strstr(variable,"tuneDigit")) sscanf(value,"%d",&tuneDigit);   
    if(strstr(variable,"mode")) sscanf(value,"%d",&mode);
    if(strstr(variable,"SSBMic")) sscanf(value,"%d",&SSBMic);
    if(strstr(variable,"FMMic")) sscanf(value,"%d",&FMMic);
    if(strstr(variable,"volume")) sscanf(value,"%d",&volume);
    if(strstr(variable,"breakInTime")) sscanf(value,"%d",&breakInTime);
    if(mode>nummode-1) mode=0;
            
  }

fclose(conffile);
return 0;

}

int writeConfig(void)
{
FILE * conffile;
char variable[80];
int value;

bandFreq[band]=freq;

for(int i=0;i<MORSEIDENTLENGTH;i++)                                                 //trim morse Ident to remove redundant spaces.
  {
  if((morseIdent[i]==95) && (morseIdent[i+1]==95))                    //find double space
    {
      morseIdent[i]=0;                                                //terminate string here
      break;
    }
  }


conffile=fopen("/home/pi/Langstone/Langstone.conf","w");

if(conffile==NULL)
  {
    return -1;
  }

fprintf(conffile,"CW_IDENT %s\n",morseIdent);
fprintf(conffile,"CWID_KEY_DOWN_TIME %d\n",CWIDkeyDownTime/100);

for(int b=0;b<numband;b++)
{
  fprintf(conffile,"bandFreq%02d %lf\n",b,bandFreq[b]);
  fprintf(conffile,"bandMode%02d %d\n",b,bandMode[b]);
  fprintf(conffile,"bandTxOffSet%02d %lf\n",b,bandTxOffset[b]);
  fprintf(conffile,"bandTxHarmonic%02d %d\n",b,bandTxHarmonic[b]);
  fprintf(conffile,"bandRxOffSet%02d %lf\n",b,bandRxOffset[b]);
  fprintf(conffile,"bandRxHarmonic%02d %d\n",b,bandRxHarmonic[b]);
  fprintf(conffile,"bandRepShift%02d %lf\n",b,bandRepShift[b]);
  fprintf(conffile,"bandDuplex%02d %d\n",b,bandDuplex[b]);
  fprintf(conffile,"bandBits%02d %d\n",b,bandBits[b]);
  fprintf(conffile,"bandFFTRef%02d %d\n",b,bandFFTRef[b]);
  fprintf(conffile,"bandSquelch%02d %d\n",b,bandSquelch[b]);
  fprintf(conffile,"bandTxAtt%02d %d\n",b,bandTxAtt[b]);
  fprintf(conffile,"bandRxGain%02d %d\n",b,bandRxGain[b]);
  fprintf(conffile,"bandSmeterZero%02d %f\n",b,bandSmeterZero[b]);
  fprintf(conffile,"bandSSBFiltLow%02d %d\n",b,bandSSBFiltLow[b]);
  fprintf(conffile,"bandSSBFiltHigh%02d %d\n",b,bandSSBFiltHigh[b]);    
}

fprintf(conffile,"currentBand %d\n",band);
fprintf(conffile,"tuneDigit %d\n",tuneDigit);
fprintf(conffile,"mode %d\n",mode);
fprintf(conffile,"SSBMic %d\n",SSBMic);
fprintf(conffile,"FMMic %d\n",FMMic);
fprintf(conffile,"volume %d\n",volume);
fprintf(conffile,"breakInTime %d\n",breakInTime);

fclose(conffile);
return 0;

}


