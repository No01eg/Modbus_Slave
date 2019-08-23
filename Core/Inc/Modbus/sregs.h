/*
 * sregs.h
 *
 *  Created on: 20 рту. 2019 у.
 *      Author: O.Novikov
 */

#ifndef MODBUS_SREGS_H_
#define MODBUS_SREGS_H_

#include "main.h"
#include "mb.h"
#include "mb_slave.h"

#if defined(MODBUS_F03S) || defined(MODBUS_F04S)
	extern ModbusError modbusParseRequest0304(ModbusSlave *status, ModbusParser *parser);
#endif

#ifdef MODBUS_F06S
	extern ModbusError modbusParseRequest06(ModbusSlave *status, ModbusParser *parser);
#endif

#ifdef MODBUS_F16S
	extern ModbusError modbusParseRequest16(ModbusSlave *status, ModbusParser *parser);
#endif

#ifdef MODBUS_F22S
	extern ModbusError modbusParseRequest22(ModbusSlave *status, ModbusParser *parser);
#endif

#endif /* MODBUS_SREGS_H_ */
