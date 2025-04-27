#ifndef _AURORA_PARAM_
#define _AURORA_PARAM_


struct AuroraParameters
{
uint8_t midi_channel=2;
uint8_t midi_control_MSB=74;
uint8_t midi_control_LSB=75;
uint8_t midi_pitchbend_amplitude_control = 5;
uint8_t speeder = 1;
unsigned long period = 10;  // inverse to the speed of the wave
};



#endif