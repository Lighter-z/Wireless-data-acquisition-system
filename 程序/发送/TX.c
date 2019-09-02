#include <stc89c52.h>
#include <intrins.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define FOSC 11059200L      //System frequency
#define BAUD 2400           //UART baudrate



unsigned char flagcom=0;

//****************************************2401IO端口定义***************************************
sbit 	MISO	=P0^2;
sbit 	MOSI	=P0^4;
sbit	SCK	    =P0^1;
sbit	CE	    =P0^0;
sbit	CSN		=P0^5;
sbit	IRQ		=P0^3;

unsigned char xdata TxBuf[32]=0;		//2401 发送寄存器
 // 
sbit     DQ=P2^7;        //DHT11
//bh1750
sbit	  SCL=P2^1;      //IIC时钟引脚定义
sbit  	  SDA=P2^2;      //IIC数据引脚定义

#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0x3A
                              
                              

typedef   unsigned char BYTE;
typedef   unsigned short WORD;
uchar   BUF[8];                         //接收数据缓存区  
unsigned char  xdata    ge,shi,bai,qian,wan;            //显示变量




uchar tem;  //温度
uchar  hum;  //湿度
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
uchar const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
uchar const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//接收地址
//***************************************NRF24L01寄存器指令*******************************************************
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
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
//*****************************************长延时*****************************************
void Delay(unsigned int s)
{
	unsigned int i;
	for(i=0; i<s; i++);
	for(i=0; i<s; i++);
}
//******************************************************************************************
uchar 	bdata sta;   //状态标志
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
/******************************************************************************************
/*延时函数
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
/*NRF24L01初始化
//***************************************************************************************/
void init_NRF24L01(void)
{
    inerDelay_us(100);
 	CE=0;    // chip enable
 	CSN=1;   // Spi disable 
 	SCK=0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   设置信道工作为2.4GHZ，收发必须一致
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送

}
/****************************************************************************************************
/*函数：uint SPI_RW(uint uchar)
/*功能：NRF24L01的SPI写时序
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
/*函数：uchar SPI_Read(uchar reg)
/*功能：NRF24L01的SPI时序
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
/*功能：NRF24L01读写寄存器函数
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
/*函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
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
/*函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
/*********************************************************************************************************/
uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uchar status,uchar_ctr;
	
	CSN = 0;            //SPI使能       
	status = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //关闭SPI
	return(status);    // 
}
/****************************************************************************************************/
/*函数：void SetRX_Mode(void)
/*功能：数据接收配置 
/****************************************************************************************************/
/*void SetRX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ收发完成中断响应，16位CRC	，主接收
	CE = 1; 
	inerDelay_us(130);
}*/
/******************************************************************************************************/
/*函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*功能：数据读取后放如rx_buf接收缓冲区中
/******************************************************************************************************/
/*unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
    unsigned char revale=0;
	sta=SPI_Read(STATUS);	// 读取状态寄存其来判断数据接收状况
	if(RX_DR)				// 判断是否接收到数据
	{
	    CE = 0; 			//SPI使能
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;			//读取数据完成标志
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	return revale;
}*/
/***********************************************************************************************************
/*函数：void nRF24L01_TxPacket(unsigned char * tx_buf)
/*功能：发送 tx_buf中数据
/**********************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	CE=0;			//StandBy I模式	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送	   0e
	CE=1;		 //置高CE，激发数据发送
	inerDelay_us(10);
}

//**************************延时函数

void delay_ms(unsigned int k)	
{						
unsigned int i,j;				
for(i=0;i<k;i++)
{			
for(j=0;j<121;j++)			
{;}}						
}

//初始化DTH11
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
  for(num=0;num<11;num++); //DTH响应80us
  for(num=0;num<24;num++);    //DTH拉高80us
  return flag;
 }
 //****************************读DTH11数据
 uchar DTH11_RD_CHAR()
 {
  uchar byte=0;
  uchar num;
  uchar num1;
  while(DQ==1);
  for(num1=0;num1<8;num1++)
  {
   while(DQ==0);
   byte<<=1;     //高位在前
   for(num=0;DQ==1;num++);
   if(num<10)
    byte|=0x00;
   else
    byte|=0x01;
  }
  return byte;
 }
 //******************************读取DTH11温度和湿度
 void  DTH11_DUSHU()
 {
  uchar num;
  if(Init_DTH11()==0)
  { 
  hum=DTH11_RD_CHAR();   //比正常值高7度左右
   DTH11_RD_CHAR();
  tem=DTH11_RD_CHAR();
//	tem=30;
   DTH11_RD_CHAR();
   DTH11_RD_CHAR();
   for(num=0;num<17;num++);  //最后BIT输出后拉低总线50us
   DQ=1;
  }
 }

//********************************iic 光照///////////////////////////////////////////
//********************************bh1750光照传感器驱动*******************************************************
 /**************************************
起始信号
**************************************/
void BH1750_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void BH1750_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit BH1750_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    BH1750_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE BH1750_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//*********************************

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
  //  BH1750_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    BH1750_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
