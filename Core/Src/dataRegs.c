/*
 * dataRegs.c
 *
 *  Created on: 26 авг. 2019 г.
 *      Author: O.Novikov
 */

#include "main.h"
#include "dataRegs.h"
#include "string.h"


u16 DebounseTime;


//ячейки хранения регистров и дискрет
const u8 device_name[] = "MLC-DI-01";


// дискретные входы

u8 DOUT[3] = {0x71, 0x33, 0x84};
u16 AIN[3] = {123, 576, 10543};

/*входные дискреты*/
u8 DIN[2];// = {0x55, 0xaa};

/*Входные регистры данных*/
u16 DevInfo[256]; // - данные об устройстве
u16 inputRegs[3] = {515, 333, 20115};
/*выходные регистры*/
u16 configRegs[6];

/*выходные дискреты*/
//u8 *DoutMask = (u8*)configRegs + 12;//[2] = {0xff,0xff}; // активность входных дискрет
//---
/* массив хранения временных отрезков для определения дребезга контактов */
u32 __time_check[sizeof(DIN)*8];
//пока что 4 страницы с данными

// страницы с дискретными выходами
dataPage memMapCoils[4];
// страницы с дискретными входами
dataPage memMapDigInput[2];
// страницы с выходными регистрами
dataPage memMapRegs[2];
// страницы с входными регистрами
dataPage memMapInputsRegs[3];

/*
 * Задание свойств страницы
 * -----------------------------
 * page - заполняемая страница
 * numPage - индекс страницы
 * count - количество записей на странице
 * massive - рабочий массив хранения данных
 * */
void allocDataInMemMap(dataPage *page, DataAllocPageNum numPage, u16 count, void* massive)
{
	page->page = numPage;
	page->count = count;
	page->dataPoint = massive;
}


/*
 * Размещение данных по страницам на карте данных
 * */
void allocateDataInMap(void)
{
	//device_ID = *(u32*)(0x1ff0f420); //изъятие уникального ID - адрес для STM32F676
	//Запись информации об устройстве: ID & название устройства
	//u32 device_ID = *(u32*)(0x1ffff7E8);

	// запись ID контроллера
	memcpy((u8*)&DevInfo[0], (u8*)0x1ffff7E8 + 2, 2);
	memcpy((u8*)&DevInfo[1], (u8*)0x1ffff7E8, 2);

	//memcpy((u8*)&DevInfo[2], (u8*)device_name, 10);
	///XXX Возможно это и не стоит такого гемора
	for(int i = 0; i < sizeof(device_name)/2; i++)
	{
		DevInfo[2 + i] = ((u16)device_name[i * 2] << 8) + device_name[i * 2 + 1];
	}

	// Размещение информации об устройстве
	allocDataInMemMap(&memMapInputsRegs[0], 0, 7, (u16*)DevInfo);
	// Размещение аналоговых входов
	allocDataInMemMap(&memMapInputsRegs[1], 1, sizeof(AIN), (u16*)AIN);
	// Размещение цифровых входов
	allocDataInMemMap(&memMapDigInput[AP_DIGIN_DATA_INPUTS], AP_DIGIN_DATA_INPUTS, sizeof(DIN)*8, (u8*)DIN);
	// размещение цифровых выходов
	allocDataInMemMap(&memMapCoils[0], 0, sizeof(DOUT)*8, (u8*)DOUT);
	//test
	allocDataInMemMap(&memMapInputsRegs[2], 1, sizeof(inputRegs),(u16*)inputRegs);

	//размещение регистров конфигурации в карте данных
	allocDataInMemMap(&memMapRegs[0], 0, sizeof(configRegs), (u16*)configRegs);
}

/* Определение */
u8 getStatusBit(u8 index)
{
	return (DoutMask[index >> 3] >>(index % 8)) & 1;
}

void setBitIntoRegPack(u8 *pack, u16 index)
{
	pack[index >> 3] |= 1 << (index % 8);
}

void resetBitIntoRegPack(u8 *pack, u16 index)
{
	pack[index >> 3] &= ~(1 << (index % 8));
}

u8 getBitFromRegPack(u8 *pack, u16 index)
{
	return (pack[index >> 3] >> (index % 8)) & 1;
}
