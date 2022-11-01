/*
 * buzzer.h
 *
 * Created: 2016-09-15 오후 11:26:49
 *  Author: Administrator
 */ 



#ifndef BUZZER_H_
#define BUZZER_H_
#include <avr/io.h>
#include <util/delay.h>

#define BUZZER_PORT_DATA PORTE
#define BUZZER_DDR_DATA DDRE

#define BUTTON_DDR_DATA DDRD
#define BUTTON_PIN     ~(PIND)

#define FREQ(x)		(unsigned int)(16000000/(2*(1+x)))

#define DO_L	523
#define RE		587
#define MI		659
#define FA		698
#define SOL		783
#define RA		880
#define SI		987
#define DO_H	1046

void Change_FREQ(unsigned int freq); //FREQ 는 주파수의 약자
void STOP_FREQ(void);
unsigned int SevenScale(unsigned char button);

#endif /* BUZZER_H_ */