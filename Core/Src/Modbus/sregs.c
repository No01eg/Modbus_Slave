/*
 * sregs.c
 *
 *  Created on: 20 авг. 2019 г.
 *      Author: O.Novikov
 */

#include "main.h"
#include "ModBus/mb.h"
#include "ModBus/mb_parser.h"
#include "ModBus/mb_slave.h"
#include "ModBus/sregs.h"
#include "string.h"

#if defined(MODBUS_F03S) || defined(MODBUS_F04S)
ModbusError modbusParseRequest0304(ModbusSlave *status, ModbusParser *parser)
{
	//чтение множества входных или выходных регистров

	//сбрасываем длину пакета приема
	u8 frameLength = 8;
	u8 i = 0;

	//проверка существования управляющих структур
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	if(parser->base.function != 3 && parser->base.function != 4)
	{
		status->parseError = MODBUS_FERROR_BADFUN;
		return MODBUS_ERROR_PARSE;
	}

	//пропускаем разбор если широковещательная команда
	if(parser->base.address == 0)
	{
		status->parseError = MODBUS_FERROR_BROADCAST;
		return MODBUS_ERROR_PARSE;
	}

	//проверка длины пакета
	if(status->request.length != frameLength)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//обмен байтами в словах
	u16 index = modbusMatchEndian(parser->request0304.index);
	u16 count = modbusMatchEndian(parser->request0304.count);

	//TODO при указании адреса разбивать по секциям, старший байт определяет какой массив будет использоваться,
	// а младший байт указывает смещение в самом массиве, толкьо как это сделать?

	//проверка на количество в 1 пакете
	if(count == 0 || count > 125)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_COUNT);

	//Тут переопределяем указатели на массивы в зависимости от индекса
	if(parser->base.function == 3)
	{
		status->registers = memMapRegs[index >> 8].dataPoint;
		status->registerCount = memMapRegs[index >> 8].count;
		index = index & 0xff;
	}
	else if(parser->base.function == 4)
	{
		status->inputRegisters = memMapInputsRegs[index >> 8].dataPoint;
		status->inputRegisterCount = memMapInputsRegs[index >> 8].count;
		index = index & 0xff;
	}

	//проверка доступности регистров
	#ifdef MODBUS_REGISTER_CALLBACK
		if(status->registerCallback == NULL)
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC);
	#else
		if((parser->base.function == 3 ? status->registers : status->inputRegisters) == NULL)
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC);
	#endif

	if(index >= (parser->base.function == 3 ? status->registerCount : status->inputRegisterCount) ||
		(u32)index + (u32)count > (u32)(parser->base.function == 3 ? status->registerCount : status->inputRegisterCount))
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_RANGE);

	//проверка всех регистров на возможность чтения (если активен callback)
	#ifdef MODBUS_REGISTER_CALLBACK
		for ( i = 0; i < count; i++ )
			if ( status->registerCallback( MODBUS_REGQ_R_CHECK, datatype, index + i, 0, status->registerCallbackContext ) == 0 )
				return modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOREAD );
	#endif

	//ответ
	frameLength = 5 + (count << 1);

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (u8*)calloc(frameLength, sizeof(u8));
		if(status->response.frame == NULL)
			return MODBUS_ERROR_ALLOC;
	#else
		if(frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE)
			return MODBUS_ERROR_ALLOC;
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//установка базовых значений
	builder->response0304.address = status->address;
	builder->response0304.function = parser->request0304.function;
	builder->response0304.length = count << 1;

	//копирование регистров в пакет ответа
	#ifdef MODBUS_REGISTER_CALLBACK
		for(i = 0; i < count; i++)
			builder->response0304.values[i] = modbusMatchEndian(status->registerCallback(MODBUS_REGQ_R, datatype, index + i, 0, status->registerCallbackContext));
	#else
		for(i=0; i < count; i++)
			builder->response0304.values[i] = modbusMatchEndian((parser->base.function == 3 ? status->registers : status->inputRegisters)[index + i]);
	#endif

	//Подсчет crc
	u16 crc = crc16(builder->frame, frameLength - 2);
	memcpy(builder->frame + frameLength - 2, &crc, 2);

	//установка длины пакета - пакет готов к отправке
	status->response.length = frameLength;
	status->parseError = MODBUS_FERROR_OK;
	return MODBUS_ERROR_OK;
}
#endif

