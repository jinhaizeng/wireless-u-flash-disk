#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"	
#include "touch.h"		
#include "usart3.h"
#include "common.h" 

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 


//ALIENTEK̽����STM32F407������ ��չʵ��5
//ATK-RM04 WIFIģ�����ʵ��   -�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 



USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB״̬
extern vu8 bDeviceState;		//USB���� ���


int main(void)
{  

  //usb���ֲ�������
  u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
  
  
	u8 key,fontok=0; 
  u32 total,free;
	u8 t=0;	
	u8 res=0;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	W25QXX_Init();				//��ʼ��W25Q128
	tp_dev.init();				//��ʼ��������
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	key=KEY_Scan(0);  
	if(key==KEY0_PRES)		//ǿ��У׼
	{
		LCD_Clear(WHITE);	//����
		TP_Adjust();  		//��ĻУ׼ 
		TP_Save_Adjdata();	  
		LCD_Clear(WHITE);	//����
	}
	fontok=font_init();		//����ֿ��Ƿ�OK
	if(fontok||key==KEY1_PRES)//��Ҫ�����ֿ�				 
	{
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
    
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//��SD������
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  
  
  while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	res=f_mount(fs[1],"1:",1); 				//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
      printf("Flash Disk Format Finish\r\n");
//			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
		}else printf("Flash Disk Format Error\r\n");      //LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}													    
		  
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
    printf("SD Card Fatfs Error!");
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//�����ʾ			  
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}													  			    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	
  printf("FATFS OK!\r\n");
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
  
  
  

  //usb���ִ���
  LCD_ShowString(30,170,200,16,16,"USB Connecting...");//��ʾ���ڽ������� 	    
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	delay_ms(1800);	
	while(1)
	{	
    
    
    
    Show_Str_Mid(0,30,"ATK-ESP8266 WIFIģ�����",16,240); 
	while(atk_8266_send_cmd("AT","OK",20))//���WIFIģ���Ƿ�����
	{
		atk_8266_quit_trans();//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	
		Show_Str(40,55,200,16,"δ��⵽ģ��!!!",16,0);
		delay_ms(800);
		LCD_Fill(40,55,200,55+16,WHITE);
		Show_Str(40,55,200,16,"��������ģ��...",16,0); 
	} 
		while(atk_8266_send_cmd("ATE0","OK",20));//�رջ���
  
  
  
    delay_ms(10); 
	  atk_8266_at_response(1);//���ATK-ESP8266ģ�鷢�͹���������,��ʱ�ϴ�������
    Show_Str_Mid(0,30,"ATK-ESP WIFI-STA ����",16,240);
		Show_Str_Mid(0,50,"��������ATK-ESP8266ģ�飬���Ե�...",12,240);
		atk_8266_wifista_test();//WIFI STA����
  
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
		{	 						   
			LCD_Fill(30,190,240,190+16,WHITE);//�����ʾ			  	   
			if(USB_STATUS_REG&0x01)//����д		  
			{
				LED1=0;
				LCD_ShowString(30,190,200,16,16,"USB Writing...");//��ʾUSB����д������	 
			}
			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				LED1=0;
				LCD_ShowString(30,190,200,16,16,"USB Reading...");//��ʾUSB���ڶ�������  		 
			}	 										  
			if(USB_STATUS_REG&0x04)LCD_ShowString(30,210,200,16,16,"USB Write Err ");//��ʾд�����
			else LCD_Fill(30,210,240,210+16,WHITE);//�����ʾ	  
			if(USB_STATUS_REG&0x08)LCD_ShowString(30,230,200,16,16,"USB Read  Err ");//��ʾ��������
			else LCD_Fill(30,230,240,230+16,WHITE);//�����ʾ    
			USB_STA=USB_STATUS_REG;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)LCD_ShowString(30,170,200,16,16,"USB Connected    ");//��ʾUSB�����Ѿ�����
			else LCD_ShowString(30,170,200,16,16,"USB DisConnected ");//��ʾUSB���γ���
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1=1;
			LED0=!LED0;//��ʾϵͳ������
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=1;
			}else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s��û�յ����߱��,����USB���γ���
			}
			USB_STATUS_REG=0;
		}
	};  
  
  
	//atk_8266_test();		//����ATK_ESP8266����
}













