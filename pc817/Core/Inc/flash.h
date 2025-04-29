#ifndef _FLASH_H
#define _FLASH_H
#include "stm32f1xx_hal.h"   //header device 
#include "stdint.h"
#include "string.h"

#pragma pack(1) 

typedef struct    //cau truc tu dinh nghia (char/int/float)
{
uint8_t Number; 
uint8_t uid[10];
uint8_t Name[20];
	
} student_infor_hust; //kich thuoc 31 byte 

#pragma pack() //chong phan manh bo nho (data alignment) 



void Flash_Unlock (void);
void Flash_Lock (void);

void Flash_Erase (uint32_t address);

void Flash_Write_Int(uint32_t address,int value);
int Flash_Read_Int(uint32_t address);

void Flash_Write_Float(uint32_t address,float f);
float Flash_Read_Float(uint32_t address); 

void Flash_Write_Array(uint32_t address,uint8_t *arr, uint16_t leng ); // mang du lieu
void Flash_Read_Array(uint32_t address, uint8_t *arr, uint16_t leng ); //*arr con tro, tro den noi muon doc du lieu

void Flash_Write_Struct(uint32_t address, student_infor_hust stru  );   //chuoi du lieu
void Flash_Read_Struct(uint32_t address,  student_infor_hust *stru );   //*data truyen tham tri, 2# 

#endif
