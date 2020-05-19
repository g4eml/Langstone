#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Lang Rx
# Generated: Tue May 19 23:29:19 2020
##################################################

from gnuradio import analog
from gnuradio import audio
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import iio
from gnuradio.eng_option import eng_option
from gnuradio.fft import logpwrfft
from gnuradio.filter import firdes
from grc_gnuradio import blks2 as grc_blks2
from optparse import OptionParser
import os
import errno

class Lang_RX(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Lang Rx")

        ##################################################
        # Variables
        ##################################################
        self.SQL = SQL = 50
        self.RxOffset = RxOffset = 0
        self.Mute = Mute = False
        self.Mode = Mode = 3
        self.Filt_Low = Filt_Low = 300
        self.Filt_High = Filt_High = 3000
        self.FFTEn = FFTEn = 0
        self.AFGain = AFGain = 20

        ##################################################
        # Blocks
        ##################################################
        self.pluto_source_0 = iio.pluto_source('ip:pluto.local', 1000000000, 529200, 2000000, 0x800, True, True, True, "slow_attack", 64.0, '', True)
        self.logpwrfft_x_0 = logpwrfft.logpwrfft_c(
        	sample_rate=44100,
        	fft_size=512,
        	ref_scale=2,
        	frame_rate=15,
        	avg_alpha=0.9,
        	average=True,
        )
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(12, (firdes.low_pass(1,529200,20000,6000)), RxOffset, 529200)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_float*512)
        self.blocks_multiply_const_vxx_2_1 = blocks.multiply_const_vff((Mode==5, ))
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_vff((Mode==4, ))
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_vff((Mode<4, ))
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vff(((AFGain/100.0) *  (not Mute), ))
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_float*512, '/tmp/langstonefft', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.blocks_add_xx_1_0 = blocks.add_vff(1)
        self.blocks_add_xx_1 = blocks.add_vff(1)
        self.blks2_selector_0 = grc_blks2.selector(
        	item_size=gr.sizeof_float*512,
        	num_inputs=1,
        	num_outputs=2,
        	input_index=0,
        	output_index=FFTEn,
        )
        self.band_pass_filter_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 44100, Filt_Low, Filt_High, 100, firdes.WIN_HAMMING, 6.76))
        self.audio_sink_0 = audio.sink(44100, "hw:CARD=Device,DEV=0", False)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(SQL-100, 0.001, 0, False)
        self.analog_nbfm_rx_0 = analog.nbfm_rx(
        	audio_rate=44100,
        	quad_rate=44100,
        	tau=75e-6,
        	max_dev=5e3,
          )
        self.analog_agc2_xx_0 = analog.agc2_ff(1e-1, 1e-1, 0.1, 1)
        self.analog_agc2_xx_0.set_max_gain(1000)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc2_xx_0, 0), (self.blocks_add_xx_1, 0))
        self.connect((self.analog_nbfm_rx_0, 0), (self.blocks_multiply_const_vxx_2_0, 0))
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.analog_pwr_squelch_xx_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.blks2_selector_0, 1), (self.blocks_file_sink_0, 0))
        self.connect((self.blks2_selector_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.blocks_add_xx_1, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_add_xx_1_0, 0), (self.analog_agc2_xx_0, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.blocks_multiply_const_vxx_2_1, 0))
        self.connect((self.blocks_complex_to_real_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.audio_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.blocks_add_xx_1_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2_0, 0), (self.blocks_add_xx_1, 1))
        self.connect((self.blocks_multiply_const_vxx_2_1, 0), (self.blocks_add_xx_1_0, 1))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.logpwrfft_x_0, 0))
        self.connect((self.logpwrfft_x_0, 0), (self.blks2_selector_0, 0))
        self.connect((self.pluto_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))

    def get_SQL(self):
        return self.SQL

    def set_SQL(self, SQL):
        self.SQL = SQL
        self.analog_pwr_squelch_xx_0.set_threshold(self.SQL-100)

    def get_RxOffset(self):
        return self.RxOffset

    def set_RxOffset(self, RxOffset):
        self.RxOffset = RxOffset
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.RxOffset)

    def get_Mute(self):
        return self.Mute

    def set_Mute(self, Mute):
        self.Mute = Mute
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Mute), ))

    def get_Mode(self):
        return self.Mode

    def set_Mode(self, Mode):
        self.Mode = Mode
        self.blocks_multiply_const_vxx_2_1.set_k((self.Mode==5, ))
        self.blocks_multiply_const_vxx_2_0.set_k((self.Mode==4, ))
        self.blocks_multiply_const_vxx_2.set_k((self.Mode<4, ))

    def get_Filt_Low(self):
        return self.Filt_Low

    def set_Filt_Low(self, Filt_Low):
        self.Filt_Low = Filt_Low
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, self.Filt_Low, self.Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_Filt_High(self):
        return self.Filt_High

    def set_Filt_High(self, Filt_High):
        self.Filt_High = Filt_High
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, self.Filt_Low, self.Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_FFTEn(self):
        return self.FFTEn

    def set_FFTEn(self, FFTEn):
        self.FFTEn = FFTEn
        self.blks2_selector_0.set_output_index(int(self.FFTEn))

    def get_AFGain(self):
        return self.AFGain

    def set_AFGain(self, AFGain):
        self.AFGain = AFGain
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Mute), ))

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

def main(top_block_cls=Lang_RX, options=None):

    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
