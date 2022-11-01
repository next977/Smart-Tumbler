/*
 * SmartTumbler_0919.c
 *
 * Created: 2016-09-19 오전 11:24:10
 * Author : Administrator
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "NS_Rainbow_avr.h"
#define null 0
#define ETX 0x0A;
#define USELED	64						// Using LED Value
unsigned int g_unAdcValue	=	0; /*16비트로 설정. ADC가 10비트로 (10비트 분해능)해야 설정을 할 수 있으므로.
									 ADCL과 ADCH를 통해 나온 결과 값을 조합하여
									최종 결과를 보여주는 변수를 선언*/
unsigned char g_ucOverflowCnt =	0; //8비트로 설정, 타이머를 구동하기 위해 오버플로우 횟수를 저장하기 위한 변수를 선언
volatile unsigned char BUF;
volatile unsigned char RX;
char NAME[10] = "$GPGGA";
char NAME1[10] = "$GPGSA";
char NAME2[10] = "$GPRMC";

char GPS_DATA[100];
char GPS_DATA1[100];
char GPS_DATA2[100];
char GPS_DATA3[100];

int GPScnt;
int GPScnt1;
int i;

char *GPS_cut = null;
char *GPS_Dcut[15] = {null};	//포인트 배열 선언.
char *GPS_Dcut1[15] = {null};
char *GPS_Dcut2[15] = {null};

void TIMER_Init (void)
{
	// Timer/Count0 사용
	// 타이머를 이용해서 1초를 만들어 주는것이 이프로그램의 목적임!
	// 동작모드와 클럭소스 및 프리스케일러 결정
	TCCR0	=	0x07;  // 0000 0111일때 111은 CSn2, CSn1, CSn0 의 값들이 111로 받아 본책 228페이지의 1024의 분주값.

	/* 0.01s에 오버플로우 발생, 1/(14745600Hz/1024)ⅹ45 = 0.03s  14745600hz는 cpu의 주파수 인데 이것은 14745600주기이다. 근데 이것의 초를 구하려면 1/14745600이고 
	X144를 해줘야 0.01초 를 만들어 줄 수 있다.*/ 
	// 오버플로우까지 카운트를 144로 설정
	TCNT0	=	0xff - 45; /*0xff이면 1111 1111이므로 255까지 카운터를 할 수 있는데, 여기서 256이면 오버풀러 이므로 256을 쓸수가 없어 144를 빼주어 111부터 144개를 카운터 하여
	                          111+144 즉 255를 만들어 주고 다음번 째 카운터에 256을 만들어줘 오버플로우 발생시켜 인터럽트가 걸리게끔 해주기위함임!.*/
	TIMSK |=	1 << TOIE0;	 /* Overflow Interupt Enable  트리거가 완료되어 인터럽트의 요청이 되면 이 값의 (1의 값)을 주어 인터럽트를 허용해주는 레지스터.
															TOIE0이라는 것은 TIMSK의 0비트에 해당이 되는 것인데, TOIE0의값은 초기값이 0이고 1<<TOIEO은 
															0만큼 왼쪽으로 1칸 옴기라는 뜻이므로 결국 아무런 영향을 주지 못한다.
															기본적으로 1<<TOIE0의 연산자를 먼저 수행하고 TIMSK or 1을 수행하므로 TIMSK or 1은 TIMSK도 초기값
															0000 0000 이고 0000 0001이므로 or은 0000 0001로 나타나 진다. 즉 TIMSK는 0000 0001이되며 마지막 0의
															비트값이 1로 작동 된다는 소리이다. (인터럽트의 마스크와 비슷한 맥락)*/
	TIFR |=	1 << TOV0;	/* set Overflow Interupt Flag  // 이 값의 허용치가 되어 인터럽트가 걸리면 깃발을 들어줘 (1의 값) 인터럽트를 허용해주는 레지스터
	                        TIFR도 TOV0이 0비트이기 때문에 TIMSK와 똑같은 역할을 함. 즉 0의 비트가 1로 작동이 된다는 소리 
	                    근데 그러면 구지 TOV0과 연산자를 왜 써주냐면 TIFR의 0의 비트가 무슨 값인지 모르기 때문에 사용자가 책을 펴볼수박에없는데 TOV0와 <<1로 TOV0의
						값을 나타내주어 사용자가 더 편하게 프로그램을 짤 수 있도록 하는 문장. */
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
		while ( (ADCSRA & 0x10)==0x00);	/* 0x10과 ADCSRA과 and 한 값이 0x00이면 while문 실행, 즉 무조건 0x00으로 
										while문을 실행하고 아무런 실행문의 문장이 없으니 그대로 멈추어 있음.
									   근데 ; 이 있으니  ; 까지 수행.while문 께속 돌다가 ADC가 완료 되어서 ADCSRA ADIF비트가 
									   1로 셋되면 while문을 나옴. 
										 */
		
		ucLowValue	=	ADCL; //ADCL 0~7번까지의 비트의 결과값을 (데이터를) 저장한다.
		ucHighValue	=	ADCH ;	//ADCH 즉 8비트 이상의 결과값을 (데이터를) 저장한다. 
		
		/* 64를 14745600의 hz를 64로 분주하여 샘플링한 값을 표로 나타내고 2.56v로 분배능을 하여 총 조도센서에서 읽어지는 아날로그
		값을 예를 들면 256이라고 하면 255까지는 8비트 1111 1111으로 표현할수 있기 때문에 ADCL로 데이터를 받아들여 저장하고,
		나머지 255에서 저장하고 남은 1의 값을 ADCH에서 데이터를 받아들여 저장한다. */
		
		g_unAdcValue	=	(ucHighValue << 8) | ucLowValue ; /* 138페이지 원래 ADLAR을 0으로 맞춰 줬기 때문에 기본적으로 상위비트는 8,9번
		째에 걸려 있는 상태인데, 상위비트를 8번왼쪽으로 옴겨8번,9번 비트자리로 이동시켜주고 그상태에서 low와 high value끼리 or을 해주어
		8번, 9번째 비트자리에 상위비트 ADC8, ADC9를 넣어 준다. 그것을 ADCvalue에 넣어준다. 즉 10비트를 만들어주는것. 
		*제일 중요한것은 hihg,low value는 기본적으로 8비트인데 ADCvalue가 16비트이기때문에 대입연산으로 인해 묵시적 형변환이 생겨
		high,low value는 둘다 16비트로 변환이 되어 or연산을 해주어 비트의 자리를 이동 시킬수 있게 해준다. 만일 8비트씩 자리이동과
		or을 한다면 8번 왼쪽으로 옴긴결과는 그 8비트를 넘어가기때문에 그 비트의 수를 버리게된다!* */
	}
	//sei();
}


