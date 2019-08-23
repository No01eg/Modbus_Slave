/*
 * mb_slave.h
 *
 *  Created on: 13 авг. 2019 г.
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
			u8 function; // код функции
			//указатель на пользовательскую функцию обработки пакета
			ModbusError (*handler)(struct modbusSlave *status, ModbusParser *parser);
		}ModbusSlaveUserFunction;
	#endif


/*
 * Подчиненное модбас устройство - статус и конфиг
 * */
typedef struct modbusSlave
{
	u8 address; //локальный адресс устройства


	// массив регистров
	#ifndef MODBUS_REGISTER_CALLBACK
		u16 *registers; 		// указатель на регистр данных
		u16 *inputRegisters; 	// указатель на входной регистр данных
		u8  *registerMask;		// защитная маска для битовых массивов (каждый бит соответствует регистру)
		u16 registerMaskLegth;	// защитная маска для регистра (1 байт содержит 8 регистров bool)
	#endif
	u16 registerCount; 			// количество регистров подчиненного устройства
	u16 inputRegisterCount; 	// количество входных регистров подчиненного устройства

	#ifndef MODBUS_COIL_CALLBACK
		u8  *coils;				// указатель на дискретные выходы/входы
		u8  *discreteInputs;	// указатель на дискретные входы
		u8  *coilMask;			// маска для записи  дискреты IO
		u16 coilMaskLength;	// длина в байтах
	#endif
	u16 coilCount;				// количество дисктретов IO
	u16 discreteInputCount;	// кол-во дискретных входов

	// код последней ошибки
	ModbusExceptionCode lastException;

	// более специфичный код ошибки
	ModbusFrameError parseError;

	//структура содержащая пакет ответа + длина
	struct
	{
		#ifdef MODBUS_STATIC_MEM_SLAVE_RESPONSE
			u8 frame[MODBUS_STATIC_MEM_SLAVE_RESPONSE];
		#else
			u8 *frame;
		#endif
		u8 length;
	}response;

	// структура содержащая пакет запроса + длина пакета
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

//прототипы функций

/* Создание пакета об ошибке
 * Данный пакет формируется если у нас не широковещательный адресс
 * */
extern ModbusError modbusBuildException(ModbusSlave *status, u8 function, ModbusExceptionCode code);


/*
 * Обработчик разбора пакета запроса
 * */
extern ModbusError modbusParseRequest(ModbusSlave *status);

/*
 * инициализатор структуры подчиненного модбас
 * */
extern ModbusError modbusSlaveInit(ModbusSlave *status);

/*
 * деинициализация структуры подчиненного модбас
 * */
extern ModbusError modbusSlaveEnd(ModbusSlave *status);

/*
 * разборщик ошибок
 * создает исключение и записывает конктретную ошибку в структуру
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

///XXX Структуры ниже могут не понадобиться вообще

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



/*структура конфига модбас */
typedef struct
{
	u8 MBActivity; 				/* 1 - is active; 0 - desactive */
	eMBMode MBModeType;			/* 0 - RTU; 1 - TCP */
	u8 MBLocalAddr;				/* */
	u8 reserve;					/* для выравнивания в конфиге*/
}eMBConfig;

#endif /* MODBUS_MB_SLAVE_H_ */
