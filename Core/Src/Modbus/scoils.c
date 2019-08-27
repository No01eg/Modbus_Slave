/*
 * scoils.c
 *
 *  Created on: 19 ���. 2019 �.
 *      Author: O.Novikov
 */

#include "main.h"
#include "ModBus/mb.h"
#include "ModBus/mb_parser.h"
#include "ModBus/mb_slave.h"
#include "ModBus/scoils.h"
#include "string.h"

#if defined(MODBUS_F01S) || defined(MODBUS_F02S)
ModbusError modbusParseRequest0102(ModbusSlave *status, ModbusParser *parser)
{
	dataPage *map = &memMapCoils[0];
	//������ ��������� ���������� ��������� ��� ������
	// ������������ ������ �� �����������

	//������� �������� ����� ������
	u8 frameLength = 8;
	u16 i = 0;

	//�������� ������� ��������
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	if(parser->base.function != 1 && parser->base.function != 2)
	{
		status->parseError = MODBUS_FERROR_BADFUN;
		return MODBUS_ERROR_PARSE;
	}

	//� ������ ������� ����������� �������������, ��� ���������� ���� ����������
	// ��� ����� ������� ���������� ������ �������� ������
	if(parser->base.address == 0)
	{
		status->parseError = MODBUS_FERROR_BROADCAST;
		return MODBUS_ERROR_PARSE;
	}

	//�������� ������ �� ������������ �����
	if(status->request.length != frameLength)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//����� ������ � ����� ��� big endian || little endian
	u16 index = modbusMatchEndian(parser->request0102.index);
	u16 count = modbusMatchEndian(parser->request0102.count);

	//�������� ��� ������ � ���������
	if(count == 0 || count > 2000)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_COUNT);

	//��� �������������� ��������� �� ������� � ����������� �� �������
	if(parser->base.function == 1)
	{
		status->coils = memMapCoils[index >> 8].dataPoint;
		status->coilCount = memMapCoils[index >> 8].count;
		index = index & 0xff;
	}
	else if(parser->base.function == 2)
	{
		status->discreteInputs = memMapDigInput[index >> 8].dataPoint;
		status->discreteInputCount = memMapDigInput[index >> 8].count;
		index = index & 0xff;
	}

	//�������� �� ����������� ���������
	#ifdef MODBUS_COIL_CALLBACK
		if ( status->registerCallback == NULL )
			return modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#else
		if ( ( parser->base.function == 1 ? status->coils : status->discreteInputs ) == NULL )
			return modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#endif

	if ( index >= ( parser->base.function == 1 ? status->coilCount : status->discreteInputCount ) || \
		(u32) index + (u32) count > (u32) ( parser->base.function == 1 ? status->coilCount : status->discreteInputCount ) )
			return modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_RANGE );

	//Check if coils can be written (if using callback function)
	#ifdef LIGHTMODBUS_COIL_CALLBACK
		for ( i = 0; i < count; i++ )
			if ( status->registerCallback( MODBUS_REGQ_R_CHECK, datatype, index + i, 0, status->registerCallbackContext ) == 0 )
				return modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOREAD );
	#endif

	//�����
	frameLength = 5 + modbusBitToBytes(count);

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (uint8_t *) calloc( frameLength, sizeof( uint8_t ) ); //Reallocate response frame memory to needed memory
		if ( status->response.frame == NULL ) return MODBUS_ERROR_ALLOC;
	#else
		if ( frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE ) return MODBUS_ERROR_ALLOC;
		memset( status->response.frame, 0, frameLength );
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//��������� ������� ������ ��� ������
	builder->base.address = status->address;
	builder->base.function = parser->base.function;
	builder->response0102.length = modbusBitToBytes(count);

	//����������� ��������� � ����� ������
	for(i = 0; i < count; i++)
	{
		u8 coil;
		#ifdef MODBUS_COIL_CALLBACK
			coil = status->registerCallback( MODBUS_REGQ_R, datatype, index + i, 0, status->registerCallbackContext );
		#else
			if ( ( coil = modbusMaskRead( parser->base.function == 1 ? status->coils : status->discreteInputs, \
				modbusBitToBytes( parser->base.function == 1 ? status->coilCount : status->discreteInputCount ), i + index ) ) == 255 )
					return MODBUS_ERROR_OTHER;
		#endif

		//������ ������ ������
		if(modbusMaskWrite(builder->response0102.values, builder->response0102.length, i, coil) == 255)
			return MODBUS_ERROR_OTHER;
	}

	//������� crc
	u16 crc = crc16(builder->frame, frameLength - 2);
	memcpy(builder->frame + frameLength - 2, &crc, 2);

	//��������� ����� ������ - ����� �����
	status->response.length = frameLength;
	return MODBUS_ERROR_OK;
}
#endif

