/*
 * dataRegs.h
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef INC_DATAREGS_H_
#define INC_DATAREGS_H_

/*extern u8 DIN[];



extern u16 AIN[];*/
extern u8 DOUT[];

typedef enum dataAllocPageNum
{
	AP_COILS_DATA_DOUT = 0,
	AP_DIGIN_DATA_INPUTS = 0,
}DataAllocPageNum;

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

#endif /* INC_DATAREGS_H_ */
