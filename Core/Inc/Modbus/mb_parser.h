/*
 * mb_parser.h
 *
 *  Created on: 15 ���. 2019 �.
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
	}base; //������ ������ �� ��� ������

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
	}request0102;//����� ������� ������� ���

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u8  length;
		u8  values[250];
		u16 crc;
	}response0102; //����� � ������� � �������� ���

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}request0304; // ����� ������� ������� word'��

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u8  length;
		u16 values[125];
		u16 crc;
	}response0304; // ����� ������ � �������� word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 value;
		u16 crc;
	}request0506; // ������ �� ������ 1 ���� ���� 1 word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 value;
		u16 crc;
	}response0506; // ����� �� ������ ������ 1 ��� ��� 1 word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u8  length;
		u8  values[246];
		u16 crc;
	}request15; //������ ������� ���

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}response15; //����� �� ������ ������� ���

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u8  length;
		u16 values[123];
		u16 crc;
	}request16; // ������ ������� word

	struct __attribute__((__packed__))
	{
		u8  address;
		u8  function;
		u16 index;
		u16 count;
		u16 crc;
	}response16; // ����� �� ������ ������� word
} ModbusParser;


#endif /* MODBUS_MB_PARSER_H_ */
