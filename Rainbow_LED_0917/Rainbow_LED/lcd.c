/*
 * lcd.c
 *
 * Created: 2016-09-17 오전 8:26:03
 *  Author: Administrator
 */ 
#include "lcd.h"

void write_Command ( unsigned char command )		//명령
{
	LCD_PORT_CONTROL	=	0x00;	/* E = 0, R/W = 0, Rs = 0  R/W 와 Rs가 0 0 이면 set DDRAM Adress 인데, 이제 ddram의 주소로 
										포트 데이터가 쓰이며 거기에 값을 넣으면 ddram의 주소로 간다.*/
	LCD_PORT_DATA	=	command;	/* Command */
	LCD_PORT_CONTROL	|=	0x04;	/* E = 1 */
	_delay_us(110);		/* Essential Delay for Simulator */

	LCD_PORT_CONTROL	&=	~(0x04);		/* E = 0 */
	_delay_us(110);
}


void write_Data ( unsigned char data )		//데이터
{
	LCD_PORT_CONTROL	=	0x00;	/* RS = 0, R/W = 0, E = 0 */
	LCD_PORT_CONTROL	|=	0x01;	/* RS = 1, DR->DDRAM */

	LCD_PORT_DATA	=	data;
	LCD_PORT_CONTROL	|=	0x04;	/* E = 1 */

	_delay_us(110);		/* Essential Delay for Simulator */

	LCD_PORT_CONTROL	&=	~(0x04);	/* E = 0 */						/*E이 1이였다가 0 으로 바뀌는 순간 (폴링에지) 일때 데이터값을
																		읽겠다는것, 읽고나서 바로 나타내주는것.*/
	_delay_us(110);
}


void print_String (unsigned char *string ) /*문자열. char string에 main에 있는 "Hello MCU World!" 라는 문자열이 매개변수로 들어온다.
									hello mcu world라는 것은 string이라는 변수에 배열처럼 작용을 한다. 즉 string이라는 번지에 H라는
									값이 들어가고, string+1이라는 번지에 e, string+2라는 번지에 l 등등으로 들어가 !까지 배열처럼
									값으로 들어가고, ! 다음에는 \0의 값이 마지막 번지로 들어가 종료를 의미한다.*/
{
	while ( *string != '\0' )	/* string의 포인트 즉 string번지의 값이 \0 이 아니면 while의 조건에 맞아 다음을 실행하고,
									write_data(string)의 값을 넣어 write_data의 함수로 넘어가 rs가 1로쓰여 ddram data로 쓰이고 data값
									을 string의 번지수의 값을 넣고 e=1이 였다가 다음 e=0인 순간 입력을 받아 LCD로 표현한다.*/
	{
		write_Data ( *string );
		string ++;				/*string의 번지를 +1씩 추가하여 다음 번지의 포인트 조건에 맞아 값을 출력 즉 처음에 H를 찍어냈으면 다음에는
								E, L L O 이런식으로 하기 위하여 ++을 함. */
	}
}


void print_String_a (unsigned char *tring ) /*문자열. char string에 main에 있는 "Hello MCU World!" 라는 문자열이 매개변수로 들어온다.
									hello mcu world라는 것은 string이라는 변수에 배열처럼 작용을 한다. 즉 string이라는 번지에 H라는
									값이 들어가고, string+1이라는 번지에 e, string+2라는 번지에 l 등등으로 들어가 !까지 배열처럼
									값으로 들어가고, ! 다음에는 \0의 값이 마지막 번지로 들어가 종료를 의미한다.*/
{
	_delay_ms(100);
	UDR0 = 'L';
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

void LCD_Init (void)
{
	LCD_DDR_DATA	=	0xFF;	//출력 포트 선택
	LCD_PORT_DATA	=	0x00; //초기화

	LCD_DDR_CONTROL	=	0x07;	// 신호선 3 PIN  출력설정 E R/W RS 설정
	LCD_PORT_CONTROL	=	0x00; //초기화
	_delay_us(110);

	LCD_PORT_CONTROL	&=	~(0x04);	/* E = 0 ,R/W = 0, RS= 0 즉 RS가 0이면 명령레지스터 선택, R/W = 0 이면 쓰기로 쓰겠다, E=0
										데이터를 쓰기로 쓰겠다 라는것.  */ 
	_delay_us(110);

	write_Command(0x38);			/* Function set data에 0x38을 준다는것 즉 0011 1000을 주는 것인데, function set을 선택을 해주어서 기능
									을 설정을 해주는 것이다. 왜냐하면 0011 1111로 ppt 자료를 보면 기능마다 우선순위가 정해져 있으므로
									상위비트부터 1로 설정이 되자마자 그 기능으로 설정이 되고 그 설정 안에서 비트에 해당하는 설정들을
									조작을 하여 나머지 설정을 조작한다는것! 즉 우선순위에 따라 상위비트로부터 1의 자리를 찾아서 그 1로부터
									그 기능을 조작한다는것. 
									DL8비트 설정, 2-line 설정, 5X8dot 설정.*/
	_delay_us(220);
			/* Function set command 즉 제어명령에 쓰는 이유가 레지스터들을 이용하여 명령을 주어 놓고 기능을
										사용 해놓고 나중에 거기에 맞는 아스키코드로 출력을 나타내주기 위해 먼저 제어명령을 조작함*/
	
			/* Function set 기능*/

	write_Command(0x0F);			/* Display ON 0000 1111로 디스플레이 on/off의 기능을 사용하고 0,1,2비트에 111을 주어 셋디스플레이,
										커서, 커서 깜박거림을 조작을 함. 그래서 셋디스플레이 on 즉 모든 디스플레이 on!, 커서on, 
										커서깜박거림 on을 설정해줌*/
	_delay_us(220);
	write_Command(0x02);			/* Cursor At Home ddram 있는 데이터는 바뀌지 않고 커서만 ddram 00주소로 이동 시킨다는 것*/
	_delay_ms(9);
	write_Command(0x01);			/* Clear Display 초기화 인데, 20h로 아스키코드로 보면 스페이스바이다. 즉 ddram 주소 00번지 자리
									부터 쭉 스페이스바를 치라는것. 어떤 데이터가 있어도 스페이스바를 치면 백스페이스 한것처럼 지워짐
									insult 되어있으니까. */
	_delay_ms(9);
	write_Command(0x06);			/* Entry mode set 엔트리모드는 insert 키 처럼 작동하는 것인데, db0, db1 즉 10인데 
									sh가 1이면 insert 모드, sh가 0이면 insert가 꺼진 밀리는 기능이다. i/d는 주소를 값을 증가 시키면 오른쪽
									으로 보내고 감소 시키면 왼쪽으로 가는것. LED를 오른쪽으로 쓸지, 왼쪽으로 쓸지 설정하는것. 1로 쓰면 오른
									쪽.
									*/
	_delay_us(220);
}