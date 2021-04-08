/***********************************************
This file is created by cirrus dsp tool on 11/17/17 18:00:40
This tool was written by XuShoucai on 06/01/09
************************************************/
const uint8_t PREKICKSTART_CFG [] = 
{
/* preKickStart.cfg */
//include preKickStart\system_pcm.cfg
// ----------
// PCM (system_pcm) config
// ----------
// PCM_Decoder_Input_Mode
0x9b,0x00,0x00,0x0e,0x00,0x00,0x00,0x02,
//include preKickStart\cs48L11_ha.cfg
// ----------
// CS48L11 (cs48L11_ha) config
// ----------
// MCLK 12.288 MHz
// Set Core= 150 MHz
// Set pll_ref_clock = 24.576 MHz
0x81,0x00,0x00,0x34,0x00,0x20,0x00,0x7d,
0x81,0x00,0x00,0x35,0x00,0x00,0x00,0xb1,
0x81,0x00,0x00,0x36,0x00,0x00,0x01,0x00,
0x81,0x00,0x00,0x37,0x00,0x00,0x03,0x0b,
// Set Input mode to Stereo PCM/Compressed Input Mode
0x81,0x00,0x00,0x01,0x00,0x00,0x00,0x02,
//include preKickStart\dsp_dai_uni.cfg
// ----------
// Remap, DAI (dsp_dai_uni) config
// ----------
// DAI A Parameter: Data Format
// DAI data format= I2S 24-bit
0x81,0x83,0x00,0x10,
0xFF,0xFF,0x00,0x00,
0xFF,0xFF,0x00,0x00,
0xFF,0xFF,0x00,0x00,
0xFF,0xFF,0x00,0x00,
0x81,0x43,0x00,0x10,
0x01,0x00,0x1F,0x00,
0x01,0x00,0x1F,0x00,
0x01,0x00,0x1F,0x00,
0x01,0x00,0x1F,0x00,
0x81,0x80,0x00,0x14,0xFF,0xFF,0xD1,0x00,
0x81,0x40,0x00,0x14,0x00,0x00,0xD1,0x00,
// DAI B Parameter: DAI_SCLK Polarity
// DAI SCLK polarity=rising edge
0x81,0x83,0x00,0x10,
0xFF,0xDF,0xFF,0xFF,
0xFF,0xDF,0xFF,0xFF,
0xFF,0xDF,0xFF,0xFF,
0xFF,0xDF,0xFF,0xFF,
// DAI C Parameter: Input LRCLK Polarity
// C1 (default) LRCLK=Low indicates Left channel
0x81,0x43,0x00,0x10,
0x00,0x10,0x00,0x00,
0x00,0x10,0x00,0x00,
0x00,0x10,0x00,0x00,
0x00,0x10,0x00,0x00,
// DAI D Parameter: Input DAI Mode
// Switch to 1in 3out
0x81,0x40,0x00,0x32,0x00,0x00,0x00,0x13,
// DAI E Parameter: Input DAI Master/Slave
// PAD1->LINE3, DAI SCLK/LRCLK slave
0x81,0x80,0x00,0x14,0xF0,0x0F,0xFF,0xFF,
// // io_buff_0_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
// // io_buff_1_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x02,0x08,0x00,0x00,0x00,
// // io_buff_2_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x03,0x00,0x00,0x00,0x01,
// // io_buff_3_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x04,0x08,0x00,0x00,0x00,
// // io_buff_4_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x05,0x08,0x00,0x00,0x00,
// // io_buff_5_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x06,0x08,0x00,0x00,0x00,
// // io_buff_6_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x07,0x08,0x00,0x00,0x00,
// // io_buff_7_source : unsigned : 32.0 format
0x9b,0x00,0x00,0x08,0x08,0x00,0x00,0x00,
//include preKickStart\dsp_dao_uni.cfg
// ----------
// Remap, DAO (dsp_dao_uni) config
// ----------
// DAO parameter A
// DAO SCLK/LRCLK Slave
0x81,0x40,0x00,0x19,0x00,0x00,0x20,0x00,
// DAO parameter B
// DAO MCLK to be slave
0x81,0x40,0x00,0x18,0x00,0x00,0x00,0x01,
// Mastered MCLK Source
// MCLK = REFCLOCK
0x81,0x00,0x00,0x37,0x00,0x00,0x01,0x00,
// DAO parameter C: Output DAO_SCLK/LRCLK Configuration
// C5- MCLK/SCLK ratio=512Fs/64Fs
0x81,0x00,0x00,0x1E,0x00,0x00,0x77,0x07,
0x81,0x80,0x00,0x19,0xFF,0xFF,0xFF,0x8F,
0x81,0x40,0x00,0x19,0x00,0x00,0x00,0x20,
// DAO parameter D: DAO data format
// I2S 32-bit
0x81,0x02,0x00,0x1A,
0x00,0x00,0x00,0x01,
0x00,0x00,0x00,0x01,
0x00,0x00,0x00,0x01,
// DAO parameter E: DAO_LRCLK polarity
// Set LRCLK polarity=Left Low
0x81,0x40,0x00,0x19,0x00,0x00,0x07,0x00,
// DAO parameter F: DAO_SCLK polarity
// Data Valid on Rising Edge (clocked out on falling)
0x81,0x80,0x00,0x19,0xFF,0xFF,0xEF,0xFF,
// DAO parameter G: DAO_MCLK Internal Source
// DAO parameter H: MCLK Divider
// Set MCLK divider=1
0x81,0x00,0x00,0x1D,
0x00,0x00,0x00,0x00,
//include preKickStart\crd_audio_out_48LXX.cfg
// ----------
//                                         Audio Out (crd_audio_out_48LXX) config
// ----------
// // (AUDIO OUT 12) Left. unsigned : 32.0 format
0x83,0x00,0x00,0x12,0x00,0x00,0x00,0x00,
// // (AUDIO OUT 13) Right. unsigned : 32.0 format
0x83,0x00,0x00,0x13,0x00,0x00,0x00,0x02,
// // (AUDIO OUT 14) Left. unsigned : 32.0 format
0x83,0x00,0x00,0x14,0x00,0x00,0x00,0x03,
// // (AUDIO OUT 15) Right. unsigned : 32.0 format
0x83,0x00,0x00,0x15,0x00,0x00,0x00,0x04,
// // (AUDIO OUT 16) Left. unsigned : 32.0 format
0x83,0x00,0x00,0x16,0x00,0x00,0x00,0x01,
// // (AUDIO OUT 17) Right. unsigned : 32.0 format
0x83,0x00,0x00,0x17,0x00,0x00,0x00,0x05,
//include preKickStart\custom.cfg
};
#define Bytes_of_preKickStart_cfg  360   //(bytes)
