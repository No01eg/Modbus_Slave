/*
 * config.h
 *
 *  Created on: 28 ���. 2019 �.
 *      Author: O.Novikov
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "main.h"

//��������� ������������ ������
typedef struct __attribute__( (__packed__ )) config
{
	u32 uartSpeed; /* �������� �������� */
	struct __attribute__( (__packed__ )) uart
	{
		u8 databits : 4; /* ���-�� ��� ������ */
		u8 stopbits : 2; /* ���-�� ����-����� */
		u8 parity 	: 2; /* �������� (0 - ���, 1 - ���, 2 - �����) */
	}Uart;
	u8 address;   /* ��������� ����� ������ */
	u16 debounce; /*����� �������� �������� ��������� */
	u16 actMask;  /* ����� �� ������� ������ */
}Config;

extern Config cfg;

/* ������ ������������ ���������� �� Flash */
void readConfig(void/*Config *cfg*/);
/* ������ ������������ ��������� �� Flash */
void writeConfig(void/*const Config *cfg*/);
/* ������������� ������������ �� ��������� */
void preloadDefaultConfig(void/*Config *cfg*/);

#endif /* INC_CONFIG_H_ */
