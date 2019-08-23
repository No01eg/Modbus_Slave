/*
 * scoils.h
 *
 *  Created on: 19 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef MODBUS_SCOILS_H_
#define MODBUS_SCOILS_H_

#include "main.h"
#include "mb.h"
#include "mb_slave.h"

#if defined(MODBUS_F01S) || defined(MODBUS_F02S)
	//#define
	/*������� ������ ��������� ������� ��������� � ���������� ������*/
	extern ModbusError modbusParseRequest0102(ModbusSlave *status, ModbusParser *parser);
#endif

#if defined(MODBUS_F05S)
	/*������� ������ � ���������� �������*/
	extern ModbusError modbusParseRequest05(ModbusSlave *status, ModbusParser *parser);
#endif

#if defined(MODBUS_F15S)
	/*������� ������ ��������� ���������� ���������*/
	extern ModbusError modbusParseRequest15(ModbusSlave *status, ModbusParser *parser);
#endif

#endif /* MODBUS_SCOILS_H_ */
