/*
 * MPU9150_0927.c
 *
 * Created: 2016-09-27 오후 2:01:01
 * Author : Administrator
 */ 

#include <util/twi.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpu9150.h"
//#include "i2cmaster.h"

#define PERIOD	10		// ms
#define FS_SEL  65.5
#define AFS_SEL	8192

#define F_CPU 16000000UL
#define BPS  19200 //BPS 19200 설정
#define BAUD ((16000000/16/BPS)-1) // UBRRn 구하기 51

void init_devices(void);
void port_init(void);
void I2C_write(unsigned char slv_addr, unsigned char address, unsigned char byte);
int I2C_read(unsigned char slv_addr, unsigned char regi);

void getRawData();
void convertRawData(void);
int calc2comp(unsigned int data);
void getDegree();

void mpu9150_initialize();

int k = 0;
int n = 0;
int data_adc = 0;
int a = 0;
int b = 0;

int buffer[18];
int ax, ay, az, gx, gy, gz, mx, my, mz,ta = 0; // raw data
int gyro_x, gyro_y, gyro_z = 0; // converted data
int accel_x, accel_y, accel_z = 0; // converted data
int angle_x, angle_y, angle_z = 0;

int dgx = 0, dgy = 0;
unsigned char str1[1000];
unsigned char turn;
int main(void)
{
	
	init_devices();
	mpu9150_initialize();
	DDRA = 0xff;
	PORTA = 0x00;
	DDRB = 0xff;
	PORTB = 0xff;
	
	while(1){
		
		
		getDegree();
		_delay_ms(20);
	
		if(turn == 10)
		{
			if((abs(gx)>=1000)||(abs(gy)>=1000)||(abs(gz)>=1000))
			{
				PORTA = 0x01;
				_delay_ms(500);
				PORTA = 0x00;
			}
			if((abs(ta)>=5000))
			{
				PORTB = 0xFE;	//3색 LED 빨강
				_delay_ms(500);
				PORTB = 0xFF;
			}
			else if((abs(ta)>=3000))
			{
				PORTB = 0xFD;	//3색 LED 초록
				_delay_ms(500);
				PORTB = 0xFF;
			}
			else
			{
				PORTB = 0xFB;	//3색 LED 파랑
				_delay_ms(500);
				PORTB = 0xFF;
			}
		}
		
		turn ++;
	}
	return 0;
}

void init_devices(void)
{

	TWCR = (1<<TWEN);	// TWI Control Reg, TWI Enable
	TWSR = 0x00;		// TWI Status Reg, initialization, prescaler : 1
	TWBR = 0x12;		// TWI Bit Rate Reg, Fscl = 400KHz(Fcpu/(16+2*TWBR*Prescaler) = Fscl)
	
}

void I2C_write(unsigned char slv_addr, unsigned char address, unsigned char byte)// master
{
	// 시작신호-> 슬레이브 어드레스찾기-> 데이터쓸 위치(슬레이브의 어드레스)-> 쓸데이터
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)); // Start
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_START);
	_delay_ms(20);
	
	TWDR = slv_addr;  // 슬레이브 어드레스 입력
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MT_SLA_ACK); // SLA+W transmitted, ACK received
	_delay_ms(20);
	
	TWDR = address;  // 슬레이브 센서내의 레지스터 어드레스 선택
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MT_DATA_ACK); // data transmitted, ACK received
	_delay_ms(20);
	
	TWDR = byte;  // 쓰고자 하는 데이터
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MT_DATA_ACK); // data transmitted, ACK received
	_delay_ms(20);
	
	TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN)); // End
	_delay_ms(20);
}
int I2C_read(unsigned char slv_addr, unsigned char regi)  //master
{
	//시작신호-> 슬레이브 어드레스찾기-> 데이터쓸 위치(슬레이브의 어드레스)	-> repeated start -> Master Receive 모드 설정 -> 수신확인 -> stop
	unsigned int result;
	
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)); // Start
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_START);
	_delay_ms(20);
	
	TWDR = slv_addr;  //슬레이브 어드레스 입력?
	
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MT_SLA_ACK);
	_delay_ms(20);
	
	TWDR = regi;  // 슬레이브 센서내의 레지스터 어드레스 선택?
	
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MT_DATA_ACK);
	_delay_ms(20);
	
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)); // repeated start
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_REP_START);
	_delay_ms(20);
	
	TWDR = slv_addr+1; // Master Receiver Mode로 설정
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MR_SLA_ACK); // SLA+R transmitted, ACK received
	_delay_ms(20);
	
	TWCR = ((1<<TWINT)|(1<<TWEN));
	while(((TWCR & 0x80) == 0x00) || (TWSR & 0xF8) != TW_MR_DATA_NACK); // 데이터 수신확인 신호 없음 수신?
	result = TWDR; //데이터 기록
	_delay_ms(20);
	
	TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN));  // stop
	return result; // 데이터 반환
	_delay_ms(20);
}

