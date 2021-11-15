#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Lang Trx Pluto
# Generated: Mon Nov 15 19:34:23 2021
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
from optparse import OptionParser


class Lang_TRX_Pluto(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Lang Trx Pluto")

        ##################################################
        # Variables
        ##################################################
        
        plutoip=os.environ.get('PLUTO_IP')
        if plutoip==None :
          plutoip='pluto.local'
        plutoip='ip:' + plutoip 
              
        self.Tx_Mode = Tx_Mode = 0
        self.Tx_LO = Tx_LO = 1000000000
        self.Tx_Gain = Tx_Gain = 30
        self.Tx_Filt_Low = Tx_Filt_Low = 300
        self.Tx_Filt_High = Tx_Filt_High = 3000
        self.ToneBurst = ToneBurst = False
        self.SQL = SQL = 50
        self.Rx_Mute = Rx_Mute = False
        self.Rx_Mode = Rx_Mode = 3
        self.Rx_LO = Rx_LO = 1000000000
        self.Rx_Gain = Rx_Gain = 30
        self.Rx_Filt_Low = Rx_Filt_Low = 300
        self.Rx_Filt_High = Rx_Filt_High = 3000
        self.RxOffset = RxOffset = 0
        self.PTT = PTT = False
        self.MicGain = MicGain = 5.0
        self.KEY = KEY = False
        self.FMMIC = FMMIC = 50
        self.AFGain = AFGain = 20

        ##################################################
        # Blocks
        ##################################################
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=11,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.pluto_source_0 = iio.pluto_source(plutoip, 1000000000, 528000, 2000000, 0x800, True, True, True, "slow_attack", 64.0, '', True)
        self.pluto_sink_0 = iio.pluto_sink(plutoip, 1000000000, 528000, 2000000, 0x800, False, 0, '', True)
        self.logpwrfft_x_0_0 = logpwrfft.logpwrfft_c(
        	sample_rate=48000,
        	fft_size=512,
        	ref_scale=2,
        	frame_rate=15,
        	avg_alpha=0.9,
        	average=True,
        )
        self.logpwrfft_x_0 = logpwrfft.logpwrfft_c(
        	sample_rate=48000,
        	fft_size=512,
        	ref_scale=2,
        	frame_rate=15,
        	avg_alpha=0.9,
        	average=True,
        )
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(11, (firdes.low_pass(1,529200,23000,2000)), RxOffset, 528000)
        self.blocks_udp_sink_0_0 = blocks.udp_sink(gr.sizeof_float*512, '127.0.0.1', 7474, 1472, False)
        self.blocks_udp_sink_0 = blocks.udp_sink(gr.sizeof_float*512, '127.0.0.1', 7373, 1472, False)
        self.blocks_mute_xx_0_0 = blocks.mute_cc(bool(not PTT))
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_4 = blocks.multiply_const_vcc(((Tx_Mode < 4) or (Tx_Mode==5), ))
        self.blocks_multiply_const_vxx_3 = blocks.multiply_const_vcc((Tx_Mode==4, ))
        self.blocks_multiply_const_vxx_2_1_0 = blocks.multiply_const_vff((1.0 + (Rx_Mode==5), ))
        self.blocks_multiply_const_vxx_2_1 = blocks.multiply_const_vff((Rx_Mode==5, ))
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_vff(((Rx_Mode==4) * 0.2, ))
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_vff((Rx_Mode<4, ))
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vff(((AFGain/100.0) *  (not Rx_Mute), ))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff((FMMIC/5.0, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(((MicGain/10.0)*(not (Tx_Mode==2))*(not (Tx_Mode==3)), ))
        self.blocks_float_to_complex_0_0 = blocks.float_to_complex(1)
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_real_0_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_1_0 = blocks.add_vff(1)
        self.blocks_add_xx_1 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.blocks_add_const_vxx_0 = blocks.add_const_vcc(((0.5 * int(Tx_Mode==5)) + (int(Tx_Mode==2) * KEY) +(int(Tx_Mode==3) * KEY), ))
        self.band_pass_filter_1 = filter.fir_filter_fff(1, firdes.band_pass(
        	1, 48000, 200, 3500, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 48000, Tx_Filt_Low, Tx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))
        self.band_pass_filter_0 = filter.fir_filter_ccc(1, firdes.complex_band_pass(
        	1, 48000, Rx_Filt_Low, Rx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))
        self.audio_source_0 = audio.source(48000, "hw:CARD=Device,DEV=0", False)
        self.audio_sink_0 = audio.sink(48000, "hw:CARD=Device,DEV=0", False)
        self.analog_sig_source_x_1 = analog.sig_source_f(48000, analog.GR_COS_WAVE, 1750, 1.0*ToneBurst, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(48000, analog.GR_COS_WAVE, 0, 1, 0)
        self.analog_rail_ff_0 = analog.rail_ff(-1, 1)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(SQL-100, 0.001, 0, False)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=48000,
        	quad_rate=48000,
        	tau=1000e-6,
        	max_dev=250,
        	fh=-1,
                )
        self.analog_nbfm_rx_0 = analog.nbfm_rx(
        	audio_rate=48000,
        	quad_rate=48000,
        	tau=75e-6,
        	max_dev=5e3,
          )
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.analog_agc3_xx_0 = analog.agc3_cc(1e-2, 5e-7, 0.1, 1.0, 1)
        self.analog_agc3_xx_0.set_max_gain(1000)
        self.analog_agc2_xx_1 = analog.agc2_cc(1e-1, 1e-1, 1.3- (0.65*(int(Tx_Mode==5))), 1.0)
        self.analog_agc2_xx_1.set_max_gain(10)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc2_xx_1, 0), (self.band_pass_filter_0_0, 0))
        self.connect((self.analog_agc3_xx_0, 0), (self.blocks_complex_to_real_0_0, 0))
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0_0, 1))
        self.connect((self.analog_nbfm_rx_0, 0), (self.blocks_multiply_const_vxx_2_0, 0))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_3, 0))
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.analog_rail_ff_0, 0), (self.band_pass_filter_1, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.audio_source_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.analog_pwr_squelch_xx_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.band_pass_filter_0_0, 0), (self.blocks_multiply_const_vxx_4, 0))
        self.connect((self.band_pass_filter_1, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.blocks_add_const_vxx_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_rail_ff_0, 0))
        self.connect((self.blocks_add_xx_1, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_add_xx_1_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.logpwrfft_x_0_0, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.blocks_multiply_const_vxx_2_1, 0))
        self.connect((self.blocks_complex_to_real_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.blocks_complex_to_real_0_0, 0), (self.blocks_multiply_const_vxx_2_1_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.analog_agc3_xx_0, 0))
        self.connect((self.blocks_float_to_complex_0_0, 0), (self.blocks_add_const_vxx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_float_to_complex_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.audio_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.blocks_add_xx_1_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2_0, 0), (self.blocks_add_xx_1, 1))
        self.connect((self.blocks_multiply_const_vxx_2_1, 0), (self.blocks_add_xx_1_0, 1))
        self.connect((self.blocks_multiply_const_vxx_2_1_0, 0), (self.blocks_add_xx_1, 0))
        self.connect((self.blocks_multiply_const_vxx_3, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_multiply_const_vxx_4, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.analog_agc2_xx_1, 0))
        self.connect((self.blocks_mute_xx_0_0, 0), (self.pluto_sink_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.logpwrfft_x_0, 0))
        self.connect((self.logpwrfft_x_0, 0), (self.blocks_udp_sink_0, 0))
        self.connect((self.logpwrfft_x_0_0, 0), (self.blocks_udp_sink_0_0, 0))
        self.connect((self.pluto_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_mute_xx_0_0, 0))

    def get_Tx_Mode(self):
        return self.Tx_Mode

    def set_Tx_Mode(self, Tx_Mode):
        self.Tx_Mode = Tx_Mode
        self.blocks_multiply_const_vxx_4.set_k(((self.Tx_Mode < 4) or (self.Tx_Mode==5), ))
        self.blocks_multiply_const_vxx_3.set_k((self.Tx_Mode==4, ))
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not (self.Tx_Mode==2))*(not (self.Tx_Mode==3)), ))
        self.blocks_add_const_vxx_0.set_k(((0.5 * int(self.Tx_Mode==5)) + (int(self.Tx_Mode==2) * self.KEY) +(int(self.Tx_Mode==3) * self.KEY), ))
        self.analog_agc2_xx_1.set_reference(1.3- (0.65*(int(self.Tx_Mode==5))))

    def get_Tx_LO(self):
        return self.Tx_LO

    def set_Tx_LO(self, Tx_LO):
        self.Tx_LO = Tx_LO
        self.pluto_sink_0.set_params(self.Tx_LO, 528000, 2000000, self.Tx_Gain, '', True)

    def get_Tx_Gain(self):
        return self.Tx_Gain

    def set_Tx_Gain(self, Tx_Gain):
        self.Tx_Gain = Tx_Gain
        self.pluto_sink_0.set_params(self.Tx_LO, 528000, 2000000, self.Tx_Gain, '', True)

    def get_Tx_Filt_Low(self):
        return self.Tx_Filt_Low

    def set_Tx_Filt_Low(self, Tx_Filt_Low):
        self.Tx_Filt_Low = Tx_Filt_Low
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Tx_Filt_Low, self.Tx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_Tx_Filt_High(self):
        return self.Tx_Filt_High

    def set_Tx_Filt_High(self, Tx_Filt_High):
        self.Tx_Filt_High = Tx_Filt_High
        self.band_pass_filter_0_0.set_taps(firdes.complex_band_pass(1, 48000, self.Tx_Filt_Low, self.Tx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_ToneBurst(self):
        return self.ToneBurst

    def set_ToneBurst(self, ToneBurst):
        self.ToneBurst = ToneBurst
        self.analog_sig_source_x_1.set_amplitude(1.0*self.ToneBurst)

    def get_SQL(self):
        return self.SQL

    def set_SQL(self, SQL):
        self.SQL = SQL
        self.analog_pwr_squelch_xx_0.set_threshold(self.SQL-100)

    def get_Rx_Mute(self):
        return self.Rx_Mute

    def set_Rx_Mute(self, Rx_Mute):
        self.Rx_Mute = Rx_Mute
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Rx_Mute), ))

    def get_Rx_Mode(self):
        return self.Rx_Mode

    def set_Rx_Mode(self, Rx_Mode):
        self.Rx_Mode = Rx_Mode
        self.blocks_multiply_const_vxx_2_1_0.set_k((1.0 + (self.Rx_Mode==5), ))
        self.blocks_multiply_const_vxx_2_1.set_k((self.Rx_Mode==5, ))
        self.blocks_multiply_const_vxx_2_0.set_k(((self.Rx_Mode==4) * 0.2, ))
        self.blocks_multiply_const_vxx_2.set_k((self.Rx_Mode<4, ))

    def get_Rx_LO(self):
        return self.Rx_LO

    def set_Rx_LO(self, Rx_LO):
        self.Rx_LO = Rx_LO
        self.pluto_source_0.set_params(self.Rx_LO, 528000, 2000000, True, True, True, "slow_attack", 64.0, '', True)

    def get_Rx_Gain(self):
        return self.Rx_Gain

    def set_Rx_Gain(self, Rx_Gain):
        self.Rx_Gain = Rx_Gain

    def get_Rx_Filt_Low(self):
        return self.Rx_Filt_Low

    def set_Rx_Filt_Low(self, Rx_Filt_Low):
        self.Rx_Filt_Low = Rx_Filt_Low
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 48000, self.Rx_Filt_Low, self.Rx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))

    def get_Rx_Filt_High(self):
        return self.Rx_Filt_High

    def set_Rx_Filt_High(self, Rx_Filt_High):
        self.Rx_Filt_High = Rx_Filt_High
        self.band_pass_filter_0.set_taps(firdes.complex_band_pass(1, 48000, self.Rx_Filt_Low, self.Rx_Filt_High, 100, firdes.WIN_HAMMING, 6.76))

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

    def get_MicGain(self):
        return self.MicGain

    def set_MicGain(self, MicGain):
        self.MicGain = MicGain
        self.blocks_multiply_const_vxx_0.set_k(((self.MicGain/10.0)*(not (self.Tx_Mode==2))*(not (self.Tx_Mode==3)), ))

    def get_KEY(self):
        return self.KEY

    def set_KEY(self, KEY):
        self.KEY = KEY
        self.blocks_add_const_vxx_0.set_k(((0.5 * int(self.Tx_Mode==5)) + (int(self.Tx_Mode==2) * self.KEY) +(int(self.Tx_Mode==3) * self.KEY), ))

    def get_FMMIC(self):
        return self.FMMIC

    def set_FMMIC(self, FMMIC):
        self.FMMIC = FMMIC
        self.blocks_multiply_const_vxx_0_0.set_k((self.FMMIC/5.0, ))

    def get_AFGain(self):
        return self.AFGain

    def set_AFGain(self, AFGain):
        self.AFGain = AFGain
        self.blocks_multiply_const_vxx_1.set_k(((self.AFGain/100.0) *  (not self.Rx_Mute), ))

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

def main(top_block_cls=Lang_TRX_Pluto, options=None):

    tb = top_block_cls()
    tb.start()
    docommands(tb)
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
