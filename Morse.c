
// Morse code routines


unsigned long morseLetters[26] = {0x001D,0x0157,0x05D7,0x0057,0x0001,0x0175,0x0177,0x0055,0x0005,0x1DDD,0x01D7,0x015D,0x0077,0x0017,0x0777,0x05DD,0x1D77,0x005D,0x0015,0x0007,0x0075,0x01D5,0x01DD,0x0757,0x1DD7,0x0577};
int morseLetterLength[26] = {5,9,11,7,1,9,9,7,3,13,9,9,7,5,11,11,13,7,5,3,7,9,9,11,13,11};
unsigned long morseNumbers[10] = {0x77777,0x1DDDD,0x7775,0x1DD5,0x0755,0x0155,0x0557,0x1577,0x5777,0x17777};
int morseNumberLength[10] = {19,17,15,13,11,9,11,13,15,17};
int morseCharIndex =0;           //pointer to the char in the Ident string
int morseBitIndex =0;            //pointer to the bit in the current morse char
int morseCharLength =0;          //length of current character in bits.
int morseBitTimer =0;            //bit timer to control speed of morse character
int morseBitInterval =5;        //number of ticks per bit. (sets the morse speed)
long morseShiftReg =0;           // 
int morseLastRet=0;

#define MORSEIDENTLENGTH 41

char morseIdent[MORSEIDENTLENGTH] = "TEST_DE_LANGSTONE";          //Ident string terminated with zero

void morseEncode(int ch,long * bits,int * count);         

//encode ascii character ch into a bit pattern. Return the result as an unsigned long bit pattern starting at bit 0 and a bit count 
void morseEncode(int ch, long * bits,int * count)
{
   if((ch==32) || (ch==95))        //space or underscore
     {
      *bits=0;
      *count=7;                                
      return;
     }
    
    if(ch==47)        //   slash
      {
       *bits=0x1757;
       *count=13+3;                       //add 3 for inter character gap
       return;
      }
      
     if((ch>47) && (ch<58))       //numbers
       {
       *bits=morseNumbers[ch-48];
       *count=morseNumberLength[ch-48]+3;       //add 3 for inter character gap
       return;
       }

     if(ch>96)                    //lower case
       {
         ch=ch-32;
       }

     if((ch>64)&&(ch<91))           //Letters
       {
         *bits=morseLetters[ch-65];
         *count=morseLetterLength[ch-65]+3;           //add 3 for inter character gap
         return;
       }

      *bits=0;                 //nothing found so return null
      *count=0;
      return;
}

// when regularly called at a rate of 100 per second this routine returns 1 or 0 to operate the key or -1 if the Ident string is complete

int morseKey(void)
{
int ret;
 if(morseBitTimer++ >= morseBitInterval)
  {
   morseBitTimer=0;
   ret = morseShiftReg & 1;                     //get next bit
   morseLastRet=ret;                    
   morseShiftReg=morseShiftReg >> 1;            //shift one bit right
   if(morseBitIndex++ >= morseCharLength)       //finished this character, get ready for next time.
    {
    morseBitIndex=0;
    morseCharIndex++;
    if(morseIdent[morseCharIndex] == 0)           //end of ident string
      {
      morseCharIndex=0;                           //reset to start of string
      ret=-1;                                     //and signal to calling program
      }
    morseEncode(morseIdent[morseCharIndex], & morseShiftReg, & morseCharLength);  
    }
   return ret;
  }
 else
 {
   return morseLastRet;
 } 
}



void morseReset(void)
{
morseCharIndex=0;                                                   //zero the index pointers
morseBitIndex=0;
morseBitTimer=0;
morseLastRet=0;
morseEncode(morseIdent[0], & morseShiftReg, & morseCharLength);     //Load the first character
}