void getRawData()
{
	/*// ACCEL
	buffer[0] = I2C_read(SLV_ADDRESS,ACCEL_XOUT_H);
	buffer[1] = I2C_read(SLV_ADDRESS,ACCEL_XOUT_L);
	buffer[2] = I2C_read(SLV_ADDRESS,ACCEL_YOUT_H);
	buffer[3] = I2C_read(SLV_ADDRESS,ACCEL_YOUT_L);
	buffer[4] = I2C_read(SLV_ADDRESS,ACCEL_ZOUT_H);
	buffer[5] = I2C_read(SLV_ADDRESS,ACCEL_ZOUT_L);*/
	
	//GYRO
	buffer[6] = I2C_read(SLV_ADDRESS,GYRO_XOUT_H);
	buffer[7] = I2C_read(SLV_ADDRESS,GYRO_XOUT_L);
	buffer[8] = I2C_read(SLV_ADDRESS,GYRO_YOUT_H);
	buffer[9] = I2C_read(SLV_ADDRESS,GYRO_YOUT_L);
	buffer[10] = I2C_read(SLV_ADDRESS,GYRO_ZOUT_H);
	buffer[11] = I2C_read(SLV_ADDRESS,GYRO_ZOUT_L);
	
	buffer[18] = I2C_read(SLV_ADDRESS,TEMP_OUT_H);
	buffer[19] = I2C_read(SLV_ADDRESS,TEMP_OUT_L);
	

	/*buffer[12] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_02);
	buffer[13] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_03);
	buffer[14] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_04);
	buffer[15] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_05);
	buffer[16] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_06);
	buffer[17] = I2C_read(SLV_ADDRESS,EXT_SENS_DATA_07);*/
	//I2C_write(SLV_ADDRESS,INT_PIN_CFG, 0x00);// bypass enable
	
	ax = (buffer[0] << 8) + buffer[1];
	ay = (buffer[2] << 8) + buffer[3];
	az = (buffer[4] << 8) + buffer[5];
	gx = (buffer[6] << 8) + buffer[7];
	gy = (buffer[8] << 8) + buffer[9];
	gz = (buffer[10] << 8) + buffer[11];
	ta = (buffer[18] << 8) + buffer[19];
	/*mx = (buffer[12] << 8) + buffer[13];
	my = (buffer[14] << 8) + buffer[15];
	mz = (buffer[16] << 8) + buffer[17];*/
	
}

void convertRawData(void)
{
	// Gyro
	gyro_x = gx/FS_SEL; // 65535/35.5/2 = 923.028
	gyro_y = gy/FS_SEL;
	gyro_z = gz/FS_SEL;
	
	// Accel
	accel_x = ax;
	accel_y = ay;
	accel_z = az;
}
void getDegree()
{
	getRawData();
	convertRawData();
	angle_x = atan2(accel_x,accel_z)*180/M_PI;
	angle_y = atan2(accel_y,accel_z)*180/M_PI;
}

void mpu9150_initialize()
{
	I2C_write(SLV_ADDRESS,PWR_MGMT_1,0x00);// 0x6b,0x6c 레지스터에 0x00값을 넣어줘야 센서 ON!
	I2C_write(SLV_ADDRESS,PWR_MGMT_2,0x00);
	I2C_write(SLV_ADDRESS,GYRO_CONFIG,0x08);// gyro 설정, 최대 500도/s로 설정
	I2C_write(SLV_ADDRESS,ACCEL_CONFIG,0x08);// accel 설정, 최대 4g 로 설정
	
	//I2C_write(SLV_ADDRESS,INT_PIN_CFG, 0x02);// bit2 BYPASS_EN 1
	//write_setting(AK8975A_ADDRESS,AK8975A_CNTL,0x0F); // self test
	
	
	//
	//delay(10);
	
	//delay(10);
	//I2C_write(AK8975A_ADDRESS,AK8975A_CNTL,0x01);
	//delay(10);
}
