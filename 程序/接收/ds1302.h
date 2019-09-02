

sbit ds1302_RST=P1^2;
sbit ds1302_IO=P1^1;
sbit ds1302_SCLK=P1^0;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
  
unsigned char  bcd16(unsigned char ucData)
{
unsigned char dat1,dat2;
dat1=ucData;
dat2=dat1/16;
dat1=dat1%16;
ucData=dat1+dat2*10;
return ucData;
}

unsigned char  hexbcd(unsigned char ucData)
{
  unsigned char dat1;
	dat1=ucData;
	ucData=((dat1/10)*16)+(dat1%10);
	return ucData;
}




/*ds1302模块部分*/

/******************************************************************** 
*函 数 名：read_Byte()
*功能描述：实时时钟读一字节
*函数说明：读DS13021Byte数据   下降沿输出一位数据
*调用函数：
*全局变量：
*输    入：ds1302_SCLK=0;
*返    回：ACC  

***********************************************************************/
uchar  read_Byte()
{
   uchar i;
   uchar j;
	//ds1302_RST=1;
   for(i=8;i>0;i--)
   {
     j=j>>1;
    if(ds1302_IO)     //由于ds1302读数据的时候，第一个数据读取在发一个Byte命令后，在第八位的下降沿
	{
	  j|=0x80;
	}
	else
	{
	  j&=0x7f;
	}
     ds1302_SCLK=1;
     ds1302_SCLK=0;//产生下降沿输出一位数据
   }
   return j;
}


/******************************************************************** 
*函 数 名：write_Byte()
*功能描述：实时时钟写一字节
*函数说明：往DS1302写入1Byte数据  上升沿输入数据
*调用函数：
*全局变量：
*输    入：ds1302_SCLK=0;tdata
*返    回： 

***********************************************************************/
void  write_Byte(uchar tdata)
{
	uchar i;
   ACC=tdata;
	//ds1302_RST=1;
   for(i=8;i>0;i--)
   {
		ds1302_IO=ACC0;
		ds1302_SCLK=1;
		ds1302_SCLK=0;//产生上升沿输入数据
		
		ACC=ACC>>1;
   }

}


/******************************************************************** 
*函 数 名：write_data_ds1302(uchar taddr,uchar tdata)
*功能描述：实时时钟写数据
*函数说明：先写地址,后写命令/数据
*调用函数：write_Byte(uchar tdata)
*全局变量：
*输    入：taddr;tdata
*返    回： 

***********************************************************************/

void write_data_ds1302(uchar taddr,uchar tdata)
{
   ds1302_RST=0;
   ds1302_SCLK=0;
   ds1302_RST=1;
   write_Byte(taddr);
   write_Byte(tdata); 
   ds1302_RST=0;
   ds1302_SCLK=1;
}

/******************************************************************** 
*函 数 名：read_data_ds1302(uchar taddr,uchar tdata)
*功能描述：读实时时钟地址里的数据
*函数说明：先写地址,后读命令/数据
*调用函数：write_Byte(uchar tdata)；read_Byte()
*全局变量：
*输    入：taddr;
*返    回：tdata 

***********************************************************************/
uchar read_data_ds1302(uchar taddr)
{  
   uchar tdata;
    
   ds1302_RST=0;
   ds1302_SCLK=0;
   ds1302_RST=1;
   write_Byte(taddr);
   tdata=read_Byte(); 
   ds1302_RST=0;
   ds1302_SCLK=1;
   return(tdata);
}


/******************************************************************** 
*函 数 名：set_ds1302()
*功能描述：设置初始时间
*函数说明：先写地址,后读命令/数据
*调用函数：
*全局变量：
*输    入：taddr;
*返    回：tdata 

***********************************************************************/

/*void set_ds1302(uchar *P1302)
{
    uchar i;
    uchar taddr = 0x80; 
    write_data_ds1302(0x8e,0x00);           // 控制命令,WP=0,写操作
    for(i =3; i>0; i--)
    { 
       write_data_ds1302(taddr,*P1302);    // 秒 分 时 日 月 星期 年  
        P1302++;
        taddr+=2;
    }
   write_data_ds1302(0x8e,0x80);           // 控制命令,WP=1,写保护
}*/




/******************************************************************** 
*函 数 名：init_ds1302()
*功能描述：初始化ds1302
*函数说明：先写地址,后读命令/数据
*调用函数：
*全局变量：
*输    入：
*返    回：

***********************************************************************/

void init_ds1302()
{
    ds1302_RST=0;
    ds1302_SCLK=0;
    //write_data_ds1302(0x8e,0x00);
	 //write_data_ds1302(0x80,0x00); //向DS1302内写秒寄存器80H写入初始秒数据00
	//write_data_ds1302(0x82,0x00);//向DS1302内写分寄存器82H写入初始分数据00
	//write_data_ds1302(0x84,0x08);//向DS1302内写小时寄存器84H写入初始小时数据12
	//write_data_ds1302(0x8a,0x04);//向DS1302内写周寄存器8aH写入初始周数据4
	//write_data_ds1302(0x86,0x07);//向DS1302内写日期寄存器86H写入初始日期数据07
	//write_data_ds1302(0x88,0x01);//向DS1302内写月份寄存器88H写入初始月份数据01
	//write_data_ds1302(0x8c,0x10);//向DS1302内写年份寄存器8cH写入初始年份数据10
	 write_data_ds1302(0x8e,0x80);
  //  write_data_ds1302(0x90,0xa5);//打开充电二级管  一个二级管串联一个2K电阻 
}