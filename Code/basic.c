#include "basic.h"
uint8_t massage[20];
uint8_t num1;

uint8_t num3_MODE1=1;
uint8_t num3_MODE2=1;
uint8_t num3_flag=0;

uint8_t PAGE=1;
uint8_t PAGE_NOW=1;

int TIM2_NUM_CNT;
int TIM3_NUM_CNT;

int TIM2_fre_IC;
int TIM3_fre_IC;

int TIM2_fre_previous_IC;
int TIM3_fre_previous_IC;

int TIM2_fre_last_IC;
int TIM3_fre_last_IC;

int NHA;
int NHB;
int NDA;
int NDB;


int PD=1000;
int PH=5000;
int PX=0;

int TIME2_freA;
int TIME2_freB;

int fA_min,fA_max;
int fB_min,fB_max;

uint8_t FLAG_A=0;
uint8_t FLAG_B=0;

uint8_t FLAG_last_A=0;
uint8_t FLAG_last_B=0;

int state;
int event_flag;
int dir;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			TIM2_NUM_CNT=TIM2->CNT;
			TIM2->CNT=0;
		}
	}
	if(htim->Instance==TIM3)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			TIM3_NUM_CNT=TIM3->CNT;
			TIM3->CNT=0;
		}
	}
}
 void fre_tim_GET()
 {
	 TIM15->CNT=0;
	 TIM2_fre_IC=1000000/TIM2_NUM_CNT;
	 TIM3_fre_IC=1000000/TIM3_NUM_CNT;
	 
	 TIM2_fre_IC+=PX;
	 TIM3_fre_IC+=PX;
 }
 void FRE_change_A();
 void FRE_change_B();
 
 void FRE_HIGH()
 {
	 //FA>PH
	 if(TIM2_fre_IC>TIM2_fre_last_IC && TIM2_fre_IC>PH && TIM2_fre_last_IC<PH)
	 {
		 NHA++;
		 led_show(2,1);
	 }
	 else if(TIM2_fre_IC>PH && TIM2_fre_last_IC>PH)
	 {
		 led_show(2,1);
	 }
	 else if(TIM2_fre_IC<PH && TIM2_fre_last_IC<PH)
	 {
		 led_show(2,0);
	 }
	 //FB>PH
	 if(TIM3_fre_IC>TIM3_fre_last_IC && TIM3_fre_IC>PH && TIM3_fre_last_IC<PH)
	 {
		 NHB++;
		 led_show(3,1);
	 }
	 else if(TIM3_fre_IC>PH && TIM3_fre_last_IC>PH)
	 {
		 led_show(3,1);
	 }
	 else if(TIM3_fre_IC<PH && TIM3_fre_last_IC<PH)
	 {
		 led_show(3,0);
	 }
	 
	 if(NHA+NHB>=3)
	 {
		 led_show(8,1);
	 }
	 else
	 {
		 led_show(8,0);
	 }
	 FRE_change_B();
	 FRE_change_A();
	 
	 TIM2_fre_last_IC=TIM2_fre_IC;
	 TIM3_fre_last_IC=TIM3_fre_previous_IC;
	 
	 TIM3_fre_previous_IC=TIM3_fre_IC;
	 
 }
 
void FRE_change_A()
{
	if(TIM2_fre_last_IC==0)
	{
		TIM2_fre_last_IC=TIM2_fre_IC;
	}
	
	if(TIM2_fre_IC>TIM2_fre_last_IC){dir=1;}
	else if(TIM2_fre_IC<TIM2_fre_last_IC){dir=-1;}
	
	switch(state)
	{
		case 0: //IDLE
		{
			if(dir==1)
			{
				fA_max=TIM2_fre_IC;
				fA_min=TIM2_fre_IC;
				state=1;
				event_flag=0;
				TIM15->CNT=0;
			}
			else if(dir==-1)
			{
				fA_max=TIM2_fre_IC;
				fA_min=TIM2_fre_IC;
				state=2;
				event_flag=0;
				TIM15->CNT=0;
			}
		}break;
		case 1: //UP
		{
			if(dir==-1)
			{
				fA_max=fA_max;
				fA_min=TIM2_fre_IC;
				state=2;
				event_flag=0;
				TIM15->CNT=0;
			}
			else
			{
				fA_max=TIM2_fre_IC;
			}
		}break;
		case 2: //DOWN
		{
			if(dir==1)
			{
				fA_min=fA_min;
				fA_max=TIM2_fre_IC;
				state=1;
				event_flag=0;
				TIM15->CNT=0;
			}
			else
			{
				fA_min=TIM2_fre_IC;
			}
		}break;
	}
	
	if(event_flag==0 && state!=0)
	{
		if((fA_max-fA_min)>PD && TIM15->CNT<30000)
		{
			NDA++;
			event_flag=1;
			state=0;
		}
	}
	
	if(TIM15->CNT>=30000 && state!=0)
	{
		state=0;
	}
	
	
}

