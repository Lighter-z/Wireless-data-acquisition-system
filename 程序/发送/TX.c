#include <stc89c52.h>
#include <intrins.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define FOSC 11059200L      //System frequency
#define BAUD 2400           //UART baudrate



unsigned char flagcom=0;

//****************************************2401IO�˿ڶ���***************************************
sbit 	MISO	=P0^2;
sbit 	MOSI	=P0^4;
sbit	SCK	    =P0^1;
sbit	CE	    =P0^0;
sbit	CSN		=P0^5;
sbit	IRQ		=P0^3;

unsigned char xdata TxBuf[32]=0;		//2401 ���ͼĴ���
 // 
sbit     DQ=P2^7;        //DHT11
//bh1750
sbit	  SCL=P2^1;      //IICʱ�����Ŷ���
sbit  	  SDA=P2^2;      //IIC�������Ŷ���

#define	  SlaveAddress   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0x46���ӵ�Դʱ��ַΪ0x3A
                              
                              

typedef   unsigned char BYTE;
typedef   unsigned short WORD;
uchar   BUF[8];                         //�������ݻ�����  
unsigned char  xdata    ge,shi,bai,qian,wan;            //��ʾ����




uchar tem;  //�¶�
uchar  hum;  //ʪ��
uint guangzhao=0;		//guangzhao
uint pm25=0;			//pm2.5
uint flag1=0;
sbit 	LED1=P1^1;
sbit 	LED2=P1^4;

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
uchar const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
uchar const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ
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
//*****************************************����ʱ*****************************************
void Delay(unsigned int s)
{
	unsigned int i;
	for(i=0; i<s; i++);
	for(i=0; i<s; i++);
}
//******************************************************************************************
uchar 	bdata sta;   //״̬��־
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
/******************************************************************************************
/*��ʱ����
/******************************************************************************************/
void inerDelay_us(unsigned char n)
{
	for(;n>0;n--)
		_nop_();
}


void Delay5ms()
{
    WORD n = 560;

    while (n--);
}

void delay_nms(unsigned int k)	
{						
unsigned int i,j;				
for(i=0;i<k;i++)
{			
for(j=0;j<121;j++)			
{;}}						
}

void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
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
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������

}
/****************************************************************************************************
/*������uint SPI_RW(uint uchar)
/*���ܣ�NRF24L01��SPIдʱ��
/****************************************************************************************************/
uint SPI_RW(unsigned char uchar)
{
	unsigned char  bit_ctr;
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
/*uchar SPI_Read(uchar reg)
{
	uchar reg_val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
}*/
/****************************************************************************************************/
/*���ܣ�NRF24L01��д�Ĵ�������
/****************************************************************************************************/
uint SPI_RW_Reg(uchar reg, uchar value)
{
	uchar status;
	
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
/*uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uchar status,uchar_ctr;
	
	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0);    // 
	
	CSN = 1;                           
	
	return(status);                    // return nRF24L01 status uchar
}*/
/*********************************************************************************************************
/*������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
/*********************************************************************************************************/
uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uchar status,uchar_ctr;
	
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
/*void SetRX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1; 
	inerDelay_us(130);
}*/
/******************************************************************************************************/
/*������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/
/*unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
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
}*/
/***********************************************************************************************************
/*������void nRF24L01_TxPacket(unsigned char * tx_buf)
/*���ܣ����� tx_buf������
/**********************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	CE=0;			//StandBy Iģʽ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������	   0e
	CE=1;		 //�ø�CE���������ݷ���
	inerDelay_us(10);
}

//**************************��ʱ����

void delay_ms(unsigned int k)	
{						
unsigned int i,j;				
for(i=0;i<k;i++)
{			
for(j=0;j<121;j++)			
{;}}						
}

//��ʼ��DTH11
 bit Init_DTH11()
 {
  bit flag;
  uchar num;
  DQ=0;
  delay_ms(20);   // >18ms
  DQ=1;
  for(num=0;num<10;num++);   // 20-40us    34.7us
  for(num=0;num<12;num++);
  flag=DQ;
  for(num=0;num<11;num++); //DTH��Ӧ80us
  for(num=0;num<24;num++);    //DTH����80us
  return flag;
 }
 //****************************��DTH11����
 uchar DTH11_RD_CHAR()
 {
  uchar byte=0;
  uchar num;
  uchar num1;
  while(DQ==1);
  for(num1=0;num1<8;num1++)
  {
   while(DQ==0);
   byte<<=1;     //��λ��ǰ
   for(num=0;DQ==1;num++);
   if(num<10)
    byte|=0x00;
   else
    byte|=0x01;
  }
  return byte;
 }
 //******************************��ȡDTH11�¶Ⱥ�ʪ��
 void  DTH11_DUSHU()
 {
  uchar num;
  if(Init_DTH11()==0)
  { 
  hum=DTH11_RD_CHAR();   //������ֵ��7������
   DTH11_RD_CHAR();
  tem=DTH11_RD_CHAR();
//	tem=30;
   DTH11_RD_CHAR();
   DTH11_RD_CHAR();
   for(num=0;num<17;num++);  //���BIT�������������50us
   DQ=1;
  }
 }

//********************************iic ����///////////////////////////////////////////
//********************************bh1750���մ���������*******************************************************
 /**************************************
��ʼ�ź�
**************************************/
void BH1750_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void BH1750_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 1;                    //����������
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
bit BH1750_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ

    return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void BH1750_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    BH1750_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
BYTE BH1750_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}

//*********************************

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //��ʼ�ź�
    BH1750_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    BH1750_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
  //  BH1750_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    BH1750_Stop();                   //����ֹͣ�ź�
}

