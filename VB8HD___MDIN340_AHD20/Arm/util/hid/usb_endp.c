/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "..\..\usb\usb_lib.h"	//by hungry 2013.06.18 usb
#include "usb_istr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[2];
//extern __IO uint8_t PrevXferComplete;	//by hungry 2013.06.18 usb
__IO uint8_t PrevXferComplete;	//by hungry 2013.06.18 usb
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
  BitAction Led_State;

  /* Read received data (2 bytes) */  
  USB_SIL_Read(EP1_OUT, Receive_Buffer);
  
  if (Receive_Buffer[1] == 0)
  {
    Led_State = Bit_RESET;
  }
  else 
  {
    Led_State = Bit_SET;
  }
 
 
  switch (Receive_Buffer[0])
  {
    case 1: /* Led 1 */
     if (Led_State != Bit_RESET)
     {
       //STM_EVAL_LEDOn(LED1);	//by hungry 2013.06.18 usb
     }
     else
     {
       //STM_EVAL_LEDOff(LED1);	//by hungry 2013.06.18 usb
     }
     break;
    case 2: /* Led 2 */
     if (Led_State != Bit_RESET)
     {
       //STM_EVAL_LEDOn(LED2);	//by hungry 2013.06.18 usb
     }
     else
     {
       //STM_EVAL_LEDOff(LED2);	//by hungry 2013.06.18 usb
     }
      break;
    case 3: /* Led 3 */
     if (Led_State != Bit_RESET)
     {
       //STM_EVAL_LEDOn(LED3);	//by hungry 2013.06.18 usb
     }
     else
     {
       //STM_EVAL_LEDOff(LED3);	//by hungry 2013.06.18 usb
     }
      break;
    case 4: /* Led 4 */
     if (Led_State != Bit_RESET)
     {
       //STM_EVAL_LEDOn(LED4);	//by hungry 2013.06.18 usb
     }
     else
     {
       //STM_EVAL_LEDOff(LED4);	//by hungry 2013.06.18 usb
     }
      break;
  default:
    //STM_EVAL_LEDOff(LED1);	//by hungry 2013.06.18 usb
    //STM_EVAL_LEDOff(LED2);
    //STM_EVAL_LEDOff(LED3);
    //STM_EVAL_LEDOff(LED4);
    break;
  }
 
  SetEPRxStatus(ENDP1, EP_RX_VALID);
 
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  PrevXferComplete = 1;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

