/*
 * config.c
 *
 *  Created on: 28 ���. 2019 �.
 *      Author: O.Novikov
 */
#include "config.h"

//����� �������� � ������ ��� �������� ������� (64 �������� � ������ - ��������� ��� STM32F103C8T6)
#define CONFIG_PAGE_OFFSET 	0x10000
// ����� �������� ���������� ��� ������������
#define CONFIG_SECTOR_IND	63
// ������ ����� � ��������, � ������
#define WORD_SIZE			4

#define CONFIG_PAGE	(0x08000000|CONFIG_PAGE_OFFSET)

#define CONFIG_END_PAGE (0x08000000|0x103FF)

Config cfg;

// ������� �������
void __FlashErase(u32 Sector, u32 count)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	u32 SectorError;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;

	EraseInitStruct.PageAddress = Sector;
	EraseInitStruct.NbPages = count;

	HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
}

void __FlashWrite(uint32_t Adrr, uint32_t data) {
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Adrr, data);
}

void __FlashUnlock() {
	HAL_FLASH_Unlock();
}

void __FlashLock() {
	HAL_FLASH_Lock();
}


/* ������ ������������ ���������� �� Flash */
void readConfig(void/*Config *cfg*/)
{
	cfg = (*(__IO Config*)CONFIG_PAGE_OFFSET);
}

/* ������ ������������ ��������� �� Flash */
void writeConfig(void/*const Config *cfg*/)
{
	__FlashUnlock();
	__FlashErase(CONFIG_PAGE, 1);

	//������ ��������� ������������ �������� ����� ����� �� 4 ����� �� ����� ��������� ��� �������
	for (uint32_t i = 0;
		(i < sizeof(cfg) / WORD_SIZE)
			& ((CONFIG_PAGE + i * WORD_SIZE) < CONFIG_END_PAGE);
			i++)
	{
		__FlashWrite(CONFIG_PAGE + i * WORD_SIZE,	*((u32*) &cfg + i));
	}
	__FlashLock();
}


/* ������������� ������������ �� ��������� */
void preloadDefaultConfig(void/*Config *cfg*/)
{
	cfg.address = 3;
	cfg.uartSpeed = 115200;
	cfg.Uart.databits = 8;
	cfg.Uart.parity = 0;
	cfg.Uart.stopbits = 0;
	cfg.actMask = 0xffff;
	cfg.debounce = 50;

	writeConfig();
}