//********���ֽڶ�ȡ*****************************************
/*
uchar Single_Read_BH1750(uchar REG_Address)
{  uchar REG_data;
    BH1750_Start();                          //��ʼ�ź�
    BH1750_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    BH1750_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    BH1750_Start();                          //��ʼ�ź�
    BH1750_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=BH1750_RecvByte();              //�����Ĵ�������
	BH1750_SendACK(1);   
	BH1750_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
*/
//*********************************************************
//
//��������BH1750�ڲ�����
//
//*********************************************************
void Multiple_Read_BH1750(void)
{   
  uchar i;	
  BH1750_Start();                          //��ʼ�ź�
  BH1750_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	
 for (i=0; i<3; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
  {
      BUF[i] = BH1750_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
      if (i == 3)
      {

         BH1750_SendACK(1);                //���һ��������Ҫ��NOACK
      }
      else
      {		
        BH1750_SendACK(0);                //��ӦACK
     }
 }

  BH1750_Stop();                          //ֹͣ�ź�
  Delay5ms();
}


//��ʼ��BH1750��������Ҫ��ο�pdf�����޸�****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  
}




void conversion(unsigned int temp_data)  //  ����ת���� ����ʮ���٣�ǧ����
{  
  wan=temp_data/10000 ;
  qian=temp_data%10000/1000;
  bai=temp_data%1000/100;
  shi= temp_data%100/10;
  ge=	temp_data%10;
}

//////////////////////���ڡ�������������������������/////////////////////////////////////////////////



void UartInit(void)		//2400bps@11.0592MHz
{
  SCON = 0x50;            //8-bit variable UART
  PCON=0X00;
  TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
  TH1 = TL1 = 0XF4; //Set auto-reload vaule
  TR1 = 1;                //Timer1 start run

  ES = 0;                 //Enable UART interrupt
  EA = 0;                 //Open master interrupt switch
}

  unsigned char dat[6];
  unsigned char  k=0;

void UART_SER (void) interrupt 4 
{
    if (RI)
    {
      RI = 0;             //Clear receive interrupt flag
    
      dat[k]= SBUF;          //P0 show UART data

      if(dat[0]!=0xaa) k=0;
      else k++;	   
      if((dat[0]==0xaa)&&(k>5))
      {	    
        pm25=dat[1]*255+dat[2];
        flagcom=1;
        ES=0;
        EA=0;
        k=0;
      }
    }
	  
    LED2=0;	//~LED2;
    Delay(50);
    LED2=1;

    if (TI)
    {
      TI = 0;             //Clear transmit interrupt flag
    }
}



//************************************������************************************************************
void main(void)
{
  float xdata guang=0;
	Delay(6000);
  TxBuf[1]=0xaa;

	DQ=1;
	LED1=0;
	Delay(20000);
	UartInit()	;

  Init_BH1750();       //��ʼ��BH1750

  pm25  =0;
  flagcom=1;
  Delay(200);
	while(1)
	{	
		 if(flagcom==1)
		 {
      Single_Write_BH1750(0x01);   // power on
      Single_Write_BH1750(0x10);   // H- resolution mode
      delay_nms(180);              //��ʱ180ms

      Multiple_Read_BH1750();       //�����������ݣ��洢��BUF��
      guangzhao=BUF[0];
      guangzhao=(guangzhao<<8)+BUF[1];//�ϳ����� 
      guang=(float)guangzhao/1.2;

      DTH11_DUSHU();
      Delay(2000);
      DTH11_DUSHU();

      Delay(20);
      DTH11_DUSHU();
      Delay(20);

      TxBuf[3]=tem;							 //��ʪ�� �������ݷ���Ĵ���
      TxBuf[5]=hum;

      conversion(guang);

      TxBuf[7]=wan;
      TxBuf[8]=qian;
      TxBuf[9]=bai;
      TxBuf[10]=shi;
      TxBuf[11]=ge;


      if(pm25>=999) pm25=999;			//pm25����
      TxBuf[12]=pm25/100;

      TxBuf[13]=pm25%100;
      Delay(20);

      LED1=0;
      init_NRF24L01() ;			//��ʼ��2401

      nRF24L01_TxPacket(TxBuf);	// ��������

      SPI_RW_Reg(WRITE_REG+STATUS,0XFf);//clear RX_DR or TX_DS or MAX_RT interrupt  
      Delay(20);

      LED1=1;
      Delay(20000);
      Delay(20000);
      Delay(20000);

      flagcom=0;

      EA=1;
      ES=1;

	   }

		}
	
}
