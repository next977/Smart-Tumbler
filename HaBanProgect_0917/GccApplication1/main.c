/*
* GYRO_data.c
*
* Created: 2016-06-21 오후 4:03:31
* Author : admin
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <util/twi.h>
#define USELED	64						// Using LED Value
#define F_CPU 16000000UL
#define BPS  19200 //BPS 19200 설정
#define BAUD ((16000000/16/BPS)-1) // UBRRn 구하기 51

unsigned char str1[50];
unsigned char str2[50];
unsigned char G_X_h, G_X_l, G_Y_h, G_Y_l, G_Z_h, G_Z_l,temp_h,temp_l;
int G_X, G_Y, G_Z,temp;

void Uart_Init(void)
{
	UCSR0A = 0x00;
	UCSR0B = 0x18; // UART 수신 기능 활성화, UART 송신 기능 활성화, 8비트 데이터 비트 설정
	UCSR0C = 0x06; // 비동기 동작, 패리티 없음, Stop bit : 1, 8비트 데이터 비트 설정
	UBRR0H = BAUD >> 8;
	UBRR0L = BAUD;
	//UBRR0L = 103;
}



void Uart_Tx(unsigned char data)
{
	while( !(UCSR0A & 0x20) );
	UDR0 = data;
}
void Uart_Tx_Str(unsigned char *str)
{
	while(*str)
	Uart_Tx(*str++);
}
unsigned char Uart_Rx(void)
{
	while( !(UCSR0A & 0x80) );

	return UDR0;
}
unsigned int TWI_RD(unsigned char slv_add, unsigned char regi)
{
	_delay_ms(1);
	unsigned int result;

	TWCR = 0xa4;                    //TWINT의 값 1, TWSTA의 값을 1로 설정하면 마스터가 되고 버스상에 START조건을 생성
	//TWEN의 값을 1로 설정하여 TWI를 활성화
	while((TWCR & 0x80) == 0x00 || (TWSR & 0xf8) != 0x08);   //TWINT의 값을 클리어 함으로써 TWI동작 시작

	TWDR = slv_add;                 //슬레이브 어드레스
	TWCR = 0x84;      //ACK
	while((TWCR & 0x80) == 0x00);

	TWDR = regi;      //reg 선택
	TWCR = 0x84;      //ACK
	while((TWCR & 0x80) == 0x00);

	TWCR = 0xa4;      //restart
	while((TWCR & 0x80) == 0x00);

	TWDR = slv_add+1;
	TWCR = 0x84;
	while((TWCR & 0x80) == 0x00);

	TWCR = 0x84;      //NOACK
	while((TWCR & 0x80) == 0x00);
	result = TWDR;

	TWCR = 0x94;      //stop

	return result;
}
unsigned int TWI_WR(unsigned char slv_add, unsigned char regi, unsigned char data)
{
	_delay_ms(1);
	unsigned int result;

	TWCR = 0xa4;      //start
	while((TWCR & 0x80) == 0x00);
	//Uart_Tx('1');
	TWDR = slv_add;
	TWCR = 0x84;      //ACK
	while((TWCR & 0x80) == 0x00);
	//Uart_Tx('2');
	TWDR = regi;      //reg 선택
	TWCR = 0x84;      //ACK
	while((TWCR & 0x80) == 0x00);
	//Uart_Tx('3');
	TWDR = data;      //reg 선택
	TWCR = 0x84;      //ACK
	while((TWCR & 0x80) == 0x00);
	//Uart_Tx('4');
	TWCR = 0x94;      //stop
	return result;

}
void TWI_Init(void)
{
	TWBR = 12;      //속도 설정
	TWSR = 0;      //상태레지스터 초기화
	TWCR = 0x01;   //제어레지스터 초기화
	_delay_ms(100);
}
int main(void)
{
	DDRD = 0x03;
	PORTD = 0x03;
	DDRA = 0xff;
	PORTA = 0x00;
	DDRB = 0xff;
	PORTB = 0x00;
	unsigned char turn=0;
	unsigned char rx_data = 0; //수신 데이터 저장할 변수
	
	Uart_Init(); //UART 통신 설정
	TWI_Init();
	_delay_ms(10);

	TWI_WR(0xd0, 0x6b, 0x00); //스탠바이 ON
	_delay_ms(100);
	TWI_WR(0xd0, 0x6c, 0x00);
	_delay_ms(100);
	TWI_WR(0xd0,0x1b,0x08);// gyro 설정, 최대 500도/s로 설정
	_delay_ms(100);
	TWI_WR(0xd0,0x1c,0x08);// accel 설정, 최대 4g 로 설정
	_delay_ms(100);
	
	while(1)
	{
	
		G_X_h = TWI_RD(0xd0, 0x43);
		G_X_l = TWI_RD(0xd0, 0x44);
		G_Y_h = TWI_RD(0xd0, 0x45);
		G_Y_l = TWI_RD(0xd0, 0x46);
		G_Z_h = TWI_RD(0xd0, 0x47);
		G_Z_l = TWI_RD(0xd0, 0x48);

		G_X = ((G_X_h << 8) | G_X_l);
		G_Y = ((G_Y_h << 8) | G_Y_l);
		G_Z = ((G_Z_h << 8) | G_Z_l);

		temp_h = TWI_RD(0xd0, 0x41);
		temp_l = TWI_RD(0xd0, 0x41);
		temp = ((temp_h << 8) | temp_l);
		if(turn==10)
		{
			
			if((abs(G_X)>=1000)||(abs(G_Y)>=1000)||(abs(G_Z)>=1000))
			{
				PORTA = 0x01;
				_delay_ms(500);
				PORTA = 0x00;	
				sprintf((char*)str1, " G_X = %3.2d, G_Y = %3.2d, G_Z = %3.2d, Temp = %3.2d", G_X,G_Y,G_Z,temp);
				Uart_Tx_Str(str1);
				Uart_Tx('\n');
				Uart_Tx('\r');
			}
			/*if(abs(temp))
			{
				
			}
			*/
			turn=0;
			
		}
		
	
		turn++;
		

	}
	return 0;
}

