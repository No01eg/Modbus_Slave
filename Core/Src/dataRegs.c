/*
 * dataRegs.c
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: O.Novikov
 */

#include "main.h"
#include "dataRegs.h"
#include "string.h"


u16 DebounseTime;


//������ �������� ��������� � �������
const u8 device_name[] = "MLC-DI-01";


// ���������� �����

u8 DOUT[3] = {0x71, 0x33, 0x84};
u16 AIN[3] = {123, 576, 10543};

/*������� ��������*/
u8 DIN[2];// = {0x55, 0xaa};

/*������� �������� ������*/
u16 DevInfo[256]; // - ������ �� ����������
u16 inputRegs[3] = {515, 333, 20115};
/*�������� ��������*/
u16 configRegs[6];

/*�������� ��������*/
//u8 *DoutMask = (u8*)configRegs + 12;//[2] = {0xff,0xff}; // ���������� ������� �������
//---
/* ������ �������� ��������� �������� ��� ����������� �������� ��������� */
u32 __time_check[sizeof(DIN)*8];
//���� ��� 4 �������� � �������

// �������� � ����������� ��������
dataPage memMapCoils[4];
// �������� � ����������� �������
dataPage memMapDigInput[2];
// �������� � ��������� ����������
dataPage memMapRegs[2];
// �������� � �������� ����������
dataPage memMapInputsRegs[3];

/*
 * ������� ������� ��������
 * -----------------------------
 * page - ����������� ��������
 * numPage - ������ ��������
 * count - ���������� ������� �� ��������
 * massive - ������� ������ �������� ������
 * */
void allocDataInMemMap(dataPage *page, DataAllocPageNum numPage, u16 count, void* massive)
{
	page->page = numPage;
	page->count = count;
	page->dataPoint = massive;
}


/*
 * ���������� ������ �� ��������� �� ����� ������
 * */
void allocateDataInMap(void)
{
	//device_ID = *(u32*)(0x1ff0f420); //������� ����������� ID - ����� ��� STM32F676
	//������ ���������� �� ����������: ID & �������� ����������
	//u32 device_ID = *(u32*)(0x1ffff7E8);

	// ������ ID �����������
	memcpy((u8*)&DevInfo[0], (u8*)0x1ffff7E8 + 2, 2);
	memcpy((u8*)&DevInfo[1], (u8*)0x1ffff7E8, 2);

	//memcpy((u8*)&DevInfo[2], (u8*)device_name, 10);
	///XXX �������� ��� � �� ����� ������ ������
	for(int i = 0; i < sizeof(device_name)/2; i++)
	{
		DevInfo[2 + i] = ((u16)device_name[i * 2] << 8) + device_name[i * 2 + 1];
	}

	// ���������� ���������� �� ����������
	allocDataInMemMap(&memMapInputsRegs[0], 0, 7, (u16*)DevInfo);
	// ���������� ���������� ������
	allocDataInMemMap(&memMapInputsRegs[1], 1, sizeof(AIN), (u16*)AIN);
	// ���������� �������� ������
	allocDataInMemMap(&memMapDigInput[AP_DIGIN_DATA_INPUTS], AP_DIGIN_DATA_INPUTS, sizeof(DIN)*8, (u8*)DIN);
	// ���������� �������� �������
	allocDataInMemMap(&memMapCoils[0], 0, sizeof(DOUT)*8, (u8*)DOUT);
	//test
	allocDataInMemMap(&memMapInputsRegs[2], 1, sizeof(inputRegs),(u16*)inputRegs);

	//���������� ��������� ������������ � ����� ������
	allocDataInMemMap(&memMapRegs[0], 0, sizeof(configRegs), (u16*)configRegs);
}

/* ����������� */
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
