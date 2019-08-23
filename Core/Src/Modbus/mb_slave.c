/*
 * mb_slave.c
 *
 *  Created on: 13 ���. 2019 �.
 *      Author: O.Novikov
 */

#include "ModBus/mb_parser.h"
#include "ModBus/mb.h"
#include "ModBus/mb_slave.h"
#include "ModBus/scoils.h"
#include "ModBus/sregs.h"
#include "string.h"

#ifdef MODBUS_SLAVE_BASE

ModbusError modbusBuildException(ModbusSlave *status, u8 function, ModbusExceptionCode code)
{
	//��������� ��������� ���������
	if(status == NULL)
		return MODBUS_ERROR_NULLPRT;

	//������������� �������� ������ � ����� ��������� ������
	status->lastException = code;

	//���� ������ ����������������� �� �� ��������� ����������
	ModbusParser *requestParser = (ModbusParser*)status->request.frame;
	if(requestParser != NULL && requestParser->base.address == 0)
	{
		status->response.length = 0;
		return MODBUS_OK;
	}

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		// �������������� ������ ������� �������
		status->response.frame = (u8*)calloc(5, sizeof(u8));
		if(status->response.frame == NULL)
			return MODBUS_ERROR_ALLOC;
	#else
		if((5 * sizeof(u8)) > MODBUS_STATIC_MEM_SLAVE_RESPONSE)
			return MODBUS_ERROR_ALLOC;
	#endif

	ModbusParser *exception = (ModbusParser*)status->response.frame;

	//��������� �������� � ����� �� ������
	exception->exception.address = status->address;
	exception->exception.function = (1 << 7) | function;
	exception->exception.code = code;
	exception->exception.crc = crc16(exception->frame, 3);

	//��������� ����� ������
	status->response.length = 5;

	//���������� �������� ��� ����������
	return MODBUS_ERROR_EXCEPTION;
}

ModbusError modbusParseRequest(ModbusSlave *status)
{
	//������ � ������������� ������� ����� modbus �� ������� �������
	ModbusError err = 0;

	// �������� ������������� ������� ��������� ��� ����������� ������
	if(status == NULL)
		return MODBUS_ERROR_NULLPRT;

	//����� ������� ������ �������
	status->response.length = 0;

	//���� � ��� ������������ ���������� ������ ������, �� �������� ���
	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		free(status->response.frame);
		status->response.frame = NULL;
	#endif

	status->parseError = MODBUS_FERROR_OK;

	//���� ������������ �������� ���������� ������ ����� �� �������� �� ������
	if(status->request.length < 4u || status->request.frame == NULL)
	{
		status->parseError = MODBUS_FERROR_LENGHT;
		return MODBUS_ERROR_PARSE;
	}

	//�������� crc
	u16 crc;
	memcpy(&crc, status->request.frame + status->request.length - 2, 2);

	//crc = modbusMatchEndian(crc);

	if(crc != crc16(status->request.frame, status->request.length - 2))
	{
		status->parseError = MODBUS_FERROR_CRC;
		return MODBUS_ERROR_PARSE;
	}

	ModbusParser *parser = (ModbusParser*)status->request.frame;
	//���� ����� �� ��������������� � ������ �� ������������� ���������� �������
	if(parser->base.address != status->address && parser->base.address != 0)
		return MODBUS_ERROR_OK;

	u8 functionMatch = 0, functionExec = 0;

	if(!functionMatch)
	{
		functionExec = 1;
		switch(parser->base.function)
		{
			#if defined(MODBUS_F01S) || defined(MODBUS_F02S)
				case 1:
				case 2:
					err = modbusParseRequest0102(status, parser);
					break;
			#endif

			#if defined(MODBUS_F03S) || defined(MODBUS_F04S)
				case 3:
				case 4:
					err = modbusParseRequest0304(status, parser);
					break;
			#endif

			#ifdef MODBUS_F05S
				case 5:
					err = modbusParseRequest05(status, parser);
					break;
			#endif

			#ifdef MODBUS_F06S
				case 6:
					err = modbusParseRequest06(status, parser);
					break;
			#endif

			#ifdef MODBUS_F15S
				case 15:
					err = modbusParseRequest15(status, parser);
					break;
			#endif

			#ifdef MODBUS_F16S
				case 16:
					err = modbusParseRequest16(status, parser);
					break;
			#endif
		}
	}

	//������� �� �����������
	if( !functionExec )
	{
		if(functionMatch)
			err = modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_FUNCTION, MODBUS_FERROR_NULLFUN);
		else
			err = modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_FUNCTION, MODBUS_FERROR_NOFUN);
	}

	return err;
}

ModbusError modbusSlaveInit(ModbusSlave *status)
{
	//������� ������������� slave modbus

	//�������� �� ����������� ���������
	if(status == NULL)
		return MODBUS_ERROR_NULLPRT;

	//����� �������� ������
	#ifndef MODBUS_STATIC_MEM_SLAVE_REQUEST
		status->request.frame = NULL;
	#else
		memset(status->response.frame, 0, MODBUS_STATIC_MEM_SLAVE_REQUEST);
	#endif

	status->request.length = 0;

	//����� ��������� ������
	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = NULL;
	#else
		memset(status->response.frame, 0, MODBUS_STATIC_MEM_SLAVE_RESPONSE);
	#endif

	status->response.length = 0;

	//����������� ���������� �� ����� ����� ����������������� ������
	if(status->address == 0)
		return MODBUS_ERROR_OTHER;

	//�������� ������������
	#ifdef MODBUS_REGISTER_CALLBACK
		if(status->registerCallback == NULL)
			status->registerCount = status->inputRegisterCount = 0;
	#else
		if(status->registerCount == 0 || status->registers == NULL)
		{
			status->registerCount = 0;
			status->registers = NULL;
		}
		if(status->inputRegisterCount == 0 || status->inputRegisters == NULL)
		{
			status->inputRegisterCount = 0;
			status->inputRegisters = NULL;
		}
	#endif

	#ifdef MODBUS_COIL_CALLBACK
		if(status->registerCallback == NULL)
			status->coilCount = status->discreteInputsCount = 0;
	#else
		if(status->coilCount == 0 || status->coils == NULL)
		{
			status->coilCount = 0;
			status->coils = NULL;
		}
		if(status->discreteInputCount == 0 || status->discreteInputs == NULL)
		{
			status->discreteInputCount = 0;
			status->discreteInputs = NULL;
		}
	#endif

	return MODBUS_ERROR_OK;
}

ModbusError modbusSlaveEnd(ModbusSlave *status)
{
	if(status == NULL)
		return MODBUS_ERROR_NULLPRT;

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		free(status->response.frame);
		status->response.length = 0;
	#endif

	return MODBUS_ERROR_OK;
}

#endif
