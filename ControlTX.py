
##################################################
# Piped Commands Tx Control Thread for Hayling Transceiver
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

#######################################################
# Manually add just before the Main () Function
# to provide support for Piped commands
#######################################################
def docommands(tb):
  try:
    os.mkfifo("/tmp/langstoneTx")
  except OSError as oe:
    if oe.errno != errno.EEXIST:
      raise    
  ex=False
  lastbase=0
  while not ex:
    fifoin=open("/tmp/langstoneTx",'r')
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
           if line=='R':
              tb.set_PTT(False) 
           if line=='T':
              tb.set_PTT(True)
           if line=='K':
              tb.set_KEY(True) 
           if line=='k':
              tb.set_KEY(False) 
           if line=='A':
              tb.set_ToneBurst(True) 
           if line=='a':
              tb.set_ToneBurst(False) 
           if line=='H':
              tb.lock()
           if line=='h':
              tb.unlock()    
           if line[0]=='G':
              value=int(line[1:])
              tb.set_MicGain(value) 
           if line[0]=='g':
              value=int(line[1:])
              tb.set_FMMIC(value)
           if line[0]=='F':
              value=int(line[1:])
              tb.set_Filt_High(value) 
           if line[0]=='f':
              value=int(line[1:])
              tb.set_Filt_Low(value) 
           if line[0]=='M':
              value=int(line[1:])
              tb.set_Mode(value)   
           if line[0]=='C':
              value=int(line[1:])
              tb.set_CTCSS(value)                    
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
