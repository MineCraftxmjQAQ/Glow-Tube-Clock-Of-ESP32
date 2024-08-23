#ifndef __MYHC595_H__
#define __MYHC595_H__

#include <time.h>

extern time_t now;
extern uint8_t GlowTube_Clock_State;

void HC595_Init(void);
void HC595_SendData(uint64_t data);
void GlowTube_Clock_Ctrl(void);

#endif