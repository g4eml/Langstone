#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Ssb Trx
# Generated: Mon May  4 17:28:39 2020
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

class SSB_TRX(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Ssb Trx")

        ##################################################
        # Variables
        ##################################################
        self.USB = USB = True
        self.SQL = SQL = 50
        self.RxOffset = RxOffset = 0
        self.PTT = PTT = False
        self.NCW = NCW = False
        self.MicGain = MicGain = 5.0
        self.MON = MON = False
        self.KEY = KEY = False
        self.FMMIC = FMMIC = 50
        self.FM = FM = False
        self.FFTEn = FFTEn = 0
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
        self.pluto_source_0 = iio.pluto_source('ip:192.168.2.1', 1000000000, 529200, 2000000, 0x800, True, True, True, "slow_attack", 64.0, '', True)
        self.pluto_sink_0 = iio.pluto_sink('ip:192.168.2.1', 1000000000, 529200, 2000000, 0x800, False, 0, '', True)
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
        self.blocks_mute_xx_0_0 = blocks.mute_cc(bool(not PTT))
        self.blocks_mute_xx_0 = blocks.mute_ff(bool(PTT and (not MON)))
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_vcc((not FM, ))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_vcc((FM, ))
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_vff((int(FM) *0.01, ))
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_vff((not FM, ))
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vff((AFGain, ))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff((FMMIC/10.0, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(((MicGain/10.0)*(not CW), ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_float*512, '/tmp/langstonefft', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_1 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.blks2_selector_0 = grc_blks2.selector(
        	item_size=gr.sizeof_float*512,
        	num_inputs=1,
        	num_outputs=2,
        	input_index=0,
        	output_index=FFTEn,
        )
        self.band_pass_filter_1 = filter.fir_filter_fff(1, firdes.band_pass(
        	1, 44100, 200, 3000, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 44100, -3000+USB*3300, -300+USB*3300, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 44100, ((-3000+USB*3300+NCW*CW*250)*(1-FM)) + (-7500 * FM), ((-300+USB*3300-NCW*CW*1950)* (1-FM)) + (7500 * FM), 100, firdes.WIN_HAMMING, 6.76))
        self.audio_source_0 = audio.source(44100, "hw:CARD=Device,DEV=0", True)
        self.audio_sink_0 = audio.sink(44100, "hw:CARD=Device,DEV=0", True)
        self.analog_sig_source_x_1_0 = analog.sig_source_f(44100, analog.GR_COS_WAVE, 800, int(CW and KEY), 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(44100, analog.GR_COS_WAVE, 0, 1, 0)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(SQL-100, 0.001, 0, False)
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
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.analog_pwr_squelch_xx_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blks2_selector_0, 1), (self.blocks_file_sink_0, 0))
        self.connect((self.blks2_selector_0, 0), (self.blocks_null_sink_0, 0))
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
        self.connect((self.blocks_mute_xx_0, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_mute_xx_0_0, 0), (self.pluto_sink_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.logpwrfft_x_0, 0))
        self.connect((self.logpwrfft_x_0, 0), (self.blks2_selector_0, 0))
        self.connect((self.pluto_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_mute_xx_0_0, 0))

    def get_USB(self):
        return self.USB

    def set_USB(self, USB):
        self.USB = USB
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 44100, -3000+self.USB*3300, -300+self.USB*3300, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, ((-3000+self.USB*3300+self.NCW*self.CW*250)*(1-self.FM)) + (-7500 * self.FM), ((-300+self.USB*3300-self.NCW*self.CW*1950)* (1-self.FM)) + (7500 * self.FM), 100, firdes.WIN_HAMMING, 6.76))

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
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, ((-3000+self.USB*3300+self.NCW*self.CW*250)*(1-self.FM)) + (-7500 * self.FM), ((-300+self.USB*3300-self.NCW*self.CW*1950)* (1-self.FM)) + (7500 * self.FM), 100, firdes.WIN_HAMMING, 6.76))

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
        self.blocks_multiply_const_vxx_2_0.set_k((int(self.FM) *0.01, ))
        self.blocks_multiply_const_vxx_2.set_k((not self.FM, ))
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, ((-3000+self.USB*3300+self.NCW*self.CW*250)*(1-self.FM)) + (-7500 * self.FM), ((-300+self.USB*3300-self.NCW*self.CW*1950)* (1-self.FM)) + (7500 * self.FM), 100, firdes.WIN_HAMMING, 6.76))

    def get_FFTEn(self):
        return self.FFTEn

    def set_FFTEn(self, FFTEn):
        self.FFTEn = FFTEn
        self.blks2_selector_0.set_output_index(int(self.FFTEn))

    def get_CW(self):
        return self.CW

    def set_CW(self, CW):
        self.CW = CW
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not self.CW), ))
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 44100, ((-3000+self.USB*3300+self.NCW*self.CW*250)*(1-self.FM)) + (-7500 * self.FM), ((-300+self.USB*3300-self.NCW*self.CW*1950)* (1-self.FM)) + (7500 * self.FM), 100, firdes.WIN_HAMMING, 6.76))
        self.analog_sig_source_x_1_0.set_amplitude(int(self.CW and self.KEY))

    def get_AFGain(self):
        return self.AFGain

    def set_AFGain(self, AFGain):
        self.AFGain = AFGain
        self.blocks_multiply_const_vxx_1.set_k((self.AFGain, ))

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
           if line=='P':
              tb.set_FFTEn(1)
           if line=='p':
              tb.set_FFTEn(0)
           if line[0]=='O':
              value=int(line[1:])
              tb.set_RxOffset(value)  
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
