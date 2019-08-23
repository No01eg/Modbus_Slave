/*
 * mb_parser.h
 *
 *  Created on: 15 авг. 2019 г.
 *      Author: O.Novikov
 */

#ifndef MODBUS_MB_PARSER_H_
#define MODBUS_MB_PARSER_H_

#include "main.h"

typedef union modbusParser
{
	u8 frame[256];

	struct __attribute__( (__packed__ ))
	{
		u8 address;
		u8 function;
	}base; //общиие фреймы на все пакета

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u8  code;
		u16 crc;
	}exception;

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}request0102;//пакет запроса массива бит

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u8  length;
		u8  values[250];
		u16 crc;
	}response0102; //пакет с ответом с массивом бит

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}request0304; // пакет запроса массива word'ов

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u8  length;
		u16 values[125];
		u16 crc;
	}response0304; // пакет ответа с массивом word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 value;
		u16 crc;
	}request0506; // запрос на запись 1 бита либо 1 word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 value;
		u16 crc;
	}response0506; // ответ на запрос записи 1 бит или 1 word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u8  length;
		u8  values[246];
		u16 crc;
	}request15; //запись массива бит

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}response15; //ответ на запись массива бит

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u8  length;
		u16 values[123];
		u16 crc;
	}request16; // запись массива word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}response16; // ответ на запись массива word
} ModbusParser;


#endif /* MODBUS_MB_PARSER_H_ */
