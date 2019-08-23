/*
 * mb.h
 *
 *  Created on: 8 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef MODBUS_MB_H_
#define MODBUS_MB_H_

#include "main.h"


/* ----------------------- Defines ------------------------------------------*/
#define MODBUS_LITTLE_ENDIAN
#define MODBUS_SLAVE_BASE

#define MODBUS_F01S
#define MODBUS_F02S
#define MODBUS_F03S
#define MODBUS_F04S
#define MODBUS_F05S
#define MODBUS_F06S
#define MODBUS_F15S
#define MODBUS_F16S
#define MODBUS_F22S


#define MODBUS_STATIC_MEM_SLAVE_RESPONSE 256
#define MODBUS_STATIC_MEM_SLAVE_REQUEST  256


/* ----------------------- Type definitions ---------------------------------*/
//������ ������� ����������
typedef enum modbusError
{
	MODBUS_ERROR_OK 	   = 0, // ������ ���
	MODBUS_ERROR_EXCEPTION = 1, // ���������, ���, ���-�� ��� �������� �� ���
	MODBUS_ERROR_ALLOC 	   = 2, // �������� � ����������� �������� � ������
	MODBUS_ERROR_OTHER     = 3, // ������ �������� � ������
	MODBUS_ERROR_NULLPRT   = 4,	// �����-�� �������� �� ���������
	MODBUS_ERROR_PARSE     = 5, // �������� � �������� ������
	MODBUS_ERROR_BUILD     = 6, // �������� � ����������� ������
	MODBUS_OK = MODBUS_ERROR_OK // �������� ������ ���
}ModbusError;

//������ ������� � ������
typedef enum modbusFrameError
{
	MODBUS_FERROR_OK = MODBUS_OK,	// ������ ���
	MODBUS_FERROR_CRC,				// �������� crc
	MODBUS_FERROR_LENGHT,			// �������� ����� ������
	MODBUS_FERROR_COUNT,			// ������� ������� ���������� ���������
	MODBUS_FERROR_VALUE,			// ������� ������� �������� (��� ������ 1 ����)
	MODBUS_FERROR_RANGE,			// �������� �������� ���������
	MODBUS_FERROR_NOSRC,			// ��� ����� ���� ������ �� ������������� �� ������� ��������� ������, �� ������� ��������
	MODBUS_FERROR_NOREAD,			// ��� ������� �� ������ ���� �� � ����� �� ������������� ���������
	MODBUS_FERROR_NOWRITE,			// ��� ������� �� ������ ���� �� � ������ �� ������������� ���������
	MODBUS_FERROR_NOFUN,			// ������� �� ��������������
	MODBUS_FERROR_BADFUN,			// ������������ ��� �������
	MODBUS_FERROR_NULLFUN,			// ������� �������������� ������������� ��� NULL
	MODBUS_FERROR_MISM_FUN,			// �������������� �������-������ �������
	MODBUS_FERROR_MISM_ADDR,		// �������������� �������-������ �������� ������
	MODBUS_FERROR_MISM_INDEX,		// �������������� �������-������ �������� �������
	MODBUS_FERROR_MISM_COUNT,		// �������������� ������-����� ���������� ��������
	MODBUS_FERROR_MISM_VALUE,		// �������������� �������-������ �������� ������
	MODBUS_FERROR_MISM_MASK,		// �������������� �������-������ �������� �����
	MODBUS_FERROR_BROADCAST			// ������� ����� �� ����������������� ���������
}ModbusFrameError;

//���� ���������� ������, ��������� � ���������
typedef enum modbusExceptionCode
{
	MODBUS_EXCEP_ILLEGAL_FUNCTION = 1, 	// �������� ��� �������
	MODBUS_EXCEP_ILLEGAL_ADDRESS  = 2,  // �������� ������ ��������
	MODBUS_EXCEP_ILLEGAL_VALUE	  = 3,	// �������� �������� ��������
	MODBUS_EXCEP_SLAVE_FAILURE	  = 4,  // ����������� �� ����� ���������� ������
	MODBUS_EXCEP_ACK			  = 5,  // Acknowledge
	MODBUS_EXCEP_NACK			  = 7,	// Negative acknowledge
}ModbusExceptionCode;


//��������� ����� ������
typedef enum modbusDataType
{
	MODBUS_HOLDING_REGISTER = 1,
	MODBUS_INPUT_REGISTER 	= 2,
	MODBUS_COIL				= 4,
	MODBUS_DISCRETE_INPUT 	= 8,
} ModbusDataType;

/* ����������� ������������ ����� ���� ��� ���������� n ��� �� ������ ������ (�� 16)*/
static inline u16 modbusBitToBytes(u16 n)
{
	return n != 0 ? (1 + ((n - 1) >> 3)) : 0;
}

/* ������ ����� ������� � word*/
static inline u16 modbusSwapEndian(u16 data)
{
	return (data << 8) | (data >> 8);
}

/*������ �������� word -> big Endian || little endian*/
#ifdef MODBUS_BIG_ENDIAN
	static inline u16 modbusMatchEndian(u16 data){return data;}
#else
	static inline u16 modbusMatchEndian(u16 data){return modbusSwapEndian(data);}
#endif

/* ������ n-�� ���� �� ������� */
extern u8 modbusMaskRead(const u8 *mask, u16 maskLenght, u16 bit);

/* ������ n-�� ���� � ������ */
extern u8 modbusMaskWrite(u8 *mask, u16 maskLenght, u16 bit, u8 value);

/* ������� CRC ������� */
extern u16 crc16(const u8* data, u8 lenght);

#include "ModBus/mb_slave.h"

extern ModbusSlave mbSlave;

extern void MB_SlaveInit(void);

#endif /* MODBUS_MB_H_ */
