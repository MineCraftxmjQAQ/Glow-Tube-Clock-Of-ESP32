#ifndef __MYHC595_H__
#define __MYHC595_H__

extern time_t now;

void HC595_Init(void);
void HC595_SendByte(unsigned char byte);
void GlowTube_Clock_Ctrl(void);

#endif