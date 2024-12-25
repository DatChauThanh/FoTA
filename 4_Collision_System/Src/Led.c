/*
 * Led.c
 *
 *  Created on: Mar 30, 2024
 *      Author: Chau Thanh Dat
 */
#include"Led_Interface.h"
#include"Rte_DistanceSensor.h"

void Led_MainFunction(void)
{
	Std_ReturnType status = E_OK ;
	uint32_t Distance = 0 ;
	status = RTE_READ_ULTRASENSOR_DISTANCE_DATA(&Distance);
	if( status == E_OK)
	{
		if(Distance <= WARNING_DISTANCE)/*Set Buzzer Pulse*/
		{
			Led_Blink(&Distance);
		}
		else
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		}
    }
	else
	{
		// Do nothing
	}
}

static void Led_Blink(uint32_t *Distance)
{
	uint32_t Copy_Distance = *(Distance);
	if (Copy_Distance <= 4){
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	}
}



