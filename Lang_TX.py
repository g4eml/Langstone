#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Lang Tx
# Generated: Wed May 25 11:58:00 2022
##################################################

import os
import errno

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


class Lang_TX(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Lang Tx")

        ##################################################
        # Variables
        ##################################################
        plutoip=os.environ.get('PLUTO_IP')
        if plutoip==None :
          plutoip='pluto.local'
        plutoip='ip:' + plutoip
        
        self.ToneBurst = ToneBurst = False
        self.PTT = PTT = False
        self.Mode = Mode = 0
        self.MicGain = MicGain = 5.0
        self.KEY = KEY = False
        self.Filt_Low = Filt_Low = 300
        self.Filt_High = Filt_High = 3000
        self.FMMIC = FMMIC = 50
        self.FFTEn = FFTEn = 0
        self.CTCSS = CTCSS = 885

        ##################################################
        # Blocks
        ##################################################
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=11,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.pluto_sink_0 = iio.pluto_sink(plutoip, 1000000000, 528000, 2000000, 0x800, False, 0, '', True)
        self.logpwrfft_x_0 = logpwrfft.logpwrfft_c(
        	sample_rate=48000,
        	fft_size=512,
        	ref_scale=2,
        	frame_rate=15,
        	avg_alpha=0.9,
        	average=True,
        )
        self.blocks_udp_sink_0 = blocks.udp_sink(gr.sizeof_float*512, '127.0.0.1', 7374, 1472, False)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_float*512)
        self.blocks_mute_xx_0_0 = blocks.mute_cc(bool(not PTT))
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_vcc(((Mode < 4) or (Mode==5), ))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_vcc((Mode==4, ))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff((FMMIC/5.0, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(((MicGain/10.0)*(not (Mode==2))*(not (Mode==3)), ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_0_0 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.blocks_add_const_vxx_0 = blocks.add_const_vcc(((0.5 * int(Mode==5)) + (int(Mode==2) * KEY) +(int(Mode==3) * KEY), ))
        self.blks2_selector_0 = grc_blks2.selector(
        	item_size=gr.sizeof_float*512,
        	num_inputs=1,
        	num_outputs=2,
        	input_index=0,
        	output_index=FFTEn,
        )
        self.band_pass_filter_1 = filter.fir_filter_fff(1, firdes.band_pass(
        	1, 48000, 300, 3500, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 48000, Filt_Low, Filt_High, 100, firdes.WIN_HAMMING, 6.76))
        self.audio_source_0 = audio.source(48000, "hw:CARD=Device,DEV=0", False)
        self.analog_sig_source_x_1_0 = analog.sig_source_f(48000, analog.GR_SIN_WAVE, CTCSS / 10.0, 0.15 * (CTCSS >0), 0)
        self.analog_sig_source_x_1 = analog.sig_source_f(48000, analog.GR_COS_WAVE, 1750, 1.0*ToneBurst, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(48000, analog.GR_COS_WAVE, 0, 1, 0)
        self.analog_rail_ff_0 = analog.rail_ff(-1, 1)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=48000,
        	quad_rate=48000,
        	tau=75e-6,
        	max_dev=5000,
        	fh=-1,
                )
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.analog_agc2_xx_1 = analog.agc2_cc(1e-1, 1e-1, 1.3- (0.65*(int(Mode==5))), 1.0)
        self.analog_agc2_xx_1.set_max_gain(10)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc2_xx_1, 0), (self.band_pass_filter_0_0, 0))
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.analog_rail_ff_0, 0), (self.band_pass_filter_1, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_add_xx_0_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.blocks_add_xx_0_0, 1))
        self.connect((self.blks2_selector_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.blks2_selector_0, 1), (self.blocks_udp_sink_0, 0))
        self.connect((self.blocks_add_const_vxx_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_rail_ff_0, 0))
        self.connect((self.blocks_add_xx_0_0, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.logpwrfft_x_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_add_const_vxx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_3, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_4, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.analog_agc2_xx_1, 0))
        self.connect((self.blocks_mute_xx_0_0, 0), (self.pluto_sink_0, 0))
        self.connect((self.logpwrfft_x_0, 0), (self.blks2_selector_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_mute_xx_0_0, 0))

    def get_ToneBurst(self):
        return self.ToneBurst

    def set_ToneBurst(self, ToneBurst):
        self.ToneBurst = ToneBurst
        self.analog_sig_source_x_1.set_amplitude(1.0*self.ToneBurst)

    def get_PTT(self):
        return self.PTT

    def set_PTT(self, PTT):
        self.PTT = PTT
        self.blocks_mute_xx_0_0.set_mute(bool(not self.PTT))

    def get_Mode(self):
        return self.Mode

    def set_Mode(self, Mode):
        self.Mode = Mode
        self.blocks_multiply_const_vxx_4.set_k(((self.Mode < 4) or (self.Mode==5), ))
        self.blocks_multiply_const_vxx_3.set_k((self.Mode==4, ))
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not (self.Mode==2))*(not (self.Mode==3)), ))
        self.blocks_add_const_vxx_0.set_k(((0.5 * int(self.Mode==5)) + (int(self.Mode==2) * self.KEY) +(int(self.Mode==3) * self.KEY), ))
        self.analog_agc2_xx_1.set_reference(1.3- (0.65*(int(self.Mode==5))))

    def get_MicGain(self):
        return self.MicGain

    def set_MicGain(self, MicGain):
        self.MicGain = MicGain
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not (self.Mode==2))*(not (self.Mode==3)), ))

    def get_KEY(self):
        return self.KEY

    def set_KEY(self, KEY):
        self.KEY = KEY
        self.blocks_add_const_vxx_0.set_k(((0.5 * int(self.Mode==5)) + (int(self.Mode==2) * self.KEY) +(int(self.Mode==3) * self.KEY), ))

    def get_Filt_Low(self):
        return self.Filt_Low

    def set_Filt_Low(self, Filt_Low):
        self.Filt_Low = Filt_Low
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Filt_Low, self.Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_Filt_High(self):
        return self.Filt_High

    def set_Filt_High(self, Filt_High):
        self.Filt_High = Filt_High
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Filt_Low, self.Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_FMMIC(self):
        return self.FMMIC

    def set_FMMIC(self, FMMIC):
        self.FMMIC = FMMIC
        self.blocks_multiply_const_vxx_0_0.set_k((self.FMMIC/5.0, ))

    def get_FFTEn(self):
        return self.FFTEn

    def set_FFTEn(self, FFTEn):
        self.FFTEn = FFTEn
        self.blks2_selector_0.set_output_index(int(self.FFTEn))

    def get_CTCSS(self):
        return self.CTCSS

    def set_CTCSS(self, CTCSS):
        self.CTCSS = CTCSS
        self.analog_sig_source_x_1_0.set_frequency(self.CTCSS / 10.0)
        self.analog_sig_source_x_1_0.set_amplitude(0.15 * (self.CTCSS >0))

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

def main(top_block_cls=Lang_TX, options=None):

    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
