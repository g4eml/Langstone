#include "Mouse.h"
#include "Touch.h"

#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "mcp23017.c"

const char *i2cNode1 = "/dev/i2c-1";        //linux i2c node for the Hyperpixel display on Pi 4
const char *i2cNode2 = "/dev/i2c-11";       //linux i2c node for the Hyperpixel display on Pi 4
int  mcp23017_addr = 0x20;	               //MCP23017 I2C Address
int p1;
int p2;
int k1;
int k2;
int lastp1;
int lastk1;
int lastp2;
int lastk2;

#define pttPin 0        // Wiring Pi pin number. Physical pin is 11
#define keyPin 1        //Wiring Pi pin number. Physical pin is 12
#define i2cPttPin 0     //MCP23017 PTT Input if fitted  Port A bit 0
#define i2cKeyPin 1     //MCP23017 Key Input if fitted  Port A bit 1

int mousePresent;
int touchPresent;
int hyperPixelPresent;
int MCP23017Present;
char mousePath[20];
char touchPath[20];
char i2cPath[20];

void detectHw();
void processTouch();
void processMouse(int mbut);
void initMCP23017(int add);
void initGPIO(void); 
void processGPIO(void);

int main()
{
  printf("\n\nStarting Langstone Hardware Test\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  detectHw();
  
  if(mousePresent) 
  {
  printf("Mouse detected on %s\n",mousePath);
  initMouse(mousePath);
  }
  else
  {
  printf("ERROR:- No Mouse Detected\n");
  }
  
  if(touchPresent)
  {
  printf("Touch Screen detected on %s\n",touchPath);
  initTouch(touchPath);
  } 
    else
  {
  printf("ERROR:- No Touch Screen Detected\n");
  }
  
   if(hyperPixelPresent)
  {
  printf("HyperPixel Screen Detected\n");
  initTouch(touchPath);
  } 
  
  if(MCP23017Present)
  {
   printf("MCP23017 Detected on i2c Bus %s\n",i2cPath);
  }
  
  initGPIO();
  
  printf("\n");
  if(mousePresent) printf("Looking for Mouse Scroll Wheel and Buttons\n");
  if(touchPresent) printf("Looking for Touches \n");
  if(MCP23017Present) printf("Looking for inputs from MCP23017 pins PA0 and PA1\n");
  if(hyperPixelPresent==0) printf("Looking for inputs from GPIO pins 11 (PTT) and 12 (KEY)\n");
  printf("\nTesting...... Enter <Control C> to exit\n\n");

   while(1)
    {
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
         }
      processGPIO();
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
  
    
// try to initialise MCP23017 i2c chip for additonal I/O
// this will set or reset MCP23017Present flag 
  initMCP23017(mcp23017_addr);
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

printf("Touch detected at X=%d Y=%d\n",touchX,touchY);
} 

void processMouse(int mbut)
{
  if(mbut==128)       //scroll whell turned 
    {
      
      if(mouseScroll>0)       printf("Scroll Wheel Clicked Clockwise\n");
      if(mouseScroll<0)       printf("Scroll Wheel Clicked Anticlockwise\n");
      mouseScroll=0;
    }
    
  if(mbut==1+128)      //Left Mouse Button down
    {
      printf("Left Mouse Button Pressed\n");    
    }
    
  if(mbut==2+128)      //Right Mouse Button down
    {
        printf("Right Mouse Button Pressed\n");         
    }
      
    
}
 
 
 // Configure the MCP23017 GPIO Extender if it is fitted :

void initMCP23017(int add) 
{
 int resp; 
 resp=i2c_init(i2cNode1);	               			//Initialize i2c node for a pi with normal display
sprintf(i2cPath,"%s",i2cNode1);
 if(resp<0)                                   //not found, try for pi with Hypepixel4 display
 {
  resp=i2c_init(i2cNode2);	               		//Initialize i2c node fo4r a pi with normal display
  sprintf(i2cPath,"%s",i2cNode2);
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

void initGPIO(void)
{
  if(hyperPixelPresent==0)
  {
  wiringPiSetup();
  pinMode(pttPin,INPUT);
  pinMode(keyPin,INPUT);
  }
}

void processGPIO(void)
{

if(hyperPixelPresent==0)              //can only use GPIO if Hyperpixel Display is not fitted. 
  {
    p1=digitalRead(pttPin);
    k1=digitalRead(keyPin);
    if(p1!=lastp1)
      {
      lastp1=p1;
      printf("GPIO Pin 11 (PTT) =%d\n",p1);
      }
    if(k1!=lastk1)
      {
      lastk1=k1;
      printf("GPIO Pin 12 (KEY) =%d\n",k1);
      }
  }
  
if(MCP23017Present==1)                //MCP23017 extender chip can be used with any display. 
  {
    p2=mcp23017_readbit(mcp23017_addr,GPIOA,0);
    k2=mcp23017_readbit(mcp23017_addr,GPIOA,1);
    
    if(p2!=lastp2)
      {
      lastp2=p2;
      printf("MCP23017 Pin PA0 (PTT) =%d\n",p2);
      }
    if(k2!=lastk2)
      {
      lastk2=k2;
      printf("MCP23017 Pin PA1 (KEY) =%d\n",k2);
      }
  }    
  
}
 