#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Ssb Trx
# Generated: Tue Nov 12 22:23:54 2019
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

class SSB_TRX(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Ssb Trx")

        ##################################################
        # Variables
        ##################################################
        self.BaseFreq = BaseFreq = 1296250000
        self.USB = USB = True
        self.TxOffset = TxOffset = 0
        self.TxLO = TxLO = BaseFreq-10000
        self.SQL = SQL = 20
        self.RxOffset = RxOffset = 0
        self.PTT = PTT = False
        self.NCW = NCW = False
        self.MicGain = MicGain = 5.0
        self.MON = MON = False
        self.KEY = KEY = False
        self.FMMIC = FMMIC = 50
        self.FM = FM = True
        self.CW = CW = False
        self.AFGain = AFGain = 20

        ##################################################
        # Blocks
        ##################################################
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=12,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.pluto_source_0 = iio.pluto_source('ip:192.168.2.1', BaseFreq, 529200, 2000000, 0x800, True, True, True, "slow_attack", 64.0, '', True)
        self.pluto_sink_0 = iio.pluto_sink('ip:192.168.2.1', TxLO, 529200, 2000000, 0x800, False, 0, '', True)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(12, (firdes.low_pass(1,529200,12000,6000)), RxOffset, 529200)
        self.blocks_mute_xx_0_0 = blocks.mute_cc(bool(not PTT))
        self.blocks_mute_xx_0 = blocks.mute_ff(bool(PTT and (not MON)))
        self.blocks_multiply_xx_2 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_1 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_vcc((not FM, ))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_vcc((FM, ))
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_vff((int(FM) *0.1, ))
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_vff((not FM, ))
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vff((AFGain/10.0, ))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff((FMMIC/10.0, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(((MicGain/10.0)*(not CW), ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_1 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.band_pass_filter_1 = filter.fir_filter_fff(1, firdes.band_pass(
        	1, 44100, 200, 3000, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccf(1, firdes.band_pass(
        	1, 44100, -3000+USB*3300+10000, -300+USB*3300+10000, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 44100, -3000+USB*3300+NCW*CW*250, -300+USB*3300-NCW*CW*1950, 100, firdes.WIN_HAMMING, 6.76))
        self.audio_source_0 = audio.source(44100, "hw:CARD=Device,DEV=0", True)
        self.audio_sink_0 = audio.sink(44100, "hw:CARD=Device,DEV=0", True)
        self.analog_simple_squelch_cc_0 = analog.simple_squelch_cc(-60+SQL, 1)
        self.analog_sig_source_x_1_0 = analog.sig_source_f(44100, analog.GR_COS_WAVE, 800, int(CW and KEY), 0)
        self.analog_sig_source_x_1 = analog.sig_source_c(529200, analog.GR_COS_WAVE, TxOffset, 1, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(44100, analog.GR_COS_WAVE, 10000, 1, 0)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=44100,
        	quad_rate=44100,
        	tau=75e-6,
        	max_dev=5e3,
        	fh=-1,
                )
        self.analog_nbfm_rx_0 = analog.nbfm_rx(
        	audio_rate=44100,
        	quad_rate=44100,
        	tau=75e-6,
        	max_dev=5e3,
          )
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.analog_nbfm_rx_0, 0), (self.blocks_multiply_const_vxx_2_0, 0))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_xx_2, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_2, 1))
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_multiply_xx_1, 1))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.analog_simple_squelch_cc_0, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_1, 0), (self.blocks_mute_xx_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_complex_to_real_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.band_pass_filter_1, 0))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.audio_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.blocks_add_xx_1, 0))
        self.connect((self.blocks_multiply_const_vxx_2_0, 0), (self.blocks_add_xx_1, 1))
        self.connect((self.blocks_multiply_const_vxx_3, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_4, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.band_pass_filter_0_0, 0))
        self.connect((self.blocks_multiply_xx_1, 0), (self.blocks_mute_xx_0_0, 0))
        self.connect((self.blocks_multiply_xx_2, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.blocks_mute_xx_0, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_mute_xx_0_0, 0), (self.pluto_sink_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.analog_simple_squelch_cc_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.pluto_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_multiply_xx_1, 0))

    def get_BaseFreq(self):
        return self.BaseFreq

    def set_BaseFreq(self, BaseFreq):
        self.BaseFreq = BaseFreq
        self.set_TxLO(self.BaseFreq-10000)
        self.pluto_source_0.set_params(self.BaseFreq, 529200, 2000000, True, True, True, "slow_attack", 64.0, '', True)

    def get_USB(self):
        return self.USB

    def set_USB(self, USB):
        self.USB = USB
        self.band_pass_filter_0_0.set_taps(firdes.band_pass(1, 44100, -3000+self.USB*3300+10000, -300+self.USB*3300+10000, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300+self.NCW*self.CW*250, -300+self.USB*3300-self.NCW*self.CW*1950, 100, firdes.WIN_HAMMING, 6.76))

    def get_TxOffset(self):
        return self.TxOffset

    def set_TxOffset(self, TxOffset):
        self.TxOffset = TxOffset
        self.analog_sig_source_x_1.set_frequency(self.TxOffset)

    def get_TxLO(self):
        return self.TxLO

    def set_TxLO(self, TxLO):
        self.TxLO = TxLO
        self.pluto_sink_0.set_params(self.TxLO, 529200, 2000000, 0, '', True)

    def get_SQL(self):
        return self.SQL

    def set_SQL(self, SQL):
        self.SQL = SQL
        self.analog_simple_squelch_cc_0.set_threshold(-60+self.SQL)

    def get_RxOffset(self):
        return self.RxOffset

    def set_RxOffset(self, RxOffset):
        self.RxOffset = RxOffset
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.RxOffset)

    def get_PTT(self):
        return self.PTT

    def set_PTT(self, PTT):
        self.PTT = PTT
        self.blocks_mute_xx_0_0.set_mute(bool(not self.PTT))
        self.blocks_mute_xx_0.set_mute(bool(self.PTT and (not self.MON)))

    def get_NCW(self):
        return self.NCW

    def set_NCW(self, NCW):
        self.NCW = NCW
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300+self.NCW*self.CW*250, -300+self.USB*3300-self.NCW*self.CW*1950, 100, firdes.WIN_HAMMING, 6.76))

    def get_MicGain(self):
        return self.MicGain

    def set_MicGain(self, MicGain):
        self.MicGain = MicGain
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not self.CW), ))

    def get_MON(self):
        return self.MON

    def set_MON(self, MON):
        self.MON = MON
        self.blocks_mute_xx_0.set_mute(bool(self.PTT and (not self.MON)))

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
        self.blocks_multiply_const_vxx_2_0.set_k((int(self.FM) *0.1, ))
        self.blocks_multiply_const_vxx_2.set_k((not self.FM, ))

    def get_CW(self):
        return self.CW

    def set_CW(self, CW):
        self.CW = CW
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not self.CW), ))
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300+self.NCW*self.CW*250, -300+self.USB*3300-self.NCW*self.CW*1950, 100, firdes.WIN_HAMMING, 6.76))
        self.analog_sig_source_x_1_0.set_amplitude(int(self.CW and self.KEY))

    def get_AFGain(self):
        return self.AFGain

    def set_AFGain(self, AFGain):
        self.AFGain = AFGain
        self.blocks_multiply_const_vxx_1.set_k((self.AFGain/10.0, ))

def docommands(tb):
  try:
    os.mkfifo("/tmp/langstonein")
  except OSError as oe:
    if oe.errno != errno.EEXIST:
      raise    
  ex=False
  lastbase=0
  while not ex:
    fifoin=open("/tmp/langstonein",'r')
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
           if line=='M':
              tb.set_MON(True) 
           if line=='m':
              tb.set_MON(False)                                                 
           if line[0]=='O':
              value=int(line[1:])
              tb.set_RxOffset(value)
           if line[0]=='o':
              value=int(line[1:])
              tb.set_TxOffset(value)  
           if line[0]=='V':
              value=int(line[1:])
              tb.set_AFGain(value)
           if line[0]=='G':
              value=int(line[1:])
              tb.set_MicGain(value) 
           if line[0]=='g':
              value=int(line[1:])
              tb.set_FMMIC(value)   
           if line[0]=='Z':
              value=int(line[1:])
              tb.set_SQL(value)                    
       except:
         break

def main(top_block_cls=SSB_TRX, options=None):

    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()

if __name__ == '__main__':
    main()
