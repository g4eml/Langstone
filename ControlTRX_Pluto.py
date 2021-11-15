
##################################################
# Piped Commands TRx Control Thread for Hayling Transceiver Pluto Version 
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
# change the pluto sink definition to  this
########################################################

        self.pluto_sink_0 = iio.pluto_sink(plutoip, 1000000000, 528000, 2000000, 0x800, False, 0, '', True)
        
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
    os.mkfifo("/tmp/langstoneTRx")
  except OSError as oe:
    if oe.errno != errno.EEXIST:
      raise    
  ex=False
  lastbase=0
  while not ex:
    fifoin=open("/tmp/langstoneTRx",'r')
    while True:
       try:
        with fifoin as filein:
         for line in filein:
           line=line.strip()
           if line[0]=='Q':
              ex=True                  
           if line[0]=='U':
              value=int(line[1:])
              tb.set_Rx_Mute(value)
           if line[0]=='H':
              value=int(line[1:])
              if value==1:   
                  tb.lock()
              if value==0:
                  tb.unlock() 
           if line[0]=='O':
              value=int(line[1:])
              tb.set_RxOffset(value)  
           if line[0]=='V':
              value=int(line[1:])
              tb.set_AFGain(value)
           if line[0]=='L':
              value=int(line[1:])
              tb.set_Rx_LO(value)
           if line[0]=='A':
              value=int(line[1:])
              tb.set_Rx_Gain(value)
           if line[0]=='S':
              value=int(line[1:])
              tb.set_SQL(value) 
           if line[0]=='F':
              value=int(line[1:])
              tb.set_Rx_Filt_High(value) 
           if line[0]=='I':
              value=int(line[1:])
              tb.set_Rx_Filt_Low(value) 
           if line[0]=='M':
              value=int(line[1:])
              tb.set_Rx_Mode(value) 
              tb.set_Tx_Mode(value)
           if line=='R':
              tb.set_PTT(False) 
           if line=='T':
              tb.set_PTT(True)
           if line[0]=='K':
              value=int(line[1:])
              tb.set_KEY(value) 
           if line[0]=='B':
              value=int(line[1:])
              tb.set_ToneBurst(value) 
           if line[0]=='G':
              value=int(line[1:])
              tb.set_MicGain(value) 
           if line[0]=='g':
              value=int(line[1:])
              tb.set_FMMIC(value)
           if line[0]=='f':
              value=int(line[1:])
              tb.set_Tx_Filt_High(value) 
           if line[0]=='i':
              value=int(line[1:])
              tb.set_Tx_Filt_Low(value)     
           if line[0]=='l':
              value=int(line[1:])
              tb.set_Tx_LO(value)  
           if line[0]=='a':
              value=int(line[1:])
              tb.set_Tx_Gain(value)       
                                                                 
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
