#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Lang Tx
# Generated: Tue May 12 20:49:16 2020
##################################################

from gnuradio import analog
from gnuradio import audio
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import iio
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import os
import errno

class Lang_TX(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Lang Tx")

        ##################################################
        # Variables
        ##################################################
        self.USB = USB = True
        self.PTT = PTT = False
        self.NCW = NCW = False
        self.MicGain = MicGain = 5.0
        self.KEY = KEY = False
        self.FMMIC = FMMIC = 50
        self.FM = FM = False
        self.CW = CW = False

        ##################################################
        # Blocks
        ##################################################
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=12,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.pluto_sink_0 = iio.pluto_sink('ip:pluto.local', 1000000000, 529200, 2000000, 0x800, False, 0, '', True)
        self.blocks_mute_xx_0_0 = blocks.mute_cc(bool(not PTT))
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_vcc((not FM, ))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_vcc((FM, ))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff((FMMIC/10.0, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(((MicGain/10.0)*(not CW), ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.band_pass_filter_1 = filter.fir_filter_fff(1, firdes.band_pass(
        	1, 44100, 200, 3000, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 44100, -3000+USB*3300+CW*250, -300+USB*3300-CW*1950, 100, firdes.WIN_HAMMING, 6.76))
        self.audio_source_0 = audio.source(44100, "hw:CARD=Device,DEV=0", False)
        self.analog_sig_source_x_1_0 = analog.sig_source_f(44100, analog.GR_COS_WAVE, 800, int(CW and KEY), 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(44100, analog.GR_COS_WAVE, 0, 1, 0)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=44100,
        	quad_rate=44100,
        	tau=75e-6,
        	max_dev=3000,
        	fh=-1,
                )
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.analog_agc2_xx_1 = analog.agc2_cc(1e-1, 1e-1, 1.3, 1.0)
        self.analog_agc2_xx_1.set_max_gain(10)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc2_xx_1, 0), (self.band_pass_filter_0_0, 0))
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.band_pass_filter_1, 0))
        self.connect((self.blocks_multiply_const_vxx_3, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_4, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.analog_agc2_xx_1, 0))
        self.connect((self.blocks_mute_xx_0_0, 0), (self.pluto_sink_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_mute_xx_0_0, 0))

    def get_USB(self):
        return self.USB

    def set_USB(self, USB):
        self.USB = USB
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300+self.CW*250, -300+self.USB*3300-self.CW*1950, 100, firdes.WIN_HAMMING, 6.76))

    def get_PTT(self):
        return self.PTT

    def set_PTT(self, PTT):
        self.PTT = PTT
        self.blocks_mute_xx_0_0.set_mute(bool(not self.PTT))

    def get_NCW(self):
        return self.NCW

    def set_NCW(self, NCW):
        self.NCW = NCW

    def get_MicGain(self):
        return self.MicGain

    def set_MicGain(self, MicGain):
        self.MicGain = MicGain
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not self.CW), ))

    def get_KEY(self):
        return self.KEY

    def set_KEY(self, KEY):
        self.KEY = KEY
        self.analog_sig_source_x_1_0.set_amplitude(int(self.CW and self.KEY))

    def get_FMMIC(self):
        return self.FMMIC

    def set_FMMIC(self, FMMIC):
        self.FMMIC = FMMIC
        self.blocks_multiply_const_vxx_0_0.set_k((self.FMMIC/10.0, ))

    def get_FM(self):
        return self.FM

    def set_FM(self, FM):
        self.FM = FM
        self.blocks_multiply_const_vxx_4.set_k((not self.FM, ))
        self.blocks_multiply_const_vxx_3.set_k((self.FM, ))

    def get_CW(self):
        return self.CW

    def set_CW(self, CW):
        self.CW = CW
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not self.CW), ))
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300+self.CW*250, -300+self.USB*3300-self.CW*1950, 100, firdes.WIN_HAMMING, 6.76))
        self.analog_sig_source_x_1_0.set_amplitude(int(self.CW and self.KEY))

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
           if line=='R':
              tb.set_PTT(False) 
           if line=='T':
              tb.set_PTT(True)
           if line=='U':
              tb.set_USB(True)
              tb.set_FM(False)
              tb.set_CW(False)              
           if line=='L':
              tb.set_USB(False)
              tb.set_FM(False)
              tb.set_CW(False) 
           if line=='F':
              tb.set_FM(True)             
           if line=='C':
              tb.set_CW(True)
              tb.set_FM(False)
              tb.set_USB(True) 
           if line=='N':
              tb.set_NCW(True)
           if line=='W':
              tb.set_NCW(False) 
           if line=='K':
              tb.set_KEY(True) 
           if line=='k':
              tb.set_KEY(False)    
           if line[0]=='G':
              value=int(line[1:])
              tb.set_MicGain(value) 
           if line[0]=='g':
              value=int(line[1:])
              tb.set_FMMIC(value) 
           if line=='H':
              tb.lock()
           if line=='h':
              tb.unlock()                     
       except:
         break


def main(top_block_cls=Lang_TX, options=None):

    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
