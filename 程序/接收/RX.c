#include <reg52.h>
#include <intrins.h>
#include "12864.h"
#include "ds1302.h"


uint stwdh=35;		   //�¶�����
uint stwdl=6;		   //�¶�����

uint stsdh=85;		   //ʪ������
uint stsdl=20;		   //ʪ������
sbit  fmq  =P1^7;      //������

#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32 	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
uint const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
uint const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//**************************************************************************************


uint wendu=0;
uint shidu=0;
unsigned int idata pm25=0;
unsigned char  idata RxBuf[32];
unsigned int flag=0;
uchar  hor,min,sec,SetA=0,SetB=0; 
unsigned char year=17,month=2,date=11;
unsigned int Alpm25=200;
uchar  initial_time[]={0x00,0x00,0x00};
unsigned int guangzhao=0;

//****************************************IO�˿ڶ���***************************************
sbit 	MISO	=P0^2;
sbit 	MOSI	=P0^4;
sbit	SCK	=P0^1;
sbit	CE	   =P0^0;
sbit	CSN	=P0^3;
sbit	IRQ	=P0^5;


sbit    k1 	=P2^0;  //����ʱ��
sbit    k2 	=P2^1;  //���ò���������
sbit    k3 	=P2^2;  // +
sbit    k4 	=P2^3;  // -
sbit    k5 	=P2^4;  // ȷ��

void Delay(unsigned int s);
void inerDelay_us(unsigned char n);
void init_NRF24L01(void);
uint SPI_RW(uint uchar);
uchar SPI_Read(uchar reg);
void SetRX_Mode(void);
uint SPI_RW_Reg(uchar reg, uchar value);
uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars);
uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars);
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);

void delay_ms(unsigned int z)
{ 
   uint x,y;
   for(x=z;x>0;x--)
   for(y=110;y>0;y--);
}
//*****************************************����ʱ*****************************************
void Delay(unsigned int s)
{
	unsigned int i;
	for(i=0; i<s; i++);
	for(i=0; i<s; i++);
}
//******************************************************************************************
uint 	bdata sta;   //״̬��־
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
/******************************************************************************************
/*��ʱ����
/******************************************************************************************/
void inerDelay_us(unsigned char n)
{
	for(;n>0;n--)	_nop_();
}
//****************************************************************************************
/*NRF24L01��ʼ��
//***************************************************************************************/
void init_NRF24L01(void)
{
   inerDelay_us(100);
 	CE=0;    // chip enable
 	CSN=1;   // Spi disable 
 	SCK=0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
}
/****************************************************************************************************
/*������uint SPI_RW(uint uchar)
/*���ܣ�NRF24L01��SPIдʱ��
/****************************************************************************************************/
uint SPI_RW(uint uchar)
{
	uint bit_ctr;
   for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
   {
		MOSI = (uchar & 0x80);         // output 'uchar', MSB to MOSI
		uchar = (uchar << 1);           // shift next bit into MSB..
		SCK = 1;                      // Set SCK high..
		uchar |= MISO;       		  // capture current MISO bit
		SCK = 0;            		  // ..then set SCK low again
   }
   return(uchar);           		  // return read uchar
}
/****************************************************************************************************
/*������uchar SPI_Read(uchar reg)
/*���ܣ�NRF24L01��SPIʱ��
/****************************************************************************************************/
uchar SPI_Read(uchar reg)
{
	uchar reg_val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
}
/****************************************************************************************************/
/*���ܣ�NRF24L01��д�Ĵ�������
/****************************************************************************************************/
uint SPI_RW_Reg(uchar reg, uchar value)
{
	uint status;
	
	CSN = 0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	CSN = 1;                   // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************/
/*������uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
/****************************************************************************************************/
uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uint status,uchar_ctr;
	
	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0);    // 
	
	CSN = 1;                           
	
	return(status);                    // return nRF24L01 status uchar
}
/*********************************************************************************************************
/*������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
/*********************************************************************************************************/
uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uint status,uchar_ctr;
	
	CSN = 0;            //SPIʹ��       
	status = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //�ر�SPI
	return(status);    // 
}
/****************************************************************************************************/
/*������void SetRX_Mode(void)
/*���ܣ����ݽ������� 
/****************************************************************************************************/
void SetRX_Mode(void)
{
	CE=0;
//	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1; 
	inerDelay_us(130);
}
/******************************************************************************************************/
/*������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
   unsigned char revale=0;
	sta=SPI_Read(STATUS);	// ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(RX_DR)				// �ж��Ƿ���յ�����
	{
	    CE = 0; 			//SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;			//��ȡ������ɱ�־
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	return revale;
}
/***********************************************************************************************************
/*������void nRF24L01_TxPacket(unsigned char * tx_buf)
/*���ܣ����� tx_buf������
/**********************************************************************************************************/
/*void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	CE=0;			//StandBy Iģʽ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	CE=1;		 //�ø�CE���������ݷ���
	inerDelay_us(10);
}*/


 /******************************************************************** 
du1302
***********************************************************************/



