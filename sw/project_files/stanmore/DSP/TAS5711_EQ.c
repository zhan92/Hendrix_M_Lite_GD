//TAS5711_EQ.c file
#include "TAS5711_EQ.h"
//Product name and EQ rev: 2.0.1
//High EQ and DRC************************************************************



unsigned char EQ_HIGH_29[] = {0x29,0x00,0x7A,0xC6,0xFA,0x0F,0x0A,0x72,0x0B,0x00,0x7A,0xC6,0xFA,0x00,0xF5,0x57,0x65,0x0F,0x8A,0x3B,0x7C};
unsigned char EQ_HIGH_2A[] = {0x2A,0x00,0x7A,0xC6,0xFA,0x0F,0x0A,0x72,0x0B,0x00,0x7A,0xC6,0xFA,0x00,0xF5,0x57,0x65,0x0F,0x8A,0x3B,0x7C};
unsigned char EQ_HIGH_2B[] = {0x2B,0x00,0x78,0x9D,0xA8,0x0F,0x25,0x52,0x30,0x00,0x69,0xC6,0xFA,0x00,0xDA,0xAD,0xD0,0x0F,0x9D,0x9B,0x5D};
unsigned char EQ_HIGH_2C[] = {0x2C,0x00,0xB8,0x2B,0xB1,0x0F,0xC4,0xC2,0x3A,0x0F,0xED,0x23,0x4F,0x00,0x3B,0x3D,0xC6,0x0F,0xDA,0xB1,0x00};
unsigned char EQ_HIGH_2D[] = {0x2D,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_2E[] = {0x2E,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_2F[] = {0x2F,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_58[] = {0x58,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_59[] = {0x59,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_30[] = {0x30,0x00,0x7A,0xC6,0xFA,0x0F,0x0A,0x72,0x0B,0x00,0x7A,0xC6,0xFA,0x00,0xF5,0x57,0x65,0x0F,0x8A,0x3B,0x7C};
unsigned char EQ_HIGH_31[] = {0x31,0x00,0x7A,0xC6,0xFA,0x0F,0x0A,0x72,0x0B,0x00,0x7A,0xC6,0xFA,0x00,0xF5,0x57,0x65,0x0F,0x8A,0x3B,0x7C};
unsigned char EQ_HIGH_32[] = {0x32,0x00,0x78,0x9D,0xA8,0x0F,0x25,0x52,0x30,0x00,0x69,0xC6,0xFA,0x00,0xDA,0xAD,0xD0,0x0F,0x9D,0x9B,0x5D};
unsigned char EQ_HIGH_33[] = {0x33,0x00,0xB8,0x2B,0xB1,0x0F,0xC4,0xC2,0x3A,0x0F,0xED,0x23,0x4F,0x00,0x3B,0x3D,0xC6,0x0F,0xDA,0xB1,0x00};
unsigned char EQ_HIGH_34[] = {0x34,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_35[] = {0x35,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_36[] = {0x36,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_5C[] = {0x5C,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_HIGH_5D[] = {0x5D,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char DRC_HIGH_3A[] = {0x3A,0x00,0x18,0x12,0x77,0x00,0x67,0xED,0x86};
unsigned char DRC_HIGH_3B[] = {0x3B,0x00,0x0C,0xA9,0x91,0x00,0x73,0x56,0x6B};
unsigned char DRC_HIGH_3C[] = {0x3C,0x00,0x00,0x0D,0xA6,0x00,0x7F,0xF2,0x57};
unsigned char DRC_HIGH_40[] = {0x40,0xFD,0x6C,0xED,0xF2};
unsigned char DRC_HIGH_41[] = {0x41,0x0F,0x84,0x44,0x45};
unsigned char DRC_HIGH_42[] = {0x42,0x00,0x08,0x42,0x10};
unsigned char DRC_HIGH_46[] = {0x46,0x00,0x00,0x00,0x00};



//LOW EQ and DRC************************************************************



unsigned char EQ_LOW_29[] = {0x29,0x00,0x7F,0x9F,0x7D,0x0F,0x00,0xC1,0x05,0x00,0x7F,0x9F,0x7D,0x00,0xFF,0x3E,0x6A,0x0F,0x80,0xC0,0x74};
unsigned char EQ_LOW_2A[] = {0x2A,0x00,0x7F,0x9F,0x7D,0x0F,0x00,0xC1,0x05,0x00,0x7F,0x9F,0x7D,0x00,0xFF,0x3E,0x6A,0x0F,0x80,0xC0,0x74};
unsigned char EQ_LOW_2B[] = {0x2B,0x00,0x80,0x63,0xD4,0x0F,0x01,0x58,0xF0,0x00,0x7E,0x46,0xCE,0x00,0xFE,0xA7,0x10,0x0F,0x81,0x55,0x5D};
unsigned char EQ_LOW_2C[] = {0x2C,0x00,0xB4,0x5F,0x0D,0x0E,0x9B,0xD9,0xF6,0x00,0xAF,0xD5,0xC7,0x00,0xFC,0x20,0x1E,0x0F,0x83,0xD1,0x18};
unsigned char EQ_LOW_2D[] = {0x2D,0x00,0x00,0x15,0xA6,0x00,0x00,0x2B,0x4D,0x00,0x00,0x15,0xA6,0x00,0xF6,0x86,0x4D,0x0F,0x89,0x23,0x17};
unsigned char EQ_LOW_2E[] = {0x2E,0x00,0x00,0x15,0xA6,0x00,0x00,0x2B,0x4D,0x00,0x00,0x15,0xA6,0x00,0xF6,0x86,0x4D,0x0F,0x89,0x23,0x17};
unsigned char EQ_LOW_2F[] = {0x2F,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_LOW_58[] = {0x58,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_LOW_59[] = {0x59,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_LOW_30[] = {0x30,0x00,0x7F,0x9F,0x7D,0x0F,0x00,0xC1,0x05,0x00,0x7F,0x9F,0x7D,0x00,0xFF,0x3E,0x6A,0x0F,0x80,0xC0,0x74};
unsigned char EQ_LOW_31[] = {0x31,0x00,0x7F,0x9F,0x7D,0x0F,0x00,0xC1,0x05,0x00,0x7F,0x9F,0x7D,0x00,0xFF,0x3E,0x6A,0x0F,0x80,0xC0,0x74};
unsigned char EQ_LOW_32[] = {0x32,0x00,0x80,0x63,0xD4,0x0F,0x01,0x58,0xF0,0x00,0x7E,0x46,0xCE,0x00,0xFE,0xA7,0x10,0x0F,0x81,0x55,0x5D};
unsigned char EQ_LOW_33[] = {0x33,0x00,0xB4,0x5F,0x0D,0x0E,0x9B,0xD9,0xF6,0x00,0xAF,0xD5,0xC7,0x00,0xFC,0x20,0x1E,0x0F,0x83,0xD1,0x18};
unsigned char EQ_LOW_34[] = {0x34,0x00,0x00,0x15,0xA6,0x00,0x00,0x2B,0x4D,0x00,0x00,0x15,0xA6,0x00,0xF6,0x86,0x4D,0x0F,0x89,0x23,0x17};
unsigned char EQ_LOW_35[] = {0x35,0x00,0x00,0x15,0xA6,0x00,0x00,0x2B,0x4D,0x00,0x00,0x15,0xA6,0x00,0xF6,0x86,0x4D,0x0F,0x89,0x23,0x17};
unsigned char EQ_LOW_36[] = {0x36,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_LOW_5C[] = {0x5C,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char EQ_LOW_5D[] = {0x5D,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char DRC_LOW_3A[] = {0x3A,0x00,0x18,0x12,0x77,0x00,0x67,0xED,0x86};
unsigned char DRC_LOW_3B[] = {0x3B,0x00,0x06,0x7E,0xFA,0x00,0x79,0x81,0x03};
unsigned char DRC_LOW_3C[] = {0x3C,0x00,0x00,0x04,0x8B,0x00,0x7F,0xFB,0x72};
unsigned char DRC_LOW_40[] = {0x40,0xFD,0x57,0xAB,0x4C};
unsigned char DRC_LOW_41[] = {0x41,0x0F,0x84,0x44,0x45};
unsigned char DRC_LOW_42[] = {0x42,0x00,0x08,0x42,0x10};
unsigned char DRC_LOW_46[] = {0x46,0x00,0x00,0x00,0x00};


