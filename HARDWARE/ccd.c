#include "ccd.h"
#include <math.h>
#include <stdlib.h>

struct CCD_State TSL1401_State[TSL1401_ADC_Channel_Size]  = {
//ADC_Channel_Index
	{0},
	{1},
	{2},
	{3},
};	//FRONT, BACK, LEFT, RIGHT

uint8_t TSL1401_Mode = 0;
uint8_t TSL1401_Is_Find_ADC_Threshold = 0;
uint8_t TSL1401_Is_Find_Line_Threshold = 0;

uint16_t TSL1401_cnt = 0;

__IO uint16_t ADC_ConvertedValue[TSL1401_ADC_Channel_Size] = {0};
__IO uint16_t TSL1401_ADC_Buffer[TSL1401_ADC_Buffer_Size][TSL1401_ADC_Channel_Size] = {0};
uint8_t TSL1401_ADC_Buffer_Idx = 0;

uint8_t TSL1401_Hash_Table[TSL1401_ADC_Channel_Size];	//Channel Index To Direction

void TSL1401_Init(void)
{	
	TSL1401_GPIO_Init();
	TSL1401_ADC_Init();
	TSL1401_TIM_Init();
	TSL1401_Hash_Table_Init();
}

void TSL1401_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
	
	TSL1401_CLK = 0;
	TSL1401_SI = 0;
}

