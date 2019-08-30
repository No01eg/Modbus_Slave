/*
 * config.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: O.Novikov
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "main.h"

typedef struct __attribute__( (__packed__ )) uart
{
	u8 databits : 4; /* кол-во бит данных */
	u8 stopbits : 2; /* кол-во стоп-битов */
	u8 parity 	: 2; /* четность (0 - нет, 1 - чет, 2 - нечет) */
}Uart;

typedef union
{
	u8 uart_pack;
	Uart uart_unpk;
}packUart;
//Структура конфигурации модуля
typedef struct __attribute__( (__packed__ )) config
{
	u32 uartSpeed; /* скорость передачи */
	packUart UartCfg;
	u8 address;   /* локальный адрес модбас */
	u16 debounce; /*время контроля дребезга контактов */
	u16 actMask;  /* маска на входные каналы */
}Config;

extern Config cfg;

/* Чтение конфигурации устройства из Flash */
void readConfig(void/*Config *cfg*/);
/* Запись конфигурации устройсва во Flash */
void writeConfig(void/*const Config *cfg*/);
/* Предустановка конфигурации по умолчанию */
void preloadDefaultConfig(void/*Config *cfg*/);

void exportConfigToMemMap(void);

#endif /* INC_CONFIG_H_ */
