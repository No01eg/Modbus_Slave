/*
 * dataRegs.h
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef INC_DATAREGS_H_
#define INC_DATAREGS_H_

extern u16 DebounseTime;

extern u8 DOUT[];

extern u32 __time_check[];
extern u8* DoutMask;//[];

#include "mapAlloc.h"

typedef struct
{
	u8 page;
	u16 count;
	void *dataPoint;
}dataPage;

// �������� � ����������� ��������
extern dataPage memMapCoils[4];
// �������� � ����������� �������
extern dataPage memMapDigInput[2];
// �������� � ��������� ����������
extern dataPage memMapRegs[2];
// �������� � �������� ����������
extern dataPage memMapInputsRegs[3];

// ������������� ����� ���������� ������ ��������� � ���������
void allocateDataInMap(void);

u8 getStatusBit(u8 index);

void setBitIntoRegPack(u8 *pack, u16 index);

void resetBitIntoRegPack(u8 *pack, u16 index);

u8 getBitFromRegPack(u8 *pack, u16 index);

#endif /* INC_DATAREGS_H_ */
