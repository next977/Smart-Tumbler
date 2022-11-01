/*
 * buzzer.c
 *
 * Created: 2016-09-15 오후 11:26:34
 *  Author: Administrator
 */ 
#include "buzzer.h"
void Buzzer_Init(void)
{
	BUZZER_DDR_DATA		=	0x0A; // OC3A(PE3) pin    OC3A(PE3)의 출력핀을 부저와 연결(VCC) GND는 GND에 연결.
	BUTTON_DDR_DATA		=	0x00; // 버튼 모든핀 입력으로 설정.
	
	TCCR3A	|=	0x40; // 16비트 CTC 12번 Mode 설정(WGM 3:0 ) 1100 설정.	COM3A(1:0) 01 Toggle 모드 설정
	TCCR3B	|=	0x19; // No prescaling (1분주)
	TCCR3C	=	0x00;
	
	TCNT3= 0x0000; // T/C1 value =0
	STOP_FREQ();
}

void Change_FREQ(unsigned int freq)
{
	unsigned int x;
	TCCR3A	|=	0x40;
	TCCR3B	|=	0x19;
	
	do
	{
		x	=TCNT3;
	}
	while (x >=FREQ(freq));
	ICR3 = FREQ(freq); // 부저에 입력되는 주파수 변경
}

void STOP_FREQ(void)
{
	TCCR3A = 0x00;
	TCCR3B = 0x00;
	TCNT3	=0x00;
	ICR3 =	FREQ(0);
	BUZZER_PORT_DATA = 0xff;
}

unsigned int SevenScale(unsigned char button)
{
	unsigned int _ret = 0;
	
	switch(button)
	{
		case 0x01:
		_ret = 523; //도
		break;
		case 0x02:
		_ret = 587; //레
		break;
		case 0x04:
		_ret = 659; //미
		break;
		case 0x08:
		_ret = 698; //파
		break;
		case 0x10:
		_ret = 783; //솔
		break;
		case 0x20:
		_ret = 880; //라
		break;
		case 0x40:
		_ret = 987; //시
		break;
		case 0x80:
		_ret = 1046; //도
		break;
	}
	return _ret;
}