void du_ds1302() 
{
    uchar k;
	
 //   uchar taddr = 0x81;
  // for (k=0; k<3; k++)
 //   {
  //      now_time[k] = read_data_ds1302(taddr);/*��ʽΪ: �� �� ʱ �� �� ���� �� */
  //      taddr+= 2;
 //   }

   k= read_data_ds1302(0x81);
   k=bcd16(k);
   if(k<60) sec=k;
   if(k>=60)
	{
		k= read_data_ds1302(0x81);
		k=bcd16(k);
		if(k<60) sec=k;
		if(k>=60)
		{
			k= read_data_ds1302(0x81);
			k=bcd16(k);
			if(k<60) sec=k;
			if(k>=60)
			{
				write_data_ds1302(0x80,hexbcd(0)); //��DS1302��д��Ĵ���80Hд���ʼ������00
				write_data_ds1302(0x82,hexbcd(0));//��DS1302��д�ּĴ���82Hд���ʼ������00
				write_data_ds1302(0x84,hexbcd(8));//��DS1302��дСʱ�Ĵ���84Hд���ʼСʱ����12
				write_data_ds1302(0x86,hexbcd(11));//��DS1302��д���ڼĴ���86Hд���ʼ��������07
				write_data_ds1302(0x88,hexbcd(2));//��DS1302��д�·ݼĴ���88Hд���ʼ�·�����01
				write_data_ds1302(0x8c,hexbcd(17));//��DS1302��д��ݼĴ���8cHд���ʼ�������10
				write_data_ds1302(0x8e,0x80);	
				date=11;month=2;year=17;
			}
		}
	}
	
   k= read_data_ds1302(0x83);
   k=bcd16(k);
   //if(k<60)
		min=k;
	
   k=read_data_ds1302(0x85);
   k=bcd16(k);
   //if(k<24)
		hor=k;
	
	k=read_data_ds1302(0x87);
   k=bcd16(k);
   //if(k<24)
		date=k;
	k=read_data_ds1302(0x89);
   k=bcd16(k);
   //if(k<24)
		month=k;
	k=read_data_ds1302(0x8d);
   k=bcd16(k);
   //if(k<24)
		year=k;
}

void xianshishijian(void)
{
	LCD_Setaddress(1,1);

	print("ʱ��");
	LCD_write_dat(':');
	LCD_write_dat(hor/10+0x30);  
	LCD_write_dat(hor%10+0x30); 
	LCD_write_dat(':'); 
	LCD_write_dat(min/10+0x30);  
	LCD_write_dat(min%10+0x30); 
	LCD_write_dat(':');
	LCD_write_dat(sec/10+0x30);  
	LCD_write_dat(sec%10+0x30);
	LCD_write_dat(' ');
	LCD_write_dat(' ');
	LCD_write_dat(' ');
}


void xianshiwsd(void)//��ʾ��ʪ��

{

 	 LCD_Setaddress(2,1);
	 //if(wendu>=99)wendu=99;
    print("�¶�");
    LCD_write_dat(':');
    LCD_write_dat(wendu/10+0x30);  
    LCD_write_dat(wendu%10+0x30); 
	 LCD_write_dat(' ');
	 print("ʪ��");
    LCD_write_dat(':');
    LCD_write_dat(shidu/10+0x30);  
    LCD_write_dat(shidu%10+0x30);
   LCD_write_dat(' ');
}


