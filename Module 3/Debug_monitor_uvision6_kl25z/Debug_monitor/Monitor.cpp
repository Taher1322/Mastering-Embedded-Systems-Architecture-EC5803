/**----------------------------------------------------------------------------
             \file Monitor.cpp
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      Monitor.cpp                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Student's name 
-- 
-- Version: 2.0
-- Date of current revision:  2016-09-29   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
   Functional Description: See below 
--
--      Copyright (c) 2015 Tim Scherr All rights reserved.
--
*/              

#include <stdio.h>
#include "shared.h"
#include "tsi_sensor.h"
#include "MMA8451Q.h"

#if defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  #define ELEC0 9
  #define ELEC1 10
#elif defined (TARGET_KL05Z)
  #define ELEC0 9
  #define ELEC1 8
#else
  #error TARGET NOT DEFINED
#endif


#if   defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  PinName const SDA = PTE25;
  PinName const SCL = PTE24;
#elif defined (TARGET_KL05Z)
  PinName const SDA = PTB4;
  PinName const SCL = PTB3;
#elif defined (TARGET_K20D50M)
  PinName const SDA = PTB1;
  PinName const SCL = PTB0;
#else
  #error TARGET NOT DEFINED
#endif


TSIAnalogSlider tsi(ELEC0, ELEC1, 40);
#define MMA8451_I2C_ADDRESS (0x1d<<1)
MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);

unsigned int Reg0,Reg1,Reg2,Reg3,Reg4,Reg5,Reg6,Reg7,Reg8,Reg9,Reg10,Reg11,Reg12,Reg13,Reg14,Reg15;

/*******************************************************************************
*   \fn  REGISTER VALUE ACESS FUNCTION
*******************************************************************************/
__asm int register_0(void)
    {
    MOV r0,r0
    BX lr
    } 
__asm int register_1(void)
    {
    MOV r0,r1
    BX lr
    } 
__asm int register_2(void)
    {
    MOV r0,r2
    BX lr
    }
__asm int register_3(void)
    {
    MOV r0,r3
    BX lr
    }   
__asm int register_4(void)
    {
    MOV r0,r4
    BX lr
    } 
__asm int register_5(void)
    {
    MOV r0,r5
    BX lr
    } 
__asm int register_6(void)
    {
    MOV r0,r6
    BX lr
    } 
__asm int register_7(void)
    {
    MOV r0,r7
    BX lr
    } 
__asm int register_8(void)
    {
    MOV r0,r8
    BX lr
    } 
__asm int register_9(void)
    {
    MOV r0,r9
    BX lr
    } 
__asm int register_10(void)
    {
    MOV r0,r10
    BX lr
    }   
__asm int register_11(void)
    {
    MOV r0,r11
    BX lr
    }
__asm int register_12(void)
    {
    MOV r0,r12
    BX lr
    }   
__asm int register_13(void)
    {
    MOV r0,r13
    BX lr
    }   
__asm int register_14(void)
    {
    MOV r0,r14
    BX lr
    }
__asm int register_15(void)
    {
    MOV r0,r15
    BX lr
    }       

/*******************************************************************************
* Set Display Mode Function
* Function determines the correct display mode.  The 3 display modes operate as 
*   follows:
*
*  NORMAL MODE       Outputs only mode and state information changes   
*                     and calculated outputs
*
*  QUIET MODE        No Outputs
*
*  DEBUG MODE        Outputs mode and state information, error counts,
*                    register displays, sensor states, and calculated output
*
*
* There is deliberate delay in switching between modes to allow the RS-232 cable 
* to be plugged into the header without causing problems. 
*******************************************************************************/

void set_display_mode(void)   
{
  UART_direct_msg_put("\r\nSelect Mode");
  UART_direct_msg_put("\r\n Hit NOR - Normal");
  UART_direct_msg_put("\r\n Hit QUI - Quiet");
  UART_direct_msg_put("\r\n Hit DEB - Debug" );
  UART_direct_msg_put("\r\n Hit V - Version#");
	UART_direct_msg_put("\r\n Hit REG - Register Values");
	UART_direct_msg_put("\r\n Hit SEN - Sensor Data");
  UART_direct_msg_put("\r\nSelect:  ");
  
}


