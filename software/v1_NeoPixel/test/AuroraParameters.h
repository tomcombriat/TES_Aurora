#ifndef _AURORA_PARAM_
#define _AURORA_PARAM_


struct AuroraParameters
{
uint8_t midi_channel;
uint8_t speeder = 2;
unsigned long period = 1;  // inverse to the speed of the wave
}



#endif