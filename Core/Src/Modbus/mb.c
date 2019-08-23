/*
 * mb.c
 *
 *  Created on: 8 авг. 2019 г.
 *      Author: O.Novikov
 */

#include "ModBus/mb.h"

//ячейки хранения регистров и дискрет
u8 DIN[4] = {0x55, 0xaa, 0x03, 0x15};
u8 DOUT[3];
u16 AIN[3] = {123, 576, 10543};

ModbusSlave mbSlave;

//TODO подчистить
// Тип текущей связи для модбаса (Ethernet || RTU(RS485)
//eMBMode MBConnect;
// Локальный адресс устройства
//u8 eMBLocalAddr;

//ModbusSlave mbSlave;

/* определение значение n-го бита в массиве битов
 * mask - массив хранения битов
 * maskLenght -
 * bit - номер бита */
u8 modbusMaskRead(const u8 *mask, u16 maskLenght, u16 bit)
{
	if (mask == NULL) return 255;
	if((bit >> 3) >= maskLenght) return 255;
	return (mask[bit >> 3] & (1<<(bit % 8))) >> (bit % 8);
}

/* Запись значения бита распоболенного в n-ой позиции в массиве битов
 *
 * */
u8 modbusMaskWrite(u8 *mask, u16 maskLenght, u16 bit, u8 value)
{
	if(mask == NULL) return 255;
	if(bit >> 3 >= maskLenght) return 255;
	if(value)
		mask[bit >> 3] |= (1 << (bit % 8));
	else
		mask[bit >> 3] &= ~(1 << (bit % 8));
	return 0;
}

// Рассчет контрольной суммы crc пакета
// msg - массив
// len - длина пакета
// возвращает значение контрольной суммы
u16 crc16(const u8* data, u8 lenght)
{
	u16 crc = 0xffff, poly = 0xa001;
	u16 i = 0;
	u8 j;
	if(data == NULL)
		return 0;

	for(i = 0; i < lenght; i++)
	{
		crc ^= data[i];
		for(j = 0; j < 8; j++)
		{
			if(crc & 0x01)
			{
				crc >>= 1;
				crc ^= poly;
			}
			else
				crc >>= 1;
		}
	}

	//CRC всегда в формате Little Endian
	#ifdef MODBUS_BIG_ENDIAN
		return (modbusSwapEndian(crc));
	#else
		return (crc);
	#endif
}





void MB_SlaveInit()
{
	mbSlave.coilCount = 32;
	mbSlave.coils = DIN;

	mbSlave.discreteInputCount = 24;
	mbSlave.discreteInputs = DOUT;

	mbSlave.registerCount = 3;
	mbSlave.registers = AIN;

	mbSlave.address = 3;

	modbusSlaveInit(&mbSlave);
}

