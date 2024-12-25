/*
 * UltraSensor.c
 *
 *  Created on: Mar 29, 2024
 *      Author: Chau Thanh Dat
 */
#include "UltraSensor_Interface.h"
#include "Rte_DistanceSensor.h"

static uint32_t Val1 = 0;
static uint32_t Val2 = 0;
static uint32_t Difference = 0;
static uint32_t  pMillis  = 0;  // is the first value captured
static uint8_t  Distance  = 0;

void UltraSensor_MainFunction(void)
{
	UltraSensor_HCSR04_Read();
	HAL_Delay(200);
}

static void UltraSensor_HCSR04_Read (void)
{
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	UltraSensor_Delay_MicroSec(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low

	pMillis = HAL_GetTick();
	while (!(HAL_GPIO_ReadPin (ECHO_GPIO_Port, ECHO_Pin)) && pMillis + 10 >  HAL_GetTick());
	Val1 = __HAL_TIM_GET_COUNTER (&htim2);

	pMillis = HAL_GetTick();
	while ((HAL_GPIO_ReadPin (ECHO_GPIO_Port, ECHO_Pin)) && pMillis + 50 > HAL_GetTick());
	Val2 = __HAL_TIM_GET_COUNTER (&htim2);

	Difference = Val2-Val1;
	Distance = Difference* 0.034/2;
	RTE_WRITE_ULTRASENSOR_DISTANCE_DATA(Distance);
}

static void UltraSensor_Delay_MicroSec (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while (__HAL_TIM_GET_COUNTER (&htim2) < time);
}