void FRE_change_B()
{
	if(TIM3_fre_IC< TIM3_fre_last_IC && FLAG_B==0)
	{
		fB_min=TIM3_fre_IC;
		TIM16->CNT=0;
	}
	else if(TIM3_fre_IC> TIM3_fre_last_IC)
	{
		FLAG_B=1;
	}
	
	if(TIM3_fre_IC> TIM3_fre_last_IC && FLAG_B==1)
	{
		fB_max=TIM3_fre_IC;
		if(fB_max-fB_min>PD && TIM16->CNT<30000)
		{
			NDB++;
			FLAG_B=0;
		}
	}
	
	if(TIM3_fre_IC> TIM3_fre_last_IC && FLAG_B==0)
	{
		fB_max=TIM3_fre_IC;
		TIM16->CNT=0;
	}
	else if(TIM3_fre_IC< TIM3_fre_last_IC)
	{
		FLAG_B=1;
	}
	
	if(TIM3_fre_IC< TIM3_fre_last_IC && FLAG_B==1)
	{
		fB_min=TIM3_fre_IC;
		if(fB_max-fB_min>PD && TIM16->CNT<30000)
		{
			NDB++;
			FLAG_B=0;
		}
	}
	
}

void led_show(uint8_t led,uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(mode)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8 <<(led-1),GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8 <<(led-1),GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
uint8_t B1_state;
uint8_t B1_last_state=1;

uint8_t B2_state;
uint8_t B2_last_state=1;

uint8_t B3_state;
uint8_t B3_last_state=1;

uint8_t B4_state;
uint8_t B4_last_state=1;

void key_scan()
{
	B1_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	//B1
	if(B1_state==1 && B1_last_state==0)
	{
		//PAGE 2 button1
		if(PAGE==2)
		{
			if(num3_MODE2==1)
			{
				PD+=100;
				//PD boarder 
				if(PD<100)
				{
					PD=100;
				}
				else if(PD>1000)
				{
					PD=1000;
				}
			}
			else if(num3_MODE2==2)
			{
				PH+=100;
				//PH boarder 
				if(PH<1000)
				{
					PH=1000;
				}
				else if(PH>10000)
				{
					PH=10000;
				}
			}
			else if(num3_MODE2==3)
			{
				PX+=100;
				//PX boarder 
				if(PX>1000)
				{
					PX=1000;
				}
				else if(PD<-1000)
				{
					PX=-1000;
				}
			}
		}
	}
	//B2
	if(B2_state==1 && B2_last_state==0)
	{
		//PAGE 2 button2
		if(PAGE==2)
		{
			if(num3_MODE2==1)
			{
				PD-=100;
				//PD boarder 
				if(PD<100)
				{
					PD=100;
				}
				else if(PD>1000)
				{
					PD=1000;
				}
			}
			else if(num3_MODE2==2)
			{
				PH-=100;
				//PH boarder 
				if(PH<1000)
				{
					PH=1000;
				}
				else if(PH>10000)
				{
					PH=10000;
				}
			}
			else if(num3_MODE2==3)
			{
				PX-=100;
				//PX boarder 
				if(PX>1000)
				{
					PX=1000;
				}
				else if(PD<-1000)
				{
					PX=-1000;
				}
			}
		}
	}
	//B3
	if(B3_last_state==1 && B3_state==0)
	{
		TIM4->CNT=0;
	}
	else if(B3_last_state==0 && B3_state==0)
	{
		//PAGE3 short long
		if(TIM4->CNT>10000)
		{
			if(PAGE==3)
			{
				num3_flag=1;
			}
		}
	}
	else if(B3_last_state==0 && B3_state==1)
	{
		if(TIM4->CNT<10000)
		{
			//PAGE1 short
			if(PAGE==1)
			{
				num3_MODE1++;
				if(num3_MODE1>2)
				{
				 num3_MODE1=1;
				}
			}
			//PAGE2 short
			if(PAGE==2)
			{
				num3_MODE2++;
				if(num3_MODE2>3)
				{
				num3_MODE2=1;
				}
			}
			//PAGE3 short NULL
			
		}
	}
	//B4
	if(B4_state==1 && B4_last_state==0)
	{
		PAGE++;
		if(PAGE>3)
		{
			PAGE=1;
		}
	}
	
	
	B1_last_state=B1_state;
	B2_last_state=B2_state;
	B3_last_state=B3_state;
	B4_last_state=B4_state;
}
void led_show_page()
{
	if(PAGE==1)
	{
		led_show(1,1);
	}
	else
	{
		led_show(1,0);
	}
}
void led_SHOW()
{
	
	if(PAGE!=PAGE_NOW)
	{
		LCD_Clear(Black);
		num3_MODE1=1;
		num3_MODE2=1;
	}
	led_show_page();
	switch(PAGE)
	{
		case 1:{
			sprintf((char*)massage,"        DATA       ");
			LCD_DisplayStringLine(Line1,massage);
			if(num3_MODE1==1)
			{
				fre_tim_GET();
				FRE_HIGH();
				//A channel display
				if(TIM2_fre_IC<1000 && TIM2_fre_IC>=0)
				{
					sprintf((char*)massage,"     A=%dHz    ",TIM2_fre_IC);
					LCD_DisplayStringLine(Line3,massage);
				}
				else if(TIM2_fre_IC>=1000)
				{
					sprintf((char*)massage,"     A=%dKHz    ",TIM2_fre_IC/1000);
					LCD_DisplayStringLine(Line3,massage);
				}
				else if(TIM2_fre_IC<0)
				{
					sprintf((char*)massage,"     A=NULL    ");
					LCD_DisplayStringLine(Line3,massage);
				}
				//B channel display
				if(TIM3_fre_IC<1000 && TIM3_fre_IC>=0)
				{
					sprintf((char*)massage,"     B=%dHz    ",TIM3_fre_IC);
					LCD_DisplayStringLine(Line4,massage);
				}
				else if(TIM3_fre_IC>=1000)
				{
					sprintf((char*)massage,"     B=%dKHz    ",TIM3_fre_IC/1000);
					LCD_DisplayStringLine(Line4,massage);
				}
				else if(TIM3_fre_IC<0)
				{
					sprintf((char*)massage,"     B=NULL    ");
					LCD_DisplayStringLine(Line4,massage);
				}
				sprintf((char*)massage," MAX=%d   %d",fA_max,fB_max);
				LCD_DisplayStringLine(Line6,massage);
				sprintf((char*)massage," MIN=%d   %d",fA_min,fB_min);
				LCD_DisplayStringLine(Line7,massage);
			}
			else if(num3_MODE1==2)
			{
				fre_tim_GET();
				FRE_HIGH();
				//A channel display
				if(TIM2_NUM_CNT<1000)
				{
					sprintf((char*)massage,"     A=%dus    ",TIM2_NUM_CNT);
					LCD_DisplayStringLine(Line3,massage);
				}
				else
				{
					sprintf((char*)massage,"     A=%dms    ",TIM2_NUM_CNT/1000);
					LCD_DisplayStringLine(Line3,massage);
				}
				//B channel display
				if(TIM3_NUM_CNT<1000)
				{
					sprintf((char*)massage,"     B=%dus     ",TIM3_NUM_CNT);
					LCD_DisplayStringLine(Line4,massage);
				}
				else
				{
					sprintf((char*)massage,"     B=%dms     ",TIM3_NUM_CNT/1000);
					LCD_DisplayStringLine(Line4,massage);
				}
			}
		}break;
		case 2:{
			sprintf((char*)massage,"        PARA       ");
			LCD_DisplayStringLine(Line1,massage);
			sprintf((char*)massage,"     PD=%dHz   ",PD);
			LCD_DisplayStringLine(Line3,massage);
			sprintf((char*)massage,"     PH=%dHz   ",PH);
			LCD_DisplayStringLine(Line4,massage);
			sprintf((char*)massage,"     PX=%dHz   ",PX);
			LCD_DisplayStringLine(Line5,massage);
			
		}break;
		case 3:{
			if(NHA+NHB>=3)
			{
				led_show(8,1);
			}
			else
			{
				led_show(8,0);
			}
			sprintf((char*)massage,"        RECD       ");
			LCD_DisplayStringLine(Line1,massage);
			sprintf((char*)massage,"     NDA=%d    ",NDA);
			LCD_DisplayStringLine(Line3,massage);
			sprintf((char*)massage,"     NDB=%d    ",NDB);
			LCD_DisplayStringLine(Line4,massage);
			sprintf((char*)massage,"     NHA=%d    ",NHA);
			LCD_DisplayStringLine(Line5,massage);
			sprintf((char*)massage,"     NHB=%d    ",NHB);
			LCD_DisplayStringLine(Line6,massage);
			if(num3_flag==1)
			{
				num3_flag=0;
				NHA=0;
				NHB=0;
				NDA=0;
				NDB=0;
			}
		}break;
	}
	PAGE_NOW=PAGE;
}