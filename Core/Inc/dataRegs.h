/*
 * dataRegs.h
 *
 *  Created on: 26 авг. 2019 г.
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

// страница с дискретными выходами
extern dataPage memMapCoils[4];
// страница с дискретными входами
extern dataPage memMapDigInput[2];
// страницы с выходными регистрами
extern dataPage memMapRegs[2];
// страницы с входными регистрами
extern dataPage memMapInputsRegs[3];

// инициализация карты размещения данных дискретов и регистров
void allocateDataInMap(void);

#endif /* INC_DATAREGS_H_ */
