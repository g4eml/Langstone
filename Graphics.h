#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "Font.h"

char *fbp = 0;
int fbfd = 0;
long int screenSize = 0;
int screenXsize=0;
int screenYsize=0;
int currentX=0;
int currentY=0;
int textSize=1;
int foreColourR=0;
int foreColourG=0;
int foreColourB=0;
int backColourR=0;
int backColourG=0;
int backColourB=0;


void closeScreen(void);
int initScreen(void);
void setPixel(int x, int y, int R, int G, int B);
void clearScreen();
void displayChar(int ch);
void setLargePixel(int x, int y, int size, int R, int G, int B);
void gotoXY(int x, int y);
void setForeColour(int R,int G,int B);
void setBackColour(int R,int G,int B);
void displayStr(char*s);
void displayButton(char*s);

void displayStr(char*s)
{
 int p;
 p=0;
 do
 {
    displayChar(s[p++]);
 }
 while(s[p]!=0);
}

void displayChar(int ch)
{
  int row;
  int col;
  int pix;

  for(row=0;row<9;row++)
    {
    pix=font[ch][row];
    for(col=0;col<8;col++)
      {
       if((pix << col) & 0x80)
         {
            setLargePixel(currentX+col*textSize,currentY+row*textSize,textSize,foreColourR,foreColourG,foreColourB);
         }
       else
         { 
            setLargePixel(currentX+col*textSize,currentY+row*textSize,textSize,backColourR,backColourG,backColourG);
         }
      }
    }

  currentX=currentX+8*textSize;
}


void displayButton(char*s)
{
int saveX=currentX;
int saveY=currentY;
for(int x=0;x<100;x++)
  {
    setPixel(currentX+x,currentY,foreColourR,foreColourG,foreColourB);
    setPixel(currentX+x,currentY+50,foreColourR,foreColourG,foreColourB);
  }
for(int y=0;y<50;y++)
  {
    setPixel(currentX,currentY+y,foreColourR,foreColourG,foreColourB);
    setPixel(currentX+100,currentY+y,foreColourR,foreColourG,foreColourB);
  }

gotoXY(currentX+1,currentY+18);
textSize=2;
displayStr("      ");
currentX=saveX;
int sx=50-((strlen(s)*16)/2);  
gotoXY(currentX+sx,currentY);
textSize=2;
displayStr(s);
currentX=saveX+105;
currentY=saveY;

}


void gotoXY(int x, int y)
{
currentX=x;
currentY=y;
}

void setForeColour(int R,int G,int B)
{
foreColourR=R;
foreColourG=G;
foreColourB=B;
}

void setBackColour(int R,int G,int B)
{
backColourR=R;
backColourG=G;
backColourB=B;
}

void clearScreen()
{
  for(int y=0;y<screenYsize;y++)
	  {
      for(int x=0;x<screenXsize;x++)
        {
        setPixel(x,y,backColourR,backColourG,backColourB);
        }
  	}
}

void setPixel(int x, int y, int R, int G, int B)
{
if((x<800)&(y<480))
  {
  int p=(x+screenXsize*y)*4;
  	memset(fbp+p,B,1);  //Blue
  	memset(fbp+p+1,G,1);  //Green
  	memset(fbp+p+2,R,1);  //Red
  	memset(fbp+p+3,0x80,1);  //A
  }

}

void setLargePixel(int x, int y, int size, int R, int G, int B)
{
  for (int px=0;px<size;px++)
    {
      for(int py=0;py<size;py++)
        {
        setPixel(x+px,y+py,R,G,B);
        }
    }
}

void closeScreen(void)
{
  munmap(fbp, screenSize);
  close(fbfd);
}


int initScreen(void)
{

  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
 
  fbfd = open("/dev/fb0", O_RDWR);
  if (!fbfd) 
  {
    printf("Error: cannot open framebuffer device.\n");
    return(0);
  }

  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) 
  {
    printf("Error reading fixed information.\n");
    return(0);
  }



  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) 
  {
    printf("Error reading variable information.\n");
    return(0);
  }
  
  screenXsize=vinfo.xres;
  screenYsize=vinfo.yres;
  
  screenSize = finfo.smem_len;
  fbp = (char*)mmap(0, screenSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
                    
   if ((int)fbp == -1) 
   {
    return 0;
   }
  else 
   {
    return 1;
   }
}