void xianshiguangzhao(void)
{
	LCD_Setaddress(4,1);
	print("����");
   LCD_write_dat(':');
   LCD_write_dat(guangzhao/10000+0x30); 
   LCD_write_dat(guangzhao%10000/1000+0x30);  
   LCD_write_dat(guangzhao%1000/100+0x30); 
	LCD_write_dat(guangzhao%100/10+0x30); 
   LCD_write_dat(guangzhao%10+0x30); 
	LCD_write_dat(' ');
	LCD_write_dat('L');
   LCD_write_dat('x');
	LCD_write_dat(' ');
	LCD_write_dat(' ');
	LCD_write_dat(' ');
}


void xianshipm(void)
{
	LCD_Setaddress(3,1);

   print("PM2.5");
   LCD_write_dat(':');
	LCD_write_dat(pm25/100+0x30); 
   LCD_write_dat(pm25%100/10+0x30);  
   LCD_write_dat(pm25%10+0x30); 
	LCD_write_dat('u');
	LCD_write_dat('g');
	LCD_write_dat('/');
   LCD_write_dat('m');
   LCD_write_dat('3');
	LCD_write_dat(' ');
	LCD_write_dat(' ');
}

void ShowSetTime(void)
{
	LCD_clr();
	LCD_Setaddress(1,2);
	print("��������ʱ��");
	LCD_Setaddress(3,1);
	print("����");
	LCD_write_dat(':');
	LCD_write_dat(' ');
	LCD_write_dat(year/10+0x30);  
	LCD_write_dat(year%10+0x30); 
	LCD_write_dat('-'); 
	LCD_write_dat(month/10+0x30);  
	LCD_write_dat(month%10+0x30); 
	LCD_write_dat('-');
	LCD_write_dat(date/10+0x30);  
	LCD_write_dat(date%10+0x30);
	LCD_write_dat(' ');
	LCD_write_dat(' ');
	
	LCD_Setaddress(4,1);
	print("ʱ��");
	LCD_write_dat(':');
	LCD_write_dat(' ');
	LCD_write_dat(hor/10+0x30);  
	LCD_write_dat(hor%10+0x30); 
	LCD_write_dat(':'); 
	LCD_write_dat(min/10+0x30);  
	LCD_write_dat(min%10+0x30); 
	LCD_write_dat(':');
	LCD_write_dat(sec/10+0x30);  
	LCD_write_dat(sec%10+0x30);
	LCD_write_dat(' ');
	LCD_write_dat(' ');
}

void ShowSetAlarmDat(void)
{
	LCD_clr();
	LCD_Setaddress(1,2);
	print("����������ֵ");
	LCD_Setaddress(2,1);
	print("PM2.5");
	LCD_write_dat(':');
	LCD_write_dat(Alpm25/100+0x30); 
   LCD_write_dat(Alpm25%100/10+0x30);  
   LCD_write_dat(Alpm25%10+0x30); 
	LCD_write_dat('u');
	LCD_write_dat('g');
	LCD_write_dat('/');
   LCD_write_dat('m');
   LCD_write_dat('3');
	
	LCD_Setaddress(3,1);
	print("�¶�");
   LCD_write_dat(':');
	LCD_write_dat(' ');
	LCD_write_dat('H');
	LCD_write_dat('=');
   LCD_write_dat(stwdh/10+0x30);  
   LCD_write_dat(stwdh%10+0x30); 
	LCD_write_dat(' ');
	LCD_write_dat('L');
	LCD_write_dat('=');
   LCD_write_dat(stwdl/10+0x30);  
   LCD_write_dat(stwdl%10+0x30); 
	LCD_write_dat(' ');
	
	LCD_Setaddress(4,1);
	print("ʪ��");
   LCD_write_dat(':');
	LCD_write_dat(' ');
	LCD_write_dat('H');
	LCD_write_dat('=');
   LCD_write_dat(stsdh/10+0x30);  
   LCD_write_dat(stsdh%10+0x30); 
	LCD_write_dat(' ');
	LCD_write_dat('L');
	LCD_write_dat('=');
   LCD_write_dat(stsdl/10+0x30);  
   LCD_write_dat(stsdl%10+0x30); 
	LCD_write_dat(' ');
}


