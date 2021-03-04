
##################################################
# Piped Commands Rx Control Thread for Hayling Transceiver
# Author: G4EML
# Needs to be manually added into Gnu Radio Flowgraph
##################################################

####################################################
#Add these imports to the top
#####################################################
 
import os
import errno

#######################################################
# Add these lines at the start of the Variables section
#######################################################

        plutoip=os.environ.get('PLUTO_IP')
        if plutoip==None :
          plutoip='pluto.local'
        plutoip='ip:' + plutoip
        
########################################################
# change the pluto source definition to  this
########################################################

        self.pluto_source_0 = iio.pluto_source(plutoip, 1000000000, 528000, 2000000, 0x800, True, True, True, "slow_attack", 64.0, '', True)

#######################################################
# Manually add just before the Main () Function
# to provide support for Piped commands
#######################################################
def docommands(tb):
  try:
    os.mkfifo("/tmp/langstoneRx")
  except OSError as oe:
    if oe.errno != errno.EEXIST:
      raise    
  ex=False
  lastbase=0
  while not ex:
    fifoin=open("/tmp/langstoneRx",'r')
    while True:
       try:
        with fifoin as filein:
         for line in filein:
           line=line.strip()
           if line=='Q':
              ex=True                  
           if line=='P':
              tb.set_FFTEn(1)
           if line=='p':
              tb.set_FFTEn(0)
           if line=='U':
              tb.set_Mute(1)
           if line=='u':
              tb.set_Mute(0)
           if line=='H':
              tb.lock()
           if line=='h':
              tb.unlock() 
           if line[0]=='O':
              value=int(line[1:])
              tb.set_RxOffset(value)  
           if line[0]=='V':
              value=int(line[1:])
              tb.set_AFGain(value)
           if line[0]=='S':
              value=int(line[1:])
              tb.set_SQL(value) 
           if line[0]=='F':
              value=int(line[1:])
              tb.set_Filt_High(value) 
           if line[0]=='f':
              value=int(line[1:])
              tb.set_Filt_Low(value) 
           if line[0]=='M':
              value=int(line[1:])
              tb.set_Mode(value) 
                                                     
       except:
         break

########################################################


#########################################################
#Replace the Main() function with this
########################################################
    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()
#########################################################
