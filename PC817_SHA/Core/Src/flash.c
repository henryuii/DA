#include "flash.h"
#include "stm32f1xx_hal.h"   //header device 
#include "stdint.h"
#include "string.h"

void Flash_Lock()
{
	HAL_FLASH_Lock();
}

void Flash_Unlock()
{
	HAL_FLASH_Unlock();
} 
                
void Flash_Erase(uint32_t address) //address: dia chi page muon xoa 
{
	HAL_FLASH_Unlock(); //first step 	
	
	FLASH_EraseInitTypeDef  EraseInitStruct; //param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure
	 
	EraseInitStruct.Banks = 1;   //stm32f103c8t6 has only 1 bank 
	EraseInitStruct.NbPages = 1; // Number page by 1 delete 
	EraseInitStruct.PageAddress = address; //Address page where you wwant to delete 
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // Type of erase 
	
	
	uint32_t pageerr; //param[out]  PageError pointer to variable that contains the configuration information on faulty page in case of error
	
	HAL_FLASHEx_Erase(&EraseInitStruct, &pageerr);
	HAL_FLASH_Lock();   //last step 
}

void Flash_Write_Int(uint32_t address, int value)
{ 
	HAL_FLASH_Unlock(); //first step 
	//int co kich thuoc 4 byte trong he vi dieu khien core size 32 bit 
	// choose HALFWORD may be not correctly, need check by other example 
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, value);  //HALFWORD 16 bit/ 2 byte length 
	HAL_FLASH_Lock();   //last step 
}


int Flash_Read_Int(uint32_t address)
{
	return *(__IO uint16_t *)(address); //*(...): Toan tu dereference: lay gia tri tai dia chi con tro tro den
	//__IO: Input/Output volatile, Gia tri tai dia chi nay co the thay doi moi luc, khong toi uu hoa bien dich
	
}

void Flash_Write_Array(uint32_t address, uint8_t *arr, uint16_t leng) //*arr cho phep truyen bat ky mang du lieu nao, min size 1 byte, khong bi loi
	//arr cho phep truyen bat ky mang nao kich thuoc toi thieu 1 byte 
// ho tro tot khi leng le so le  //phu hop giao thuc truyen thong UART/SPI.. ma khong phai ep kieu 	
{
	HAL_FLASH_Unlock();
	uint16_t *pt = (uint16_t*)arr;  
	for(uint16_t i=0; i< (leng+1)/2 ;i++) //ghi 2 byte mot lan, le byte thi khong bi thieu 
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address + 2*i, *pt);
		pt++;
	}
	HAL_FLASH_Lock();
}

void Flash_Read_Array(uint32_t address, uint8_t *arr, uint16_t leng)
{
	uint16_t * pt = (uint16_t*)arr; 
	for(uint16_t i=0; i< (leng+1)/2;i++)
	{
		*pt = *(__IO uint16_t *)(address+2*i);
		pt++;
	}
}

void Flash_Write_Struct(uint32_t address, student_infor_hust stru)
{
	Flash_Write_Array(address,(uint8_t*) &stru, sizeof(stru));
}

void Flash_Read_Struct(uint32_t address, student_infor_hust *stru)
{
	Flash_Read_Array(address, (uint8_t*)stru, sizeof(student_infor_hust));
}