#ifdef MODBUS_F06S
ModbusError modbusParseRequest06(ModbusSlave *status, ModbusParser *parser)
{
	//обновление длины пакета
	u8 frameLength = 8;

	//проверка наличия структур
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	//проверка корректности длины пакета
	if(status->request.length != frameLength)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//не стоит дальше разбирать если это широковещательный пакет
	if ( parser->base.address == 0 )
	{
		status->parseError = MODBUS_FERROR_OK;
		return MODBUS_ERROR_OK;
	}

	//обмен байтами в слове
	u16 index = modbusMatchEndian(parser->request0506.index);
	u16 value = modbusMatchEndian(parser->request0506.value);

	//проверка диапазона доступных ячеек
	#ifdef MODBUS_REGISTER_CALLBACK
		if(status->registerCallback(MODBUS_REGQ_W_CHECK, MODBUS_HOLDING_REGISTER, index, 0, status->registerCallbackContext) == 0)
			return modbusBuilderExceptionErr(status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE);
	#else
		if(modbusMaskRead(status->registerMask, status->registerMaskLegth, index) == 1)
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE);
	#endif

	//ответ
	frameLength = 8;

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (u8*)calloc(frameLength, sizeof(u8));
		if(status->response.frame == NULL)
			return MODBUS_ERROR_ALLOC;
	#else
		if(frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE)
			return MODBUS_ERROR_ALLOC;
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//После всех проверок записываем данные
	#ifdef MODBUS_REGISTER_CALLBACK
		status->registerCallback(MODBUS_REGQ_W, MODBUS_HOLDING_REGISTER, index, value, status->registerCallbackContext);
	#else
		status->registers[index] = value;
	#endif

	//установка базовых значений в пакете
	builder->response0506.address = status->address;
	builder->response0506.function = parser->request0506.function;
	builder->response0506.index = parser->request0506.index;
	builder->response0506.value = parser->request0506.value;

	//подсчет CRC
	builder->response0506.crc = crc16(builder->frame, frameLength - 2);

	//установка длины пакета - пакет готов
	status->response.length = frameLength;
	status->parseError = MODBUS_FERROR_OK;

	return MODBUS_ERROR_OK;
}
#endif

#ifdef MODBUS_F16S
ModbusError modbusParseRequest16(ModbusSlave *status, ModbusParser *parser)
{
	//Запись множества выходных регистров

	//обновление длины пакета
	u8 i = 0;
	u8 frameLength;

	//Проверка наличия структур управления
	if(status == NULL || parser == NULL)
		return MODBUS_ERROR_NULLPRT;

	//не обрабатывать если адресс широковещательный
	if ( parser->base.address == 0 )
	{
		status->parseError = MODBUS_FERROR_OK;
		return MODBUS_ERROR_OK;
	}

	//проверка длины пакета
	if(status->request.length >= 7u)
	{
		frameLength = 9 + parser->request16.length;
		if(status->request.length != frameLength)
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);
	}
	else
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_LENGHT);

	//обмен байтами в словах
	u16 index = modbusMatchEndian(parser->request16.index);
	u16 count = modbusMatchEndian(parser->request16.count);

	//проверка данных
	if(parser->request16.length == 0 || count == 0 || count != (parser->request16.length >> 1) || count > 123)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_VALUE, MODBUS_FERROR_COUNT);

	//проверка доступности регистров
	#ifdef MODBUS_REGiSTER_CALLBACK
		if(status->registerCallback == NULL)
			return modbusBuilderExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC);
	#else
		if(status->registers == NULL)
			return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_NOSRC);
	#endif

	if(index >= status->registerCount || (u32)index + (u32)count > (u32)status->registerCount)
		return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_ILLEGAL_ADDRESS, MODBUS_FERROR_RANGE);

	//проверка защиты записи
	#ifdef MODBUS_REGISTER_CALLBACK
		for(i = 0; i < count; i++)
			if(status->registerCallback(MODBUS_REGQ_W_CHECK, MODBUS_HOLDING_REGISTER, index + i, 0, status->registerCallbackContext) == 0)
				return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE);
	#else
		for(i = 0; i < count; i++)
			if(modbusMaskRead(status->registerMask, status->registerMaskLegth, index + i) == 1)
				return modbusBuildExceptionErr(status, parser->base.function, MODBUS_EXCEP_SLAVE_FAILURE, MODBUS_FERROR_NOWRITE);
	#endif

	//ответ
	frameLength = 8;

	#ifndef MODBUS_STATIC_MEM_SLAVE_RESPONSE
		status->response.frame = (u8*)calloc(frameLength, sizeof(u8));
		if(status->response.frame == NULL)
			return MODBUS_ERROR_ALLOC;
	#else
		if(frameLength > MODBUS_STATIC_MEM_SLAVE_RESPONSE)
			return MODBUS_ERROR_ALLOC;
	#endif

	ModbusParser *builder = (ModbusParser*)status->response.frame;

	//После всех проверок записываем данные регистров
	#ifdef MODBUS_REGISTER_CALLBACK
		for(i = 0; i < count; i++)
			status->registerCallback(MODBUS_REGQ_W, MODBUS_HOLDING_REGISTER, index + i, modbusMarchEndian(parser->request16.values[i]), status->registerCallbackContext);
	#else
		for(i = 0; i < count; i++)
			status->registers[index + i] = modbusMatchEndian(parser->request16.values[i]);
	#endif

	//Установка базовых значений
	builder->response16.address = status->address;
	builder->response16.function = parser->base.function;
	builder->response16.index = parser->request16.index;
	builder->response16.count = parser->request16.count;

	//Подсчет crc
	builder->response16.crc = crc16(builder->frame, frameLength - 2);

	//Установка длины и пакет готов
	status->response.length = frameLength;
	status->parseError = MODBUS_FERROR_OK;

	return MODBUS_ERROR_OK;
}
#endif
