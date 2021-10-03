/*************************************************************************
*
* File name : main.c
* Description : Contains main function
* Author: Gaurang Krishnakant Rane/ Taher Ujjainwala
* Tools : mbed, Keil
*
**************************************************************************/
#include "mbed.h"
 Timer t;

void squareroot(int num);/*function declaration of squareroot function*/

/*************************************************************************************************************
*
* Name : int main()
* Description : function contains fuction call to squareroot function
*
*************************************************************************************************************/
int main()
{
		t.start();
	  squareroot(121);/*function call to squareroot function*/
		float time_val = t.read();
		t.stop();
		__asm("NOP");
   }


	 
__asm void squareroot(int num)
{
					MOVS R1,#0 /*Initializing done variable*/
					MOVS R2,#0 /*Initializing lower limit A*/
					MOVS R6,#255 /*Initializing higher limit B*/
					MOVS R4,#1 /*Initializing sqaureroot C*/
					MOVS R3,#40 /*Initializing maximum number of iteration*/
LOOP
					MOVS R5,R4 /*taking backup of C*/
					ADDS R4,R2,R6
					LSRS R4,#1 /*calcultating midpoint*/
					
					MOVS R7,R4
					MULS R7,R4,R7 /*sqaure of midpoint*/
BEGINIF			
					CMP R0,R7  /*Comparing square of midpoint with inut argument */
					BNE IFELSE
					MOVS R1,#1
					B LAST
IFELSE	
					CMP R0,R7
					BLE ELSE	
					MOVS R2,R4
					B LAST	
ELSE
					MOVS R6,R4
					B LAST
LAST			SUBS R3,R3,#1 /*Comparing conditions for exiting loop*/
					CMP R3,#0
					BEQ RETURN
					CMP R3,#0
					BNE LOOP
					CMP R1,#0
				  BNE RETURN
					CMP R1,#0
					BEQ LOOP
					
					CMP R4,R5
					BEQ RETURN
					CMP R4,R5
					BNE LOOP
					
RETURN					
					MOVS R0,R4  /*move squareroot to R0*/
					BX LR
						
}

