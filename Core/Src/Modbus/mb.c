/*
 * mb.c
 *
 *  Created on: 8 авг. 2019 г.
 *      Author: O.Novikov
 */
#include "main.h"
#include "ModBus/mb.h"

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

	mbSlave.coilCount = memMapCoils[0].count;
	mbSlave.coils = (u8*)memMapCoils[0].dataPoint;

	// По умолчанию у нас цифровые входы
	mbSlave.discreteInputCount = memMapDigInput[AP_DIGIN_DATA_INPUTS].count;
	mbSlave.discreteInputs = (u8*)memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint;

	//По умолчанию сделаем аналоговые входы
	mbSlave.registerCount = memMapInputsRegs[1].count;
	mbSlave.registers = (u16*)memMapInputsRegs[1].dataPoint;

	mbSlave.address = cfg.address;

	modbusSlaveInit(&mbSlave);
}