/*
ISR(USART1_RX_vect) // USART0 부분은 GPS와 연결 , USART1 부분은 블루투스 연결.
{
	BUF= UDR1;
	while(!(UCSR0A & 0x20)); // 이전에 송신한 데이터가 아직 송신 중이면 대기
	UDR0 = BUF;
	
}*/
ISR(USART1_RX_vect)
{
	
	
	RX = UDR1;                                                //GPS데이터를 1byte씩 RX변수에 저장
	
	if(RX == '$')                                             //시작 바이트가 '$'일 경우를 판별 맞으면
	{
		GPS_DATA[0] = '$';                                    //배열의 0번째 자리에 '$'를 저장
		GPScnt = 1;                                           //배열의 카운터를 1증가
	}
	else                                                      //시작 바이트 '$'다음부터
	{
		GPS_DATA[GPScnt] = RX;                                //배열에 수신된 GPS데이터를 저장
		
		if(GPS_DATA[GPScnt] == 0x0A)                          //수신된 데이터가 End Byte 0x0A='\n'일 경우
		{
			GPScnt1 = GPScnt;                                 //수신된 문자열 카운터를 다른 변수에 저장
			if(strncmp(NAME,GPS_DATA,6) == 0)                 //배열의 6번째 자리까지 문자열 "GPGGA"와 비교, 맞으면
			{
				memmove(GPS_DATA1,GPS_DATA,GPScnt1);          /*GPS_DATA1 배열에 메모리 복사하여 저장 strncpy 와 비슷한데, memmove로 사용가능, memcpy와도 같은기능
																memmove (move+1,move1+1,3) 이런식으면 move1(원본) +1주소부터 move(사본)+1주소에 3만큼 복사하겠다는것*/
				GPS_cut = strtok(GPS_DATA1,",");              /*GPS_Data1의 구분자 ','의 첫번째 주소값을 null로 변경 하여 
																반환 즉, GPS_DATA1이 첫주소부터','을 만나기 전까지 체크하여 ','을 만나자마자 바로 첫주소를 반환
																','을 null로 변환해주는것!*/
				for(i=0; GPS_cut!=null; i++)                  //null이 없을때까지
				{
					GPS_Dcut[i] = GPS_cut;                    /*포인트변수 GPS_DATA1의 첫 주소를 바로 Dcut 포인터배열에 반환, Dcut은 ','만나기전 첫 주소를 가리
																키고있다! null이 있을때 마다 i를 증가 시켜 주소값을 반환하여 GPS_Dcut배열에 저장*/
					GPS_cut = strtok(null,",");               //null의 첫번째 직후 주소값부터, ','이후 ','만나기전까지 확인하고, ','이후의 첫 주소를 반환!
				}
			}
			if(strncmp(NAME1,GPS_DATA,6) == 0)
			{
				memmove(GPS_DATA2,GPS_DATA,GPScnt1);
				GPS_cut = strtok(GPS_DATA2,",");
				for(i=0; GPS_cut!=null; i++)
				{
					GPS_Dcut1[i] = GPS_cut;
					GPS_cut = strtok(null,",");
				}
			}
			if(strncmp(NAME2,GPS_DATA,GPScnt1) == 0)
			{
				memmove(GPS_DATA3,GPS_DATA,GPScnt1);
				GPS_cut = strtok(GPS_DATA3,",");
				for(i=0; GPS_cut!=null; i++)
				{
					GPS_Dcut2[i] = GPS_cut;
					GPS_cut = strtok(null,",");
				}
			}
		}
		GPScnt++;
	}
	
}