//*****************************************************************************/
/// \fn void chk_UART_msg(void) 
///
//*****************************************************************************/
void chk_UART_msg(void)    
{
   UCHAR j;
   while( UART_input() )      // becomes true only when a byte has been received
   {                                    // skip if no characters pending
      j = UART_get();                 // get next character

      if( j == '\r' )          // on a enter (return) key press
      {                // complete message (all messages end in carriage return)
         UART_msg_put("->");
         UART_msg_process();
      }
      else 
      {
         if ((j != 0x02) )         // if not ^B
         {                             // if not command, then   
            UART_put(j);              // echo the character   
         }
         else
         {
           ;
         }
         if( j == '\b' ) 
         {                             // backspace editor
            if( msg_buf_idx != 0) 
            {                       // if not 1st character then destructive 
               UART_msg_put(" \b");// backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )  
         {                                // check message length too large
            UART_msg_put("\r\nToo Long!");
            msg_buf_idx = 0;
         }
         else if ((display_mode == QUIET) && (msg_buf[0] != 0x02) && 
                  (msg_buf[0] != 'D') && (msg_buf[0] != 'N') && 
                  (msg_buf[0] != 'V') &&
                  (msg_buf_idx != 0))
         {                          // if first character is bad in Quiet mode
            msg_buf_idx = 0;        // then start over
         }
         else {                        // not complete message, store character
 
            msg_buf[msg_buf_idx] = j;
            msg_buf_idx++;
            if (msg_buf_idx > 2)
            {
               UART_msg_process();
            }
         }
      }
   }
}

//*****************************************************************************/
///  \fn void UART_msg_process(void) 
///UART Input Message Processing
//*****************************************************************************/
void UART_msg_process(void)
{
   UCHAR chr,err=0;
//   unsigned char  data;


   if( (chr = msg_buf[0]) <= 0x60 ) 
   {      // Upper Case
      switch( chr ) 
      {
         case 'D':
            if((msg_buf[1] == 'E') && (msg_buf[2] == 'B') && (msg_buf_idx == 3)) 
            {
               display_mode = DEBUG;
               UART_msg_put("\r\nMode=DEBUG\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'N':
            if((msg_buf[1] == 'O') && (msg_buf[2] == 'R') && (msg_buf_idx == 3)) 
            {
               display_mode = NORMAL;
               UART_msg_put("\r\nMode=NORMAL\n");
               //display_timer = 0;
							 //display_flag = 1;
            }
            else
               err = 1;
            break;

         case 'Q':
            if((msg_buf[1] == 'U') && (msg_buf[2] == 'I') && (msg_buf_idx == 3)) 
            {
               display_mode = QUIET;
               UART_msg_put("\r\nMode=QUIET\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'V':
            display_mode = VERSION;
            UART_msg_put("\r\n");
            UART_msg_put( CODE_VERSION ); 
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;
                	 
				 case 'R':
					 if ((msg_buf[1] == 'E') && (msg_buf[2] == 'G') && (msg_buf_idx == 3)) 
					 {
						  display_mode = REGISTER;
						  UART_msg_put("\r\nMode= REGISTER \n");
              display_timer = 0;
						 
					 }
					 else 
						 err = 1;
					 break;
					 
				 case 'S':
					 if ((msg_buf[1] == 'E') && (msg_buf[2] == 'N') && (msg_buf_idx == 3)) 
					 {
						 display_mode = SENSOR; 
						 UART_msg_put("\r\nMode= SENSOR VALUES \n");
             display_timer = 0;
						 
					 }
					 
					 default:
            err = 1;
      }
   }

   else 
   {                                 // Lower Case
      switch( chr ) 
      {
        default:
         err = 1;
      }
   }

   if( err == 1 )
   {
      UART_msg_put("\n\rError!");
   }   
   else if( err == 2 )
   {
      UART_msg_put("\n\rNot in DEBUG Mode!");
   }   
   else
   {
    msg_buf_idx = 0;          // put index to start of buffer for next message
      ;
   }
   msg_buf_idx = 0;          // put index to start of buffer for next message


}


//*****************************************************************************
///   \fn   is_hex
/// Function takes 
///  @param a single ASCII character and returns 
///  @return 1 if hex digit, 0 otherwise.
///    
//*****************************************************************************
UCHAR is_hex(UCHAR c)
{
   if( (((c |= 0x20) >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))  )
      return 1;
   return 0;
}

/*******************************************************************************
*   \fn  DEBUG and DIAGNOSTIC Mode UART Operation
*******************************************************************************/
void monitor(void)
{

/**********************************/
/*     Spew outputs               */
/**********************************/

   switch(display_mode)
   {
      case(QUIET):
         {
             UART_msg_put("\r\n ");
             display_flag = 0;
         }  
         break;
      case(VERSION):
         {
             display_flag = 0;
         }  
         break;         
      case(NORMAL):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nNORMAL ");
               UART_msg_put(" Flow: ");
                             UART_hex_put(0x95);
               // ECEN 5803 add code as indicated
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
                             UART_hex_put(0x56);    
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               UART_hex_put(0x85);    
               UART_direct_msg_put(" \b ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               display_flag = 0;
							
            }
         }  
         break;
      case(DEBUG):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nDEBUG ");
               UART_msg_put(" Flow: ");
               UART_hex_put(0x81);    
               // ECEN 5803 add code as indicated               
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               UART_hex_put(0x52);        
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               UART_hex_put(0x87); 
							 UART_direct_msg_put(" \b");
							
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               
               /****************      ECEN 5803 add code as indicated   ***************/             
               //  Create a display of  error counts, sensor states, and
               //  ARM Registers R0-R15
               
               //  Create a command to read a section of Memory and display it
            
               //  Create a command to read 16 words from the current stack 
               // and display it in reverse chronological order.
              
                   
               // clear flag to ISR  
                             
               display_flag = 0;
             }   
         }  
         break;
				 
			case (REGISTER): 
			{
				if (display_flag == 1)
				{
					UART_msg_put("\r\nREGISTER VALUES ARE: ");
					
					 //Reading all Register values
               Reg0 = register_0();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R0:");  
               UART_direct_hex_put(Reg0);
                            
               Reg1=register_1();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R1:");  
               UART_direct_hex_put(Reg1);
                             
               Reg2=register_2();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R2:");  
               UART_direct_hex_put(Reg2);
                             
               Reg3=register_3();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R3:");  
               UART_direct_hex_put(Reg3);
               
               Reg4=register_4();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R4:");  
               UART_direct_hex_put(Reg4);
                             
               Reg5=register_5();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R5:");  
               UART_direct_hex_put(Reg5);
                             
               Reg6=register_6();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R6:");  
               UART_direct_hex_put(Reg6);
                             
               Reg7=register_7();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R7:");  
               UART_direct_hex_put(Reg7);
                             
                             
               Reg8=register_8();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R8:");  
               UART_direct_hex_put(Reg8);
                             
                             
               Reg9=register_9();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R9:");  
               UART_direct_hex_put(Reg9);
                             
               Reg10=register_10();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R10:"); 
               UART_direct_hex_put(Reg10);
                             
               Reg11=register_11();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R11:"); 
               UART_direct_hex_put(Reg11);
                             
               Reg12=register_12();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R12:"); 
               UART_direct_hex_put(Reg12);
                             
               Reg13=register_13();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R13:"); 
               UART_direct_hex_put(Reg13);
                             
               Reg14=register_14();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R14:"); 
               UART_direct_hex_put(Reg14);
                             
               Reg15=register_15();
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("R15:"); 
               UART_direct_hex_put(Reg15);
							 
							 display_flag = 0;
				}
			}
			break;
			
			case (SENSOR): 
			{
				if (display_flag == 1)
				{
					  
               //Displaying the Touch Slider Value             
               UART_direct_msg_put("\r\n");
               UART_direct_msg_put("Touch TSI value\r\n");
               float state = tsi.readPercentage();
               UART_direct_msg_put("TSI: ");
               UART_direct_hex_put(state);
                    
               //Displaying the Accelerometer Values 
                float x, y, z;
                UART_direct_msg_put("\r\n");
                UART_direct_msg_put("Accelerometer Values: \n\r");
                UART_direct_msg_put("X: ");
                x = abs(acc.getAccX());
                UART_direct_hex_put(x);
                
								UART_direct_msg_put("\r\n");
                UART_direct_msg_put("Y: ");
                y = abs(acc.getAccY());
                UART_direct_hex_put(y);
                
								UART_direct_msg_put("\r\n");
                UART_direct_msg_put("Z: ");
                z = abs(acc.getAccZ());
                UART_hex_put(z);
								
								display_flag = 0;
  
				}
				
			}
			break;			
      default:
      {
         UART_msg_put("Mode Error");
      }  
   }
}