/*
uchar Single_Read_BH1750(uchar REG_Address)
{  uchar REG_data;
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    BH1750_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BH1750_RecvByte();              //读出寄存器数据
	BH1750_SendACK(1);   
	BH1750_Stop();                           //停止信号
    return REG_data; 
}
*/
//*********************************************************
//
//连续读出BH1750内部数据
//
//*********************************************************
void Multiple_Read_BH1750(void)
{   
  uchar i;	
  BH1750_Start();                          //起始信号
  BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	
 for (i=0; i<3; i++)                      //连续读取6个地址数据，存储中BUF
  {
      BUF[i] = BH1750_RecvByte();          //BUF[0]存储0x32地址中的数据
      if (i == 3)
      {

         BH1750_SendACK(1);                //最后一个数据需要回NOACK
      }
      else
      {		
        BH1750_SendACK(0);                //回应ACK
     }
 }

  BH1750_Stop();                          //停止信号
  Delay5ms();
}


//初始化BH1750，根据需要请参考pdf进行修改****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  
}




void conversion(unsigned int temp_data)  //  数据转换出 个，十，百，千，万
{  
  wan=temp_data/10000 ;
  qian=temp_data%10000/1000;
  bai=temp_data%1000/100;
  shi= temp_data%100/10;
  ge=	temp_data%10;
}

//////////////////////串口、、、、、、、、、、、、、/////////////////////////////////////////////////



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



//************************************主函数************************************************************
void main(void)
{
  float xdata guang=0;
	Delay(6000);
  TxBuf[1]=0xaa;

	DQ=1;
	LED1=0;
	Delay(20000);
	UartInit()	;

  Init_BH1750();       //初始化BH1750

  pm25  =0;
  flagcom=1;
  Delay(200);
	while(1)
	{	
		 if(flagcom==1)
		 {
      Single_Write_BH1750(0x01);   // power on
      Single_Write_BH1750(0x10);   // H- resolution mode
      delay_nms(180);              //延时180ms

      Multiple_Read_BH1750();       //连续读出数据，存储在BUF中
      guangzhao=BUF[0];
      guangzhao=(guangzhao<<8)+BUF[1];//合成数据 
      guang=(float)guangzhao/1.2;

      DTH11_DUSHU();
      Delay(2000);
      DTH11_DUSHU();

      Delay(20);
      DTH11_DUSHU();
      Delay(20);

      TxBuf[3]=tem;							 //温湿度 光照数据放入寄存器
      TxBuf[5]=hum;

      conversion(guang);

      TxBuf[7]=wan;
      TxBuf[8]=qian;
      TxBuf[9]=bai;
      TxBuf[10]=shi;
      TxBuf[11]=ge;


      if(pm25>=999) pm25=999;			//pm25处理
      TxBuf[12]=pm25/100;

      TxBuf[13]=pm25%100;
      Delay(20);

      LED1=0;
      init_NRF24L01() ;			//初始化2401

      nRF24L01_TxPacket(TxBuf);	// 发送数据

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