#ifdef MODBUS_F05S
ModbusError modbusParseRequest05(ModbusSlave *status, ModbusParser *parser)
{
	//������ ����������� ������

	//���������� ����� ������
	u8 frameLength = 8;

	//�������� ������� ��������
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	if(parser->base.function != 5)
	{
		status->parseError = MODBUS_FERROR_BADFUN;
		return MODBUS_ERROR_PARSE;
	}

	//�� �������� �� ����������������� �������
	if(parser->base.address == 0)
	{
		status->parseError = MODBUS_FERROR_OK;
		return MODBUS_ERROR_OK;
	}
	//�������� ������������ ����� ������
	if(status->request.length != frameLength)
		return modbusBuildExceptionErr(status, 5, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//����� ������
	u16 index = modbusMatchEndian(parser->request0506.index);
	u16 value = modbusMatchEndian(parser->request0506.value);

	if(value != 0x0000 || value != 0xff00)
		return modbusBuildExceptionErr(status, 5, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_VALUE);

	//Check if coils are accessible
	#ifdef MODBUS_COIL_CALLBACK
		if ( status->registerCallback == NULL )
			return modbusBuildExceptionErr( status, 5, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#else
		if ( status->coils == NULL )
			return modbusBuildExceptionErr( status, 5, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#endif

	//�������� ����� �� ������ �� ��������
	if(index >= status->coilCount)
		return modbusBuildExceptionErr(status, 5, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_RANGE);

	//Check if reg is allowed to be written
	#ifdef LIGHTMODBUS_COIL_CALLBACK
		if ( status->registerCallback( MODBUS_REGQ_R_CHECK, MODBUS_COIL, index, 0, status->registerCallbackContext ) == 0 )
			return modbusBuildExceptionErr( status, 5, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE );
	#else
		if ( modbusMaskRead( status->coilMask, status->coilMaskLength, index ) == 1 )
			return modbusBuildExceptionErr( status, 5, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE );
	#endif

	//�����
	frameLength = 8;

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (u8*) calloc( frameLength, sizeof( u8 ) ); //Reallocate response frame memory to needed memory
		if ( status->response.frame == NULL ) return MODBUS_ERROR_ALLOC;
	#else
		if ( frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE ) return MODBUS_ERROR_ALLOC;
		memset( status->response.frame, 0, frameLength );
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//����� �������� ���� ��������� ���������� ���������� ������
	#ifdef MODBUS_COIL_CALLBACK
		status->registerCallback( MODBUS_REGQ_W, MODBUS_COIL, index, value == 0xFF00, status->registerCallbackContext );
	#else
		if ( modbusMaskWrite( status->coils, modbusBitToBytes( status->coilCount ), index, value == 0xFF00 ) == 255 )
			return MODBUS_ERROR_OTHER;
	#endif

	//��������� ������� ������ ��� ������
	builder->base.address = status->address;
	builder->base.function = parser->base.function;
	builder->response0506.index = parser->request0506.index;
	builder->response0506.value = parser->request0506.value;

	//������� CRC
	builder->response0506.crc = crc16(builder->frame, frameLength - 2);

	//������������� ����� ����� - ����� �����
	status->response.length = frameLength;
	status->parseError = MODBUS_FERROR_OK;
	return MODBUS_ERROR_OK;
}
#endif

#ifdef MODBUS_F15S
ModbusError modbusParseRequest15(ModbusSlave *status, ModbusParser *parser)
{
	//������ ��������� ���������� �������

	//���������� ����� ������
	u16 i = 0;
	u8 frameLength;

	//�������� ������� ��������
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	//Do not respond when frame is broadcasted
	if ( parser->base.address == 0 )
	{
		status->parseError = MODBUS_FERROR_OK;
		return MODBUS_ERROR_OK;
	}

	//�������� ����������� ����� ������
	if(status->request.length >= 7u)
	{
		frameLength = 9 + parser->request15.length;
		if(status->request.length != frameLength)
			return modbusBuildExceptionErr(status, 15, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);
	}
	else
		return modbusBuildExceptionErr(status, 15, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//����� ������
	u16 index = modbusMatchEndian(parser->request15.index);
	u16 count = modbusMatchEndian(parser->request15.count);

	//�������� ������
	if(parser->request15.length == 0 || count == 0 || (modbusBitToBytes(count) != parser->request15.length) || count > 1968)
		return modbusBuildExceptionErr(status, 15, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_COUNT);

	//Check if coils are accessible
	#ifdef LIGHTMODBUS_COIL_CALLBACK
		if ( status->registerCallback == NULL )
			return modbusBuildExceptionErr( status, 15, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#else
		if ( status->coils == NULL )
			return modbusBuildExceptionErr( status, 15, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC );
	#endif

	if(index >= status->coilCount || (u32)index + (u32)count > (u32)status->coilCount)
		return modbusBuildExceptionErr(status, 15, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_RANGE);

	//Check for write protection
	#ifdef LIGHTMODBUS_COIL_CALLBACK
		for ( i = 0; i < count; i++ )
			if ( status->registerCallback( MODBUS_REGQ_W_CHECK, MODBUS_COIL, index + i, 0, status->registerCallbackContext ) == 0 )
				return modbusBuildExceptionErr( status, 15, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE );
	#else
		for ( i = 0; i < count; i++ )
			if ( modbusMaskRead( status->coilMask, status->coilMaskLength, index + i ) == 1 )
				return modbusBuildExceptionErr( status, 15, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE );
	#endif

	//�����
	frameLength = 8;

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (u8*) calloc( frameLength, sizeof( u8 ) ); //Reallocate response frame memory to needed memory
		if ( status->response.frame == NULL ) return MODBUS_ERROR_ALLOC;
	#else
		if ( frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE ) return MODBUS_ERROR_ALLOC;
		memset( status->response.frame, 0, frameLength );
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//After all possible exceptions write values to registers
	for ( i = 0; i < count; i++ )
	{
		u8 coil;
		if ( ( coil = modbusMaskRead( parser->request15.values, parser->request15.length, i ) ) == 255 ) return MODBUS_ERROR_OTHER;
			#ifdef MODBUS_COIL_CALLBACK
			status->registerCallback( MODBUS_REGQ_W, MODBUS_COIL, index + i, coil, status->registerCallbackContext );
		#else
			if ( modbusMaskWrite( status->coils, modbusBitToBytes( status->coilCount ), index + i, coil ) == 255 ) return MODBUS_ERROR_OTHER;
		#endif
	}

	//Set up basic response data
	builder->base.address = status->address;
	builder->base.function = parser->base.function;
	builder->response15.index = parser->request15.index;
	builder->response15.count = parser->request15.count;

	//Calculate crc
	builder->response15.crc = crc16( builder->frame, frameLength - 2 );

	//Set frame length - frame is ready
	status->response.length = frameLength;
	status->parseError = MODBUS_FERROR_OK;
	return MODBUS_OK;
}
#endif
