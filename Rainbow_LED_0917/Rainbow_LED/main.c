
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "NS_Rainbow_avr.h"
#include "lcd.h"
#define USELED	64						// Using LED Value

unsigned int g_unAdcValue	=	0; //ADC 12비트의 값을 담을 변수 선언
unsigned char g_ucOverflowCnt =	0; //8비트로 설정, 타이머를 구동하기 위해 오버플로우 횟수를 저장하기 위한 변수를 선언

void TIMER_Init (void)
{
	TCCR0	=	0x07;  // 0000 0111일때 111은 CSn2, CSn1, CSn0 의 값들이 111로 받아 본책 228페이지의 1024의 분주값.

	/* 0.01s에 오버플로우 발생, 1/(14745600Hz/1024)ⅹ45 = 0.03s  14745600hz는 cpu의 주파수 인데 이것은 14745600주기이다. 근데 이것의 초를 구하려면 1/14745600이고 
	X144를 해줘야 0.01초 를 만들어 줄 수 있다.*/ 
	// 오버플로우까지 카운트를 144로 설정
	
	TCNT0	=	0xff - 45; /*0xff이면 1111 1111이므로 255까지 카운터를 할 수 있는데, 여기서 256이면 오버풀러 이므로 256을 쓸수가 없어 144를 빼주어 111부터 144개를 카운터 하여
	                          111+144 즉 255를 만들어 주고 다음번 째 카운터에 256을 만들어줘 오버플로우 발생시켜 인터럽트가 걸리게끔 해주기위함임!.*/
	TIMSK |=	1 << TOIE0;	 
	TIFR  |=	1 << TOV0;	
	sei();
}

ISR (TIMER0_OVF_vect)	/* Timer0 Overflow0 ISP		0.003초 마다 인터럽트에 걸리게 하여 카운터를 세고, 그 카운터가 100이 되었을때,													즉 0.3초 마다 ADCL,ADCH의 데이터를 읽어 조도의 데이터를 LED나 FND로 표현 */
{
	//cli(); // 인터럽트 종료.
	
	unsigned char ucLowValue	=	0; //ADC 변환 결과 값에서 ADCL 값을 저장할 변수를 선언
	unsigned char ucHighValue	=	0;	//ADC 변환 결과 값에서 ADCH 값을 저장할 변수를 선언.
	
	TCNT0	=	0xff - 45;		// 0.003s 0.003s를 맞춰주기 위해 -45를 함.
	g_ucOverflowCnt++;
	
	if (g_ucOverflowCnt == 100)		/* 0.003s * 100 = 0.3s 0.3초 마다 ADCL과 ADCH를 변수에 받아들여 센서 값을 읽어옴
									 0.3초마다 ADC변환을 하는것*/
	{
		g_ucOverflowCnt	=	0;
		// ADC 변환이 끝날떄 까지 대기
		while ( (ADCSRA & 0x10)==0x00);	
		ucLowValue	=	ADCL; //ADCL 0~7번까지의 비트의 결과값을 (데이터를) 저장한다.
		ucHighValue	=	ADCH ;	//ADCH 즉 8비트 이상의 결과값을 (데이터를) 저장한다. 
		
		g_unAdcValue	=	(ucHighValue << 8) | ucLowValue ; 
	}
	//sei();
}

void ADC_Init()
{
	// Internal 2.56V Voltage Reference with external capacitor at AREF pin
	ADMUX	=	0xC1;     	/* ADC1 Channel, ADC멀티플랙서 선택 레지스터 0~4비트까지 mux0~mux4로 즉 1100 0001 0비트의 mux0을 선택
								7비트, 6비트의 REFS1, REFS0 을 11로 주어 2.56v를 써줌. , 5비트의 ADLAR에 0을 주어 우정렬을 함. */	

	// ADC Module Enable, Free Running Mode, ADC Prescaler : 64
	ADCSRA	=	0xA6;	/* ADC제어/상태 레지스터로 1010 0110 7비트인 ADEN에 1을 주어 ADC를 허용 하고, ADFR에 1을 주어 free running 모드로 
							동작, ADPS0~2 까지 총 3비트에 110을 주어 64번으로 분주해 샘플링 .*/
	ADCSRA	|=	0x40;	/* ADC start conversion 0100 0000 or 해서 0100 0000 즉 ADSC비트에 1을 주어서 free running 모드에서 첫 번째 변환을 시작 하기
							위해 "1"로 설정 , 그다음은 자동으로 변환이 반복됨, free running 모드는 한번 시작하면 자동적으로 adc를 변환*/
}

int main (void)
{
	unsigned char ucLowValue	=	0; //ADC 변환 결과 값에서 ADCL 값을 저장할 변수를 선언
	unsigned char ucHighValue	=	0;	//ADC 변환 결과 값에서 ADCH 값을 저장할 변수를 선언.
	unsigned char line = 0;
	
	_delay_ms(10);
	NS_Rainbow_init(USELED, PB, 0); //Array LED 설정 PORTB, 0번핀 사용
	setBrightness(50); 	// 0,1(OFF) ~ 255 Array LED 색 설정

	TIMER_Init();
	ADC_Init ();		// PORTF = ADC1
	
	while (1)
	{								// 수위센서의 값의 범위에 따른 Array LED 제어
		if((g_unAdcValue>900)&&(g_unAdcValue<=1000))
		{
		
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				setColor_RGB(3 + line, 0x1DDB16);			// Green
				setColor_RGB(4 + line, 0x0000FF);			// Blue
				setColor_RGB(5 + line, 0x0100FF);			// Dark Blue
				setColor_RGB(6 + line, 0x3F0099);			// Purple
				show();
				_delay_ms(100);
			}	
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>800)&&(g_unAdcValue<=900))
		{
		
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				setColor_RGB(3 + line, 0x1DDB16);			// Green
				setColor_RGB(4 + line, 0x0000FF);			// Blue
				setColor_RGB(5 + line, 0x0100FF);			// Dark Blue
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>600)&&(g_unAdcValue<=800))
		{
		
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				setColor_RGB(3 + line, 0x1DDB16);			// Green
				setColor_RGB(4 + line, 0x0000FF);			// Blue
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>500)&&(g_unAdcValue<=600))
		{
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				setColor_RGB(3 + line, 0x1DDB16);			// Green
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>300)&&(g_unAdcValue<=500))
		{
		
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>100)&&(g_unAdcValue<=300))
		{
		
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		}
		else if((g_unAdcValue>30)&&(g_unAdcValue<=100))
			{
			
				for(int i = 7; i >= 0; i--)
				{
					line = i * 8;
					setColor_RGB(0 + line, 0xFF0000);			// RED
					show();
					_delay_ms(100);
				}
				_delay_ms(500);
				clear();
			}
		else if((g_unAdcValue>=0)&&(g_unAdcValue<=30))
		{
			clear();
		}	
		else
		{
			for(int i = 7; i >= 0; i--)
			{
				line = i * 8;
				setColor_RGB(0 + line, 0xFF0000);			// RED
				setColor_RGB(1 + line, 0xFF5E00);			// Orange
				setColor_RGB(2 + line, 0xFFE400);			// Yellow
				setColor_RGB(3 + line, 0x1DDB16);			// Green
				setColor_RGB(4 + line, 0x0000FF);			// Blue
				setColor_RGB(5 + line, 0x0100FF);			// Dark Blue
				setColor_RGB(6 + line, 0x3F0099);			// Purple
				setColor_RGB(7 + line, 0xFF00DD);			// Pink
				show();
				_delay_ms(100);
			}
			_delay_ms(500);
			clear();
		
		}
	}
	return 0;
}