void SetFun(void)
{
	unsigned int i=2000;
	if((k1==0)||(k2==0))
	{
		Delay(5000)	;
		if((k1==0)&&(k2==0))
		{
			write_data_ds1302(0x8e,0x00);
			write_data_ds1302(0x80,hexbcd(0)); //��DS1302��д��Ĵ���80Hд���ʼ������00
			write_data_ds1302(0x82,hexbcd(0));//��DS1302��д�ּĴ���82Hд���ʼ������00
			write_data_ds1302(0x84,hexbcd(8));//��DS1302��дСʱ�Ĵ���84Hд���ʼСʱ����12
			write_data_ds1302(0x86,hexbcd(11));//��DS1302��д���ڼĴ���86Hд���ʼ��������07
			write_data_ds1302(0x88,hexbcd(2));//��DS1302��д�·ݼĴ���88Hд���ʼ�·�����01
			write_data_ds1302(0x8c,hexbcd(17));//��DS1302��д��ݼĴ���8cHд���ʼ�������10
			write_data_ds1302(0x8e,0x80);	
			date=11;month=2;year=17;
			SetB=0;SetA=0;
			fmq=0;delay_ms(50);fmq=1;delay_ms(300);
			fmq=0;delay_ms(50);fmq=1;
		}
		while((k1==0)&&(k2==0));
		
	}
	if(k1==0)
	{
		Delay(5000)	;
		
		if(k1==0)
		{
			SetB=0;
			if(SetA<6) SetA++;
			else SetA=0;
			switch(SetA)
			{
				case 0:	LCD_clr();break;
				case 1:	
				case 2:	
				case 3:	
				case 4:	
				case 5:	
				case 6:	ShowSetTime();break;				
			}
		}
		while(k1==0);
	}
	
	if(k2==0)
	{
		Delay(5000)	;
		
		if(k2==0)
		{
			SetA=0;
			if(SetB<5) SetB++;
			else SetB=0;
			switch(SetB)
			{
				case 0:	LCD_draw_clr();break;
				case 1:	
				case 2:	
				case 3:	
				case 4:	
				case 5:	ShowSetAlarmDat();break;				
			}
		}
		while(k2==0);
	}
	
	if(SetA!=0)
	{
		if(k3==0)
		{
			Delay(5000)	;
			if(k3==0)
			{
				switch(SetA)
				{
					case 1:	sec++;if(sec>59) sec=0;break;
					case 2:  min++;if(min>59) min=0;break;
					case 3:  hor++;if(hor>23) hor=0;break;
					case 4:  
					{
						if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12)) 
						{
							date++; 
							if(date>31) date=1;
						}
						else if((month==4)||(month==6)||(month==9)||(month==11))
						{
							date++; 
							if(date>30) date=1;
						}
						else if(month==2)
						{
							i=2000+year;
							if((i%100)==0)
							{
								if(i%400==0)	{date++; if(date>29) date=1;}
								else {date++; if(date>28) date=1;}
							}
							else
							{
								if((i%4)==0)	{date++; if(date>29) date=1;}
								else {date++; if(date>28) date=1;}
							}
						}
						else {date++; if(date>31) date=1;}
					}break;
					case 5:  
					{
						month++;if(month>12) month=1;
						if(date>28)
						{
							if(month==2)
							{
								i=2000+year;
								if((i%100)==0)
								{
									if((i%400)==0)	date=29;
									else date=28;
								}
								else
								{
									if((i%4)==0)	 date=29;
									else date=28;
								}
							}
							if((month==4)||(month==6)||(month==9)||(month==11))
							{
								if(date>30) date=1;
							}
						}
					}break;
					case 6:  
					{
						year++;if(year>99) year=1;
						if(date>28)
						{
							if(month==2)
							{
								i=2000+year;
								if((i%100)==0)
								{
									if((i%400)==0)	date=29;
									else date=28;
								}
								else
								{
									if((i%4)==0)	 date=29;
									else date=28;
								}
							}
							if((month==4)||(month==6)||(month==9)||(month==11))
							{
								if(date>30) date=1;
							}
						}
					}break;
				}
			}
		}
		if(k4==0)
		{
			Delay(5000)	;
			if(k4==0)
			{
				switch(SetA)
				{
					case 1:	if(sec>0) sec--; break;
					case 2:  if(min>0) min--; break;
					case 3:  if(hor>0) hor--; break;
					case 4:  if(date>1) date--; break;
					case 5:  
					{
						if(month>1) month--; 
						if(date>28)
						{
							if(month==2)
							{
								i=2000+year;
								if((i%100)==0)
								{
									if((i%400)==0)	date=29;
									else date=28;
								}
								else
								{
									if((i%4)==0)	 date=29;
									else date=28;
								}
							}
							if((month==4)||(month==6)||(month==9)||(month==11))
							{
								if(date>30) date=1;
							}
						}
					}break;
					case 6:  
					{
						if(year>1) year--; 
						if(date>28)
						{
							if(month==2)
							{
								i=2000+year;
								if((i%100)==0)
								{
									if((i%400)==0)	date=29;
									else date=28;
								}
								else
								{
									if((i%4)==0)	 date=29;
									else date=28;
								}
							}
							if((month==4)||(month==6)||(month==9)||(month==11))
							{
								if(date>30) date=1;
							}
						}
					}break;
				}
			}
		}
		switch(SetA)
		{
			case 1:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x80, hexbcd(sec));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(4,7);
				LCD_write_dat(' ');  
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(4,7);
				LCD_write_dat(sec/10+0x30);  
				LCD_write_dat(sec%10+0x30);
				LCD_write_dat(' ');
				LCD_write_dat(' ');						
			}break;
			case 2:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x82, hexbcd(min));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(4,5);
				LCD_write_dat(':');
				LCD_write_dat(' ');  
				LCD_write_dat(' '); 
				delay_ms(10);
				LCD_Setaddress(4,5);
				LCD_write_dat(':');
				LCD_write_dat(min/10+0x30);  
				LCD_write_dat(min%10+0x30);
					
			}break;
			case 3:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x84, hexbcd(hor));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(4,4);
				LCD_write_dat(' ');  
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(4,4);
				LCD_write_dat(hor/10+0x30);  
				LCD_write_dat(hor%10+0x30);
					
			}break;
			case 4:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x86, hexbcd(date));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(3,7);
				LCD_write_dat(' ');  
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(3,7);
				LCD_write_dat(date/10+0x30);  
				LCD_write_dat(date%10+0x30);
				LCD_write_dat(' ');
				LCD_write_dat(' ');						
			}break;
			case 5:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x88, hexbcd(month));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(3,5);
				LCD_write_dat('-');
				LCD_write_dat(' ');  
				LCD_write_dat(' '); 
				delay_ms(10);
				LCD_Setaddress(3,5);
				LCD_write_dat('-');
				LCD_write_dat(month/10+0x30);  
				LCD_write_dat(month%10+0x30);
				if(month==2)
				{
					write_data_ds1302(0x8e,0x00); 
					write_data_ds1302(0x86, hexbcd(date));
					write_data_ds1302(0x8e,0x80);
					LCD_Setaddress(3,7);
					LCD_write_dat(date/10+0x30);  
					LCD_write_dat(date%10+0x30);	
				}
					
			}break;
			case 6:
			{
				write_data_ds1302(0x8e,0x00); 
				write_data_ds1302(0x8c, hexbcd(year));
				write_data_ds1302(0x8e,0x80);	
				LCD_Setaddress(3,4);
				LCD_write_dat(' ');  
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(3,4);
				LCD_write_dat(year/10+0x30);  
				LCD_write_dat(year%10+0x30);
				if(month==2)
				{
					write_data_ds1302(0x8e,0x00); 
					write_data_ds1302(0x86, hexbcd(date));
					write_data_ds1302(0x8e,0x80);
					LCD_Setaddress(3,7);
					LCD_write_dat(date/10+0x30);  
					LCD_write_dat(date%10+0x30);	
				}
					
			}break;
		}
		
	}
	
	/*************************************���������޲���*******************************/
	if(SetB!=0)
	{
		if(k3==0)
		{
			Delay(5000)	;
			if(k3==0)
			{
				switch(SetB)
				{
					case 1:	stsdl++;if(stsdl>90) stsdl=0;break;
					case 2:  stsdh++;if(stsdh>90) stsdh=0;break;
					case 3:  stwdl++;if(stwdl>60) stwdl=0;break;
					case 4:  stwdh++;if(stwdh>60) stwdh=0;break;
					case 5:  Alpm25++;if(Alpm25>990) Alpm25=20;break;
				}
			}
		}
		if(k4==0)
		{
			Delay(5000)	;
			if(k4==0)
			{
				switch(SetB)
				{
					case 1:	if(stsdl>0) stsdl--; break;
					case 2:  if(stsdh>0) stsdh--; break;
					case 3:  if(stwdl>0) stwdl--; break;
					case 4:  if(stwdh>0) stwdh--; break;
					case 5:  if(Alpm25>20) Alpm25--; break;
				}
			}
		}
		switch(SetB)
		{
			case 1:
			{
				LCD_Setaddress(4,7);
				LCD_write_dat('=');  
				LCD_write_dat(' ');
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(4,7);
				LCD_write_dat('='); 
				LCD_write_dat(stsdl/10+0x30);  
				LCD_write_dat(stsdl%10+0x30);
				LCD_write_dat(' ');
				LCD_write_dat(' ');						
			}break;
			case 2:
			{
				LCD_Setaddress(4,5);
				LCD_write_dat(' ');  
				LCD_write_dat(' '); 
				delay_ms(20);
				LCD_Setaddress(4,5);
				LCD_write_dat(stsdh/10+0x30);  
				LCD_write_dat(stsdh%10+0x30);
					
			}break;
			case 3:
			{	
				LCD_Setaddress(3,7);
				LCD_write_dat('=');  
				LCD_write_dat(' ');
				LCD_write_dat(' ');
				delay_ms(20);
				LCD_Setaddress(3,7);
				LCD_write_dat('='); 
				LCD_write_dat(stwdl/10+0x30);  
				LCD_write_dat(stwdl%10+0x30);
				LCD_write_dat(' ');
				LCD_write_dat(' ');			
					
			}break;
			case 4:
			{
				LCD_Setaddress(3,5);
				LCD_write_dat(' ');  
				LCD_write_dat(' '); 
				delay_ms(20);
				LCD_Setaddress(3,5);
				LCD_write_dat(stwdh/10+0x30);  
				LCD_write_dat(stwdh%10+0x30);				
			}break;
			case 5:
			{
				LCD_Setaddress(2,4);
				LCD_write_dat(' ');
				LCD_write_dat(' ');  
				LCD_write_dat(' '); 
				delay_ms(20);
				LCD_Setaddress(2,4);	
				LCD_write_dat(Alpm25/100+0x30); 
				LCD_write_dat(Alpm25%100/10+0x30);  
				LCD_write_dat(Alpm25%10+0x30); 
			}break;
		}
		
	}
	
	
	
	if(k5==0)
	{
		Delay(5000)	;
		
		if(k5==0)
		{
			SetB=0;SetA=0;
			LCD_clr();
		}	
	}	
	
	
}


