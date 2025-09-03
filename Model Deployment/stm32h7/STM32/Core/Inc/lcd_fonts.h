#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>



typedef struct _pFont
{    
	const uint8_t 		*pTable;  	
	uint16_t 			Width; 		 	
	uint16_t 			Height; 			
	uint16_t 			Sizes;	 
	uint16_t				Table_Rows;		
} pFONT;



extern	pFONT	CH_Font12 ;		
extern	pFONT	CH_Font16 ;    
extern	pFONT	CH_Font20 ;  
extern	pFONT	CH_Font24 ;   
extern	pFONT	CH_Font32 ;   



extern pFONT ASCII_Font32;		
extern pFONT ASCII_Font24;	
extern pFONT ASCII_Font20; 	
extern pFONT ASCII_Font16; 	
extern pFONT ASCII_Font12; 	

#endif 
 