void TSL1401_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ((u32)ADC1+0x4c);	
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = TSL1401_ADC_Channel_Size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
	DMA_ClearFlag(DMA2_Stream0, DMA_IT_TCIF0);	
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA2_Stream0, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;  
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = TSL1401_ADC_Channel_Size;                                    
	ADC_Init(ADC1, &ADC_InitStructure);
	//---------------------------------------------------------------------------

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_56Cycles);  //PC0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_56Cycles);  //PC1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_56Cycles);  //PC2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_56Cycles);  //PC3

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TSL1401_TIM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 1680-1;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //允许定时器更新中断
	TIM_Cmd(TIM3, DISABLE); //失能定时器
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //定时器中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TSL1401_Hash_Table_Init(void)
{
	u8 i;
	
	for (i = 0; i < TSL1401_ADC_Channel_Size; ++i)
	{
		TSL1401_Hash_Table[TSL1401_State[i].ADC_Channel_Index] = i;
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		if (TSL1401_cnt == 0)
		{
			TSL1401_CLK = 0;
			TSL1401_SI = 1;
			++TSL1401_cnt;
		}
		else if (TSL1401_cnt == 1)
		{			
			TSL1401_CLK = 1;
			TSL1401_SI = 1;
			++TSL1401_cnt;
		}
		else if (TSL1401_cnt < 258)
		{
			if ((TSL1401_cnt & 1) == 0)
			{
				TSL1401_CLK = 0;
				TSL1401_SI = 0;
				ADC_SoftwareStartConv(ADC1);
			}
			else
			{
				TSL1401_CLK = 1;
				TSL1401_SI = 0;
			}
			++TSL1401_cnt;
		}
		else if (TSL1401_cnt == 258)
		{
			TSL1401_CLK = 0;
			TSL1401_SI = 0;
			++TSL1401_cnt;
		}
		else if (TSL1401_cnt == 259)
		{
			TSL1401_CLK = 0;
			TSL1401_SI = 0;
			++TSL1401_cnt;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除中断标志位
	}
}

void DMA2_Stream0_IRQHandler(void)
{
	u8 j, k;
	struct CCD_State *ptr;
	
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET)
	{
		//Get Max and Min
		for (j = 0; j < TSL1401_ADC_Channel_Size; ++j)
		{
			TSL1401_ADC_Buffer[TSL1401_ADC_Buffer_Idx][j] = ADC_ConvertedValue[j];
			
			k = TSL1401_Hash_Table[j];
			ptr = &TSL1401_State[k];
			
			if (TSL1401_ADC_Buffer_Idx == 0+5)
				ptr->ADC_Max = ptr->ADC_Min = ADC_ConvertedValue[j];
			else if (TSL1401_ADC_Buffer_Idx < TSL1401_ADC_Buffer_Size - 5)
			{
				if (ptr->ADC_Max < ADC_ConvertedValue[j])
					ptr->ADC_Max = ADC_ConvertedValue[j];
				if (ptr->ADC_Min > ADC_ConvertedValue[j])
					ptr->ADC_Min = ADC_ConvertedValue[j];
			}
		}
		
		if (TSL1401_ADC_Buffer_Idx + 1 >= TSL1401_ADC_Buffer_Size)
		{
			TSL1401_ADC_Buffer_Idx = 0;
			
			for (k = 0; k < TSL1401_ADC_Channel_Size; ++k)
			{
				ptr = &TSL1401_State[k];
				ptr->ADC_Median = (ptr->ADC_Max + ptr->ADC_Min) / 2;
			}
			
			//Find ADC Threshold If Needed
			TSL1401_Find_ADC_Threshold();
			//Edge Detect
			TSL1401_Edge_Detect();
			//Find Line Threshold After Edge_Detect If Needed
			TSL1401_Find_Line_Threshold();
		}
		else
			++TSL1401_ADC_Buffer_Idx;
		
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

void TSL1401_Measure(void)
{
	u8 i, j, k;
	struct CCD_State *ptr;
	
	for (i = 0; i < TSL1401_ADC_Buffer_Size; ++i)
	{
		for (j = 0; j < TSL1401_ADC_Channel_Size; ++j)
		{
			if (i == 0+5)
			{
				k = TSL1401_Hash_Table[j];
				ptr = &TSL1401_State[k];
				ptr->ADC_Max = ptr->ADC_Min = TSL1401_ADC_Buffer[i][j];
			}
			else if (i < TSL1401_ADC_Buffer_Size - 5)
			{
				k = TSL1401_Hash_Table[j];
				ptr = &TSL1401_State[k];
				if (ptr->ADC_Max < TSL1401_ADC_Buffer[i][j])
					ptr->ADC_Max = TSL1401_ADC_Buffer[i][j];
				if (ptr->ADC_Min > TSL1401_ADC_Buffer[i][j])
					ptr->ADC_Min = TSL1401_ADC_Buffer[i][j];
			}
		}
	}
	for (k = 0; k < TSL1401_ADC_Channel_Size; ++k)
	{
		ptr = &TSL1401_State[k];
		ptr->ADC_Median = (ptr->ADC_Max + ptr->ADC_Min) / 2;
	}
	
	//Find ADC Threshold If Needed
	TSL1401_Find_ADC_Threshold();
	//Edge Detect
	TSL1401_Edge_Detect();
	//Find Line Threshold After Edge_Detect If Needed
	TSL1401_Find_Line_Threshold();
}

void TSL1401_Find_ADC_Threshold(void)
{
	u8 k;
	struct CCD_State *ptr;
	
	if (TSL1401_Is_Find_ADC_Threshold == 0)
	{
		for (k = 0; k < TSL1401_ADC_Channel_Size; ++k)
		{
			ptr = &TSL1401_State[k];
			
			if (TSL1401_Mode)
			{
				ptr->Line_ADC_Threshold = ptr->ADC_Max;
				ptr->Place_ADC_Threshold = ptr->ADC_Min;
			}
			else
			{
				ptr->Line_ADC_Threshold = ptr->ADC_Min;
				ptr->Place_ADC_Threshold = ptr->ADC_Max;
			}
			ptr->Difference_ADC_Threshold = (ptr->ADC_Max - ptr->ADC_Min) / 2;
		}
		TSL1401_Is_Find_ADC_Threshold = 1;
	}
}

void TSL1401_Edge_Detect(void)
{
	u8 i, j, k;
	struct CCD_State *ptr;
	
	for (j = 0; j < TSL1401_ADC_Channel_Size; ++j)
	{
		k = TSL1401_Hash_Table[j];
		ptr = &TSL1401_State[k];
		
		ptr->ADC_Median = (ptr->ADC_Max + ptr->ADC_Min) / 2;//计算出本次中间值
		if (ptr->ADC_Max - ptr->ADC_Min <= ptr->Difference_ADC_Threshold)
		{
			ptr->Line_Median_Pos = 0xFF;
			if (TSL1401_Mode)
			{
				if (abs((int)ptr->ADC_Max - (int)ptr->Line_ADC_Threshold) <= 
					abs((int)ptr->ADC_Max - (int)ptr->Place_ADC_Threshold))
				{
					ptr->Line_Left_Edge_Pos = 5;
					ptr->Line_Right_Edge_Pos = TSL1401_ADC_Buffer_Size - 5 - 5 + 1;
				}
				else
				{
					ptr->Line_Left_Edge_Pos = ptr->Line_Right_Edge_Pos = 0xFF;
				}
			}
			else
			{
				if (abs((int)ptr->ADC_Min - (int)ptr->Line_ADC_Threshold) <= 
					abs((int)ptr->ADC_Min - (int)ptr->Place_ADC_Threshold))
				{
					ptr->Line_Left_Edge_Pos = 5;
					ptr->Line_Right_Edge_Pos = TSL1401_ADC_Buffer_Size - 5 - 5 + 1;
				}
				else
				{
					ptr->Line_Left_Edge_Pos = ptr->Line_Right_Edge_Pos = 0xFF;
				}
			}
			continue;
		}
		else
		{
			ptr->Line_Left_Edge_Pos = 0xFF;
			ptr->Line_Right_Edge_Pos = 0xFF;
		}
		
		if (TSL1401_Mode)
		{
			for (i = 5; i < TSL1401_ADC_Buffer_Size - 5 - 5; ++i)
			{
				if (TSL1401_ADC_Buffer[i][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+1][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+2][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+3][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+4][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+5][j] > ptr->ADC_Median)
				{
					ptr->Line_Left_Edge_Pos = i+2;
					break;
				}
			}
			for (i = TSL1401_ADC_Buffer_Size - 5 - 5 - 1; i >= 5; --i)
			{
				if (TSL1401_ADC_Buffer[i][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+1][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+2][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+3][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+4][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+5][j] <= ptr->ADC_Median)
				{
					ptr->Line_Right_Edge_Pos = i+2;
					break;
				}
			}
		}
		else
		{
			for (i = 5; i < TSL1401_ADC_Buffer_Size - 5 - 5; ++i)
			{
				if (TSL1401_ADC_Buffer[i][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+1][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+2][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+3][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+4][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+5][j] <= ptr->ADC_Median)
				{
					ptr->Line_Left_Edge_Pos = i+2;
					break;
				}
			}
			for (i = TSL1401_ADC_Buffer_Size - 5 - 5 - 1; i >= 5; --i)
			{
				if (TSL1401_ADC_Buffer[i][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+1][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+2][j] <= ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+3][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+4][j] > ptr->ADC_Median && 
					TSL1401_ADC_Buffer[i+5][j] > ptr->ADC_Median)
				{
					ptr->Line_Right_Edge_Pos = i+2;
					break;
				}
			}
		}
		ptr->Line_Median_Pos = (ptr->Line_Left_Edge_Pos + ptr->Line_Right_Edge_Pos) / 2;
	}
}

void TSL1401_Find_Line_Threshold(void)
{
	u8 j, k;
	struct CCD_State *ptr;
	
	if (TSL1401_Is_Find_Line_Threshold == 0)
	{
		for (j = 0; j < TSL1401_ADC_Channel_Size; ++j)
		{
			k = TSL1401_Hash_Table[j];
			ptr = &TSL1401_State[k];
			
			ptr->Line_Left_Edge_Pos_Threshold = ptr->Line_Left_Edge_Pos;
			ptr->Line_Right_Edge_Pos_Threshold = ptr->Line_Right_Edge_Pos;
			ptr->Line_Median_Pos_Threshold = ptr->Line_Median_Pos;
		}
		TSL1401_Is_Find_Line_Threshold = 1;
	}
}

void TSL1401_SetEnabled(u8 enabled)
{
	if (enabled)
	{
		TIM_Cmd(TIM3, ENABLE);
	}
	else
	{
		TIM_Cmd(TIM3, DISABLE);
	}
}

void TSL1401_Refresh(void)
{
	TSL1401_cnt = 0;
}