//************************************������************************************************************
void main(void)
{	
	init_ds1302();
	LCD_init();
 	LCD_draw_clr();
	
	init_NRF24L01() ;
	delay_ms(1);
	Delay(6000);
	
	k1=1;
	k2=1;
	k3=1;
	k4=1;
	while(1)
	{
		if((SetA==0)&&(SetB==0))
		{
			flag++;
			du_ds1302() ;				//��ȡ1302ʱ��
			xianshishijian();			 //��ʾʱ��		 
			SetRX_Mode();				//����2401 ����ģʽ
			if(nRF24L01_RxPacket(RxBuf))	//��ȡ2401 ���ռĴ���
			{
				if(RxBuf[1]==0xaa)										  //���ݴ��� 
				{
					wendu=RxBuf[3];	 
										  
					shidu=RxBuf[5];	
					guangzhao=RxBuf[7]*10000+ RxBuf[8]*1000 +RxBuf[9]*100+RxBuf[10]*10+RxBuf[11];

					pm25= RxBuf[12]*100+RxBuf[13];
				}
				xianshiwsd();
				xianshipm();
				xianshiguangzhao();
				flag=0;
			}	

			/////����Ϊ���� �����ޱ�������
			if((pm25<995)&&(pm25>0))
			{
				if((wendu>stwdh )||(wendu<stwdl)||(shidu>stsdh)||(shidu<stsdl)||(pm25>Alpm25)	)	fmq=0;
				else	  fmq=1;
			}

			if(flag>=250)						 //����250δ���� ��ʾͨѶ����
			{
				LCD_draw_clr();
				LCD_Setaddress(2,1);

				print("ͨѶ�����С�����");
				flag=0;
			}
		}
		SetFun();	//���ò���
	}	
}

