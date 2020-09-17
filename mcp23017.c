
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sys/ioctl.h>
#include "/usr/include/linux/i2c-dev.h"
#include "/usr/include/linux/i2c.h"


#define GPIOA	0
#define GPIOB	1
#define IODIR	0
#define IPOL	1
#define GPINTEN	2
#define DEFVAL	3
#define INTCON	4
#define IOCON	5
#define GPPU	6
#define INTF	7
#define INTCAP	8
#define GPIO	9
#define OLAT	10

#define MCP_REGISTER(r,g) (((r)<<1)|(g))    // For I2C routines


//#####################################################################
//##   I2C Access Functions                                          ##
//#####################################################################

static int i2c_fd = -1;			  //Device node: 
static unsigned long i2c_funcs = 0;	     //Support flags

// Write 8 bits to I2C bus peripheral:

int i2c_write8(int addr,int reg,int byte) 
{
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg iomsgs[1];
	char buf[2];
	int rc;

	buf[0] = (unsigned char) reg;	         // MCP23017 register no. 
	buf[1] = (unsigned char) byte;	       // Byte to write to register 

	iomsgs[0].addr = (unsigned) addr;
	iomsgs[0].flags = 0;		               // Write
	iomsgs[0].buf = buf;
	iomsgs[0].len = 2;

	msgset.msgs = iomsgs;
	msgset.nmsgs = 1;

	rc = ioctl(i2c_fd,I2C_RDWR,&msgset);
	return rc < 0 ? -1 : 0;
}


// Read 8-bit value from peripheral:

int i2c_read8(int addr,int reg) 
{
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg iomsgs[2];
	char buf[1], rbuf[1];
	int rc;

	buf[0] = (char) reg;

	iomsgs[0].addr = iomsgs[1].addr = (unsigned) addr;
	iomsgs[0].flags = 0;		                      // Write
	iomsgs[0].buf = buf;
	iomsgs[0].len = 1;

	iomsgs[1].flags = I2C_M_RD;	                  // Read
	iomsgs[1].buf = rbuf;
	iomsgs[1].len = 1;

	msgset.msgs = iomsgs;
	msgset.nmsgs = 2;

	rc = ioctl(i2c_fd,I2C_RDWR,&msgset);
	return rc < 0 ? -1 : ((int)(rbuf[0]) & 0x0FF);
}



// Open I2C bus and check capabilities :

int i2c_init(const char *node) 
{
	int rc;

	i2c_fd = open(node,O_RDWR);	
	if ( i2c_fd < 0 ) 
  {
   return -1;
	}          


// Make sure the driver supports plain I2C I/O:

	rc = ioctl(i2c_fd,I2C_FUNCS,&i2c_funcs);
	assert(rc >= 0);
	assert(i2c_funcs & I2C_FUNC_I2C);
  return 0;
}

/*
 * Close the I2C driver :
 */
void i2c_close(void) 
{
	close(i2c_fd);
	i2c_fd = -1;
}


//#####################################################################
//##      MCP23017 Functions                                         ##
//#####################################################################

// Write to MCP23017 A or B register:

int mcp23017_writereg(int add,int reg,int AB,int value) 
{
	int reg_addr = MCP_REGISTER(reg,AB);
	int rc;

	rc = i2c_write8(add,reg_addr,value);
	return rc;
}


// Read 8 bits from port

unsigned mcp23017_readport(int add,int AB) 
{
	int reg_addr = MCP_REGISTER(GPIO,AB);

	return i2c_read8(add,reg_addr);
}


// Write 8-bits port

void mcp23017_writeport(int add,int AB,int value) 
{
	int reg_addr = MCP_REGISTER(GPIO,AB);

	i2c_write8(add,reg_addr,value);
}

int mcp23017_readbit(int add,int AB, int bit)
{
 int val;
 int mask;
 
  mask=1<<bit;
  
	int reg_addr = MCP_REGISTER(GPIO,AB);

	val=i2c_read8(add,reg_addr);
  
  if((val & mask) >0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


void mcp23017_writebit(int add,int AB,int bit,int value) 
{
int val;
int mask;

 mask=1<<bit;
 
	int reg_addr = MCP_REGISTER(GPIO,AB);
  val= i2c_read8(add,reg_addr);
  if(value==0)
  {
    val=val & (~mask);
  }
  else
  {
    val=val | mask;
  }
	i2c_write8(add,reg_addr,val);
}

   
