#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <signal.h>
#include <linux/input.h>

int mfd;
int mouseScroll;

int mouseAvailable();
int getMouse();
int initMouse(char * mpath);


int initMouse(char * mpath)
{
        if ((mfd = open(mpath, O_RDONLY)) < 0) 
        {
                return 1;
        }

}

//Returns 0 if no mouse event available. 1=Left Button up, 2=Right Button up 3=Centre Button up 4=Side Button up 5=extra button up 128=scroll wheel move (and updates mouseScroll value)  Add 128 to value for button down event

int getMouse()
{
	int i;
  size_t rb;
  struct input_event ev[64];
  int retval;
  
  retval=0;
  if (mouseAvailable())
  {
    rb=read(mfd,ev,sizeof(struct input_event)*64);
    for (i = 0;  i <  (rb / sizeof(struct input_event)); i++)
    {
      if((ev[i].type==1) & (ev[i].code==272)) retval=1+(128*ev[i].value);   //left button
      if((ev[i].type==1) & (ev[i].code==273)) retval=2+(128*ev[i].value);   //right button 
      if((ev[i].type==1) & (ev[i].code==274)) retval=3+(128*ev[i].value);   //Center button
      if((ev[i].type==1) & (ev[i].code==275)) retval=4+(128*ev[i].value);   //Side button
      if((ev[i].type==1) & (ev[i].code==276)) retval=5+(128*ev[i].value);   //Extra button  
      if((ev[i].type==2) & (ev[i].code==8))                                 //Scroll wheel
        {
        mouseScroll=mouseScroll+ev[i].value;
        retval=128;
        }
    }
  
  }
 return retval;   
}


int mouseAvailable()  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(mfd, &fds);
  select(mfd+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(mfd, &fds));
}