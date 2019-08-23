/*
 * mb.h
 *
 *  Created on: 8 авг. 2019 г.
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
//Ошибки времени исполнения
typedef enum modbusError
{
	MODBUS_ERROR_OK 	   = 0, // ошибок нет
	MODBUS_ERROR_EXCEPTION = 1, // индикация, что, что-то при передаче не так
	MODBUS_ERROR_ALLOC 	   = 2, // проблема с размещением объектов в памяти
	MODBUS_ERROR_OTHER     = 3, // прочие проблемы с модбас
	MODBUS_ERROR_NULLPRT   = 4,	// какой-то параметр не обозначен
	MODBUS_ERROR_PARSE     = 5, // проблемы с разбором пакета
	MODBUS_ERROR_BUILD     = 6, // проблема с построением пакета
	MODBUS_OK = MODBUS_ERROR_OK // повторно ошибок нет
}ModbusError;

//Ошибки фреймов в пакете
typedef enum modbusFrameError
{
	MODBUS_FERROR_OK = MODBUS_OK,	// ошибок нет
	MODBUS_FERROR_CRC,				// неверная crc
	MODBUS_FERROR_LENGHT,			// неверная длина пакета
	MODBUS_FERROR_COUNT,			// неверно указано количество элементов
	MODBUS_FERROR_VALUE,			// неверно указано значение (при записи 1 бита)
	MODBUS_FERROR_RANGE,			// неверный диапазон регистров
	MODBUS_FERROR_NOSRC,			// Для этого типа данных не предусмотрено ни функции обратного вызова, ни массива значений
	MODBUS_FERROR_NOREAD,			// нет доступа на чтение хотя бы с одним из запрашиваемых элементов
	MODBUS_FERROR_NOWRITE,			// нет доступа на запись хотя бы к одному из запрашиваемых элементов
	MODBUS_FERROR_NOFUN,			// функция не поддерживается
	MODBUS_FERROR_BADFUN,			// неправильный код функции
	MODBUS_FERROR_NULLFUN,			// функция переопределена пользователем как NULL
	MODBUS_FERROR_MISM_FUN,			// несоответствие запроса-ответа функции
	MODBUS_FERROR_MISM_ADDR,		// Несоответствие запроса-ответа ведомого адреса
	MODBUS_FERROR_MISM_INDEX,		// Несоответствие запроса-ответа значения индекса
	MODBUS_FERROR_MISM_COUNT,		// Несоответствие запрос-ответ количества значений
	MODBUS_FERROR_MISM_VALUE,		// Несоответствие запроса-ответа значения данных
	MODBUS_FERROR_MISM_MASK,		// Несоответствие запроса-ответа значения маски
	MODBUS_FERROR_BROADCAST			// Получен ответ на широковещательное сообщение
}ModbusFrameError;

//Коды исключений модбас, описанные в стандарте
typedef enum modbusExceptionCode
{
	MODBUS_EXCEP_ILLEGAL_FUNCTION = 1, 	// неверный код функции
	MODBUS_EXCEP_ILLEGAL_ADDRESS  = 2,  // неверный адресс элемента
	MODBUS_EXCEP_ILLEGAL_VALUE	  = 3,	// Неверное значение элемента
	MODBUS_EXCEP_SLAVE_FAILURE	  = 4,  // подчиненный не может обработать запрос
	MODBUS_EXCEP_ACK			  = 5,  // Acknowledge
	MODBUS_EXCEP_NACK			  = 7,	// Negative acknowledge
}ModbusExceptionCode;


//кодировка типов данных
typedef enum modbusDataType
{
	MODBUS_HOLDING_REGISTER = 1,
	MODBUS_INPUT_REGISTER 	= 2,
	MODBUS_COIL				= 4,
	MODBUS_DISCRETE_INPUT 	= 8,
} ModbusDataType;

/* определение необходимого числа байт для считывания n бит по модбас скопом (до 16)*/
static inline u16 modbusBitToBytes(u16 n)
{
	return n != 0 ? (1 + ((n - 1) >> 3)) : 0;
}

/* меняет байты местами в word*/
static inline u16 modbusSwapEndian(u16 data)
{
	return (data << 8) | (data >> 8);
}

/*формат передачи word -> big Endian || little endian*/
#ifdef MODBUS_BIG_ENDIAN
	static inline u16 modbusMatchEndian(u16 data){return data;}
#else
	static inline u16 modbusMatchEndian(u16 data){return modbusSwapEndian(data);}
#endif

/* Чтение n-го бита из массива */
extern u8 modbusMaskRead(const u8 *mask, u16 maskLenght, u16 bit);

/* Запись n-го бита в массив */
extern u8 modbusMaskWrite(u8 *mask, u16 maskLenght, u16 bit, u8 value);

/* Рассчет CRC цепочки */
extern u16 crc16(const u8* data, u8 lenght);

#include "ModBus/mb_slave.h"

extern ModbusSlave mbSlave;

extern void MB_SlaveInit(void);

#endif /* MODBUS_MB_H_ */
