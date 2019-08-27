/*
 * dataRegs.c
 *
 *  Created on: 26 авг. 2019 г.
 *      Author: O.Novikov
 */

#include "main.h"
#include "dataRegs.h"
#include "string.h"

//ячейки хранения регистров и дискрет
const u8 device_name[] = "MLC-DI-01";
//u32 device_ID = 0;
// дискретные входы
u8 DIN[4] = {0x55, 0xaa, 0x03, 0x15};
u8 DOUT[3] = {0x71, 0x33, 0x84};
u16 AIN[3] = {123, 576, 10543};


/*Входные регистры данных*/
u16 DevInfo[256];
u16 inputRegs[3] = {515, 333, 20115};

//пока что 4 страницы с данными

// страница с дискретными выходами
dataPage memMapCoils[4];
// страница с дискретными входами
dataPage memMapDigInput[2];
// страницы с выходными регистрами
dataPage memMapRegs[2];
// страницы с входными регистрами
dataPage memMapInputsRegs[3];


void allocDataInMemMap(dataPage *page, DataAllocPageNum numPage, u16 count, void* massive)
{
	page->page = numPage;
	page->count = count;
	page->dataPoint = massive;
}

void allocateDataInMap(void)
{
	//device_ID = *(u32*)(0x1ff0f420); //изъятие уникального ID - адрес для STM32F676
	//Запись информации об устройстве: ID & название устройства
	u32 device_ID = *(u32*)(0x1ffff7E8);

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

	allocDataInMemMap(&memMapCoils[0], 0, sizeof(DOUT)*8, (u8*)DOUT);

	allocDataInMemMap(&memMapInputsRegs[2], 1, sizeof(inputRegs),(u16*)inputRegs);
	/*memMapInputsRegs[0].page = 0;
	memMapInputsRegs[0].count = 7;
	memMapInputsRegs[0].dataPoint = (u16*)DevInfo;*/


	//размещение цифровых входов
	/*memMapDigInput[0].page = 0;
	memMapDigInput[0].count = sizeof(DIN) * 8;// = {0, sizeof(DIN)*8, (u8*)DIN};
	memMapDigInput[0].dataPoint = (u8*)DIN;*/

	//memSrc[1].
}
