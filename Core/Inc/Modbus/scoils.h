/*
 * scoils.h
 *
 *  Created on: 19 авг. 2019 г.
 *      Author: O.Novikov
 */

#ifndef MODBUS_SCOILS_H_
#define MODBUS_SCOILS_H_

#include "main.h"
#include "mb.h"
#include "mb_slave.h"

#if defined(MODBUS_F01S) || defined(MODBUS_F02S)
	//#define
	/*функция чтения множества битовых регистров и дискретных входов*/
	extern ModbusError modbusParseRequest0102(ModbusSlave *status, ModbusParser *parser);
#endif

#if defined(MODBUS_F05S)
	/*функция записи в дискретный регистр*/
	extern ModbusError modbusParseRequest05(ModbusSlave *status, ModbusParser *parser);
#endif

#if defined(MODBUS_F15S)
	/*функция записи множества дискретных регистров*/
	extern ModbusError modbusParseRequest15(ModbusSlave *status, ModbusParser *parser);
#endif

#endif /* MODBUS_SCOILS_H_ */