void uart_set(void) //USART0은 GPS수신용 혹은 블루투스 수신용, USART1은 PC와와의 하이퍼 터미널을 위한것
{
	UBRR0H = 0;
	UBRR0L = 103; //전송속도는 9600bps
	UCSR0A = 0x00;
	UCSR0B = 0x98; // 수신완료 인터럽트 허용, receiver와 transmitter 허용
	UCSR0C = 0x06; // 8bit 단위 송수신
	
	UBRR1H = 0;
	UBRR1L = 103; //전송속도는 9600bps
	UCSR1A = 0x00;
	UCSR1B = 0x98; // receiver와 transmitter 허용
	UCSR1C = 0x06; // 8bit 단위 송수신
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

void print_String_a ( char *tring ) /*문자열. char string에 main에 있는 "Hello MCU World!" 라는 문자열이 매개변수로 들어온다.
									hello mcu world라는 것은 string이라는 변수에 배열처럼 작용을 한다. 즉 string이라는 번지에 H라는
									값이 들어가고, string+1이라는 번지에 e, string+2라는 번지에 l 등등으로 들어가 !까지 배열처럼
									값으로 들어가고, ! 다음에는 \0의 값이 마지막 번지로 들어가 종료를 의미한다.*/
{
	UDR0 = 'L';
	UDR0 = 'a';
	_delay_ms(100);
	UDR0 = ':';
	_delay_ms(9);
	while ( *tring != '\0' )	/* string의 포인트 즉 string번지의 값이 \0 이 아니면 while의 조건에 맞아 다음을 실행하고,
									write_data(string)의 값을 넣어 write_data의 함수로 넘어가 rs가 1로쓰여 ddram data로 쓰이고 data값
									을 string의 번지수의 값을 넣고 e=1이 였다가 다음 e=0인 순간 입력을 받아 LCD로 표현한다.*/
	{
						/*string의 번지를 +1씩 추가하여 다음 번지의 포인트 조건에 맞아 값을 출력 즉 처음에 H를 찍어냈으면 다음에는
								E, L L O 이런식으로 하기 위하여 ++을 함. */
		
		UDR0 = (*tring);
		tring ++;
		_delay_ms(100);
	}
	UDR0 = 32;
}

void print_String_b ( char *tring ) /*문자열. char string에 main에 있는 "Hello MCU World!" 라는 문자열이 매개변수로 들어온다.
									hello mcu world라는 것은 string이라는 변수에 배열처럼 작용을 한다. 즉 string이라는 번지에 H라는
									값이 들어가고, string+1이라는 번지에 e, string+2라는 번지에 l 등등으로 들어가 !까지 배열처럼
									값으로 들어가고, ! 다음에는 \0의 값이 마지막 번지로 들어가 종료를 의미한다.*/
{
	UDR0 = 'L';
	_delay_ms(9);
	UDR0 = 'o';
	_delay_ms(100);
	UDR0 = ':';
	_delay_ms(9);
	while ( *tring != '\0' )	/* string의 포인트 즉 string번지의 값이 \0 이 아니면 while의 조건에 맞아 다음을 실행하고,
									write_data(string)의 값을 넣어 write_data의 함수로 넘어가 rs가 1로쓰여 ddram data로 쓰이고 data값
									을 string의 번지수의 값을 넣고 e=1이 였다가 다음 e=0인 순간 입력을 받아 LCD로 표현한다.*/
	{
						/*string의 번지를 +1씩 추가하여 다음 번지의 포인트 조건에 맞아 값을 출력 즉 처음에 H를 찍어냈으면 다음에는
								E, L L O 이런식으로 하기 위하여 ++을 함. */
		
		UDR0 = (*tring);
		tring ++;
		_delay_ms(100);
	}
	UDR0 = 32;
}

int main (void)
{
	unsigned char ucLowValue	=	0; //ADC 변환 결과 값에서 ADCL 값을 저장할 변수를 선언
	unsigned char ucHighValue	=	0;	//ADC 변환 결과 값에서 ADCH 값을 저장할 변수를 선언.
	unsigned char line = 0;
	
	DDRE = 0x01; // PORTF 0번핀 출력설정
	DDRD = 0x08; // PORTD 0번핀 입력설정
	
	_delay_ms(10);
	NS_Rainbow_init(USELED, PB, 0);
	setBrightness(50); 	// 0,1(OFF) ~ 255

	TIMER_Init();
	ADC_Init ();		// PORTF = ADC1
	 
	 
	 uart_set();
	 _delay_ms(5);
	
	 sei();
	while (1)
	{
		
		print_String_a ( GPS_Dcut[2] );
		print_String_b ( GPS_Dcut[4] );
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


