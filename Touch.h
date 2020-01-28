#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <signal.h>
#include <linux/input.h>

int tfd;
int touchX=0;
int touchY=0;

int touchAvailable();
int getTouch();
int initTouch(char * tpath);


int initTouch(char * tpath)
{
        if ((tfd = open(tpath, O_RDONLY)) < 0) {
                return 1;
        }
}

//Returns 0 if no touch available. 1 if Touch start. 2 if touch end. 3 if touch move

int getTouch()
{
	int i;
  size_t rb;
  struct input_event ev[64];
  int retval;
  
  retval=0;
  if(touchAvailable())
    {
    rb=read(tfd,ev,sizeof(struct input_event)*64);
        for (i = 0;  i <  (rb / sizeof(struct input_event)); i++)
        {
              if (ev[i].type ==  EV_SYN) 
                {
                if(retval==0) retval=3;
                break;  //action
                }
              else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 1) retval=1; //touch start
              else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 0) retval=2; //touch finish
              else if (ev[i].type == EV_ABS && ev[i].code == 0 && ev[i].value > 0)
              {
			          touchX = ev[i].value;
		          }
                else if (ev[i].type == EV_ABS  && ev[i].code == 1 && ev[i].value > 0)
                {
			            touchY = ev[i].value;
		            }

	       }

    }

 return retval;
}


int touchAvailable()  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(tfd, &fds);
  select(tfd+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(tfd, &fds));
}