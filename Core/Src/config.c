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
	uint32_t SectorError;

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
	for (u32 i = 0;
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
	cfg.UartCfg.uart_unpk.databits = 8;
	cfg.UartCfg.uart_unpk.parity = 0;
	cfg.UartCfg.uart_unpk.stopbits = 1;
	cfg.debounce = 50;
	cfg.actMask = 0xffff;

	writeConfig();
}

/*
 *
 * */
void exportConfigToMemMap(void)
{
	u16 *ptr = memMapRegs[0].dataPoint;
	*((u16*)ptr++) = cfg.address;
	*((u32*)ptr) = cfg.uartSpeed;
	ptr += 2;
	*((u16*)ptr++) = (u16)cfg.UartCfg.uart_pack;
	*((u16*)ptr++) = cfg.debounce;
	*((u16*)ptr++) = cfg.actMask;
}

void importCfgFromMemAndSave(void)
{
	u16 *ptr = memMapRegs[0].dataPoint;

	cfg.address = (u8)*((u16*)ptr++);
	cfg.uartSpeed = *((u32*)ptr);
	ptr += 2;
	cfg.UartCfg.uart_pack = *((u16*)ptr++);
	cfg.debounce = *((u16*)ptr++);
	cfg.actMask = *((u16*)ptr++);

	//FIX restore write
	//writeConfig();
}
