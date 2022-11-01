/*
 * mpu9150.h
 *
 * Created: 2016-09-27 오후 2:08:41
 *  Author: Administrator
 */ 
// MPU6050 ACC+GYRO

#define SLV_ADDRESS		   0xD0	  // Slave Address
#define SELF_TEST_X        0x0D   // R/W
#define SELF_TEST_Y        0x0E   // R/W
#define SELF_TEST_Z        0x0F   // R/W
#define SELF_TEST_A        0x10   // R/W
#define SMPLRT_DIV         0x19   // R/W
#define CONFIG             0x1A   // R/W
#define GYRO_CONFIG        0x1B   // R/W
#define ACCEL_CONFIG       0x1C   // R/W
#define FF_THR             0x1D   // R/W
#define FF_DUR             0x1E   // R/W
#define MOT_THR            0x1F   // R/W
#define MOT_DUR            0x20   // R/W
#define ZRMOT_THR          0x21   // R/W
#define ZRMOT_DUR          0x22   // R/W
#define FIFO_EN            0x23   // R/W
#define I2C_MST_CTRL       0x24   // R/W
#define I2C_SLV0_ADDR      0x25   // R/W
#define I2C_SLV0_REG       0x26   // R/W
#define I2C_SLV0_CTRL      0x27   // R/W
#define I2C_SLV1_ADDR      0x28   // R/W
#define I2C_SLV1_REG       0x29   // R/W
#define I2C_SLV1_CTRL      0x2A   // R/W
#define I2C_SLV2_ADDR      0x2B   // R/W
#define I2C_SLV2_REG       0x2C   // R/W
#define I2C_SLV2_CTRL      0x2D   // R/W
#define I2C_SLV3_ADDR      0x2E   // R/W
#define I2C_SLV3_REG       0x2F   // R/W
#define I2C_SLV3_CTRL      0x30   // R/W
#define I2C_SLV4_ADDR      0x31   // R/W
#define I2C_SLV4_REG       0x32   // R/W
#define I2C_SLV4_DO        0x33   // R/W
#define I2C_SLV4_CTRL      0x34   // R/W
#define I2C_SLV4_DI        0x35   // R
#define I2C_MST_STATUS     0x36   // R
#define INT_PIN_CFG        0x37   // R/W
#define INT_ENABLE         0x38   // R/W
#define INT_STATUS         0x3A   // R
#define ACCEL_XOUT_H       0x3B   // R
#define ACCEL_XOUT_L       0x3C   // R
#define ACCEL_YOUT_H       0x3D   // R
#define ACCEL_YOUT_L       0x3E   // R
#define ACCEL_ZOUT_H       0x3F   // R
#define ACCEL_ZOUT_L       0x40   // R
#define TEMP_OUT_H         0x41   // R
#define TEMP_OUT_L         0x42   // R
#define GYRO_XOUT_H        0x43   // R
#define GYRO_XOUT_L        0x44   // R
#define GYRO_YOUT_H        0x45   // R
#define GYRO_YOUT_L        0x46   // R
#define GYRO_ZOUT_H        0x47   // R
#define GYRO_ZOUT_L        0x48   // R
#define EXT_SENS_DATA_00   0x49   // R
#define EXT_SENS_DATA_01   0x4A   // R
#define EXT_SENS_DATA_02   0x4B   // R
#define EXT_SENS_DATA_03   0x4C   // R
#define EXT_SENS_DATA_04   0x4D   // R
#define EXT_SENS_DATA_05   0x4E   // R
#define EXT_SENS_DATA_06   0x4F   // R
#define EXT_SENS_DATA_07   0x50   // R
#define EXT_SENS_DATA_08   0x51   // R
#define EXT_SENS_DATA_09   0x52   // R
#define EXT_SENS_DATA_10   0x53   // R
#define EXT_SENS_DATA_11   0x54   // R
#define EXT_SENS_DATA_12   0x55   // R
#define EXT_SENS_DATA_13   0x56   // R
#define EXT_SENS_DATA_14   0x57   // R
#define EXT_SENS_DATA_15   0x58   // R
#define EXT_SENS_DATA_16   0x59   // R
#define EXT_SENS_DATA_17   0x5A   // R
#define EXT_SENS_DATA_18   0x5B   // R
#define EXT_SENS_DATA_19   0x5C   // R
#define EXT_SENS_DATA_20   0x5D   // R
#define EXT_SENS_DATA_21   0x5E   // R
#define EXT_SENS_DATA_22   0x5F   // R
#define EXT_SENS_DATA_23   0x60   // R
#define MOT_DETECT_STATUS  0x61   // R
#define I2C_SLV0_DO        0x63   // R/W
#define I2C_SLV1_DO        0x64   // R/W
#define I2C_SLV2_DO        0x65   // R/W
#define I2C_SLV3_DO        0x66   // R/W
#define I2C_MST_DELAY_CTRL 0x67   // R/W
#define SIGNAL_PATH_RESET  0x68   // R/W
#define MOT_DETECT_CTRL    0x69   // R/W
#define USER_CTRL          0x6A   // R/W
#define PWR_MGMT_1         0x6B   // R/W
#define PWR_MGMT_2         0x6C   // R/W
#define FIFO_COUNTH        0x72   // R/W
#define FIFO_COUNTL        0x73   // R/W
#define FIFO_R_W           0x74   // R/W
#define WHO_AM_I           0x75   // R

//MPU9150 Compass
#define CMPS_XOUT_L        0x4A   // R
#define CMPS_XOUT_H        0x4B   // R
#define CMPS_YOUT_L        0x4C   // R
#define CMPS_YOUT_H        0x4D   // R
#define CMPS_ZOUT_L        0x4E   // R
#define CMPS_ZOUT_H        0x4F   // R

//Magnetometer Registers
//MPU9150_CMPS_ADDRESS
#define WHO_AM_I_AK8975A 0x00 // should return 0x48
#define INFO             0x01
#define AK8975A_ST1      0x02  // data ready status bit 0
#define AK8975A_ADDRESS  0x0C  // bypass address
#define AK8975A_XOUT_L	 0x03  // data
#define AK8975A_XOUT_H	 0x04
#define AK8975A_YOUT_L	 0x05
#define AK8975A_YOUT_H	 0x06
#define AK8975A_ZOUT_L	 0x07
#define AK8975A_ZOUT_H	 0x08
#define AK8975A_ST2      0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8975A_CNTL     0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
//#define AK8975A_ASTC     0x0C  // Self test control
#define AK8975A_ASAX     0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8975A_ASAY     0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8975A_ASAZ     0x12  // Fuse ROM z-axis sensitivity adjustment value


/*

//MPU9150 Compass
#define MPU9150_CMPS_ADDRESS       0x0C << 1 //cmps address with end write bit
#define CMPS_XOUT_L        0x4A   // R
#define CMPS_XOUT_H        0x4B   // R
#define CMPS_YOUT_L        0x4C   // R
#define CMPS_YOUT_H        0x4D   // R
#define CMPS_ZOUT_L        0x4E   // R
#define CMPS_ZOUT_H        0x4F   // R*/