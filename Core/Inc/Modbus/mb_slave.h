/*
 * mb_slave.h
 *
 *  Created on: 13 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef MODBUS_MB_SLAVE_H_
#define MODBUS_MB_SLAVE_H_

#include "ModBus/mb_parser.h"
#include "ModBus/mb.h"

#ifdef MODBUS_SLAVE_BASE

	#ifdef MODBUS_SLAVE_USER_FUNCTIONS
		struct modbusSlave;

		typedef struct modbusSlaveUserFunction
		{
			u8 function; // ��� �������
			//��������� �� ���������������� ������� ��������� ������
			ModbusError (*handler)(struct modbusSlave *status, ModbusParser *parser);
		}ModbusSlaveUserFunction;
	#endif


/*
 * ����������� ������ ���������� - ������ � ������
 * */
typedef struct modbusSlave
{
	u8 address; //��������� ������ ����������


	// ������ ���������
	#ifndef MODBUS_REGISTER_CALLBACK
		u16 *registers; 		// ��������� �� ������� ������
		u16 *inputRegisters; 	// ��������� �� ������� ������� ������
		u8  *registerMask;		// �������� ����� ��� ������� �������� (������ ��� ������������� ��������)
		u16 registerMaskLegth;	// �������� ����� ��� �������� (1 ���� �������� 8 ��������� bool)
	#endif
	u16 registerCount; 			// ���������� ��������� ������������ ����������
	u16 inputRegisterCount; 	// ���������� ������� ��������� ������������ ����������

	#ifndef MODBUS_COIL_CALLBACK
		u8  *coils;				// ��������� �� ���������� ������/�����
		u8  *discreteInputs;	// ��������� �� ���������� �����
		u8  *coilMask;			// ����� ��� ������  �������� IO
		u16 coilMaskLength;	// ����� � ������
	#endif
	u16 coilCount;				// ���������� ���������� IO
	u16 discreteInputCount;	// ���-�� ���������� ������

	// ��� ��������� ������
	ModbusExceptionCode lastException;

	// ����� ����������� ��� ������
	ModbusFrameError parseError;

	//��������� ���������� ����� ������ + �����
	struct
	{
		#ifdef MODBUS_STATIC_MEM_SLAVE_RESPONSE
			u8 frame[MODBUS_STATIC_MEM_SLAVE_RESPONSE];
		#else
			u8 *frame;
		#endif
		u8 length;
	}response;

	// ��������� ���������� ����� ������� + ����� ������
	struct
	{
		#ifdef MODBUS_STATIC_MEM_SLAVE_REQUEST
			u8 frame[MODBUS_STATIC_MEM_SLAVE_REQUEST];
		#else
			u8 *frame;
		#endif
		u8 length;
	}request;
}ModbusSlave;

//��������� �������

/* �������� ������ �� ������
 * ������ ����� ����������� ���� � ��� �� ����������������� ������
 * */
extern ModbusError modbusBuildException(ModbusSlave *status, u8 function, ModbusExceptionCode code);


/*
 * ���������� ������� ������ �������
 * */
extern ModbusError modbusParseRequest(ModbusSlave *status);

/*
 * ������������� ��������� ������������ ������
 * */
extern ModbusError modbusSlaveInit(ModbusSlave *status);

/*
 * ��������������� ��������� ������������ ������
 * */
extern ModbusError modbusSlaveEnd(ModbusSlave *status);

/*
 * ��������� ������
 * ������� ���������� � ���������� ����������� ������ � ���������
 * */
static inline ModbusError modbusBuildExceptionErr(ModbusSlave *status, u8 function, ModbusExceptionCode code, ModbusFrameError parseError)
{
	if(status == NULL)
		return MODBUS_ERROR_NULLPRT;
	status->parseError = parseError;
	ModbusError  err = modbusBuildException(status, function, code);
	if(err == MODBUS_ERROR_OK)
		return MODBUS_ERROR_PARSE;
	else
		return err;
}

#endif

///XXX ��������� ���� ����� �� ������������ ������

/*! \ingroup modbus
 * \brief Modbus serial transmission modes (RTU/ASCII).
 *
 * Modbus serial supports two transmission modes. Either ASCII or RTU. RTU
 * is faster but has more hardware requirements and requires a network with
 * a low jitter. ASCII is slower and more reliable on slower links (E.g. modems)
 */
typedef enum
{
    MB_RTU,                     /*!< RTU transmission mode. */
    MB_ASCII,                   /*!< ASCII transmission mode. */
    MB_TCP                      /*!< TCP mode. */
} eMBMode;



/*��������� ������� ������ */
typedef struct
{
	u8 MBActivity; 				/* 1 - is active; 0 - desactive */
	eMBMode MBModeType;			/* 0 - RTU; 1 - TCP */
	u8 MBLocalAddr;				/* */
	u8 reserve;					/* ��� ������������ � �������*/
}eMBConfig;

#endif /* MODBUS_MB_SLAVE_H_ */
