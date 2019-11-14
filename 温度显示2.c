#include<reg51.h>	
#define LCD1602_DATAPINS P0

typedef	 unsigned int	  uint;
typedef unsigned char	 uchar;

sbit LCD1602_E=P2^7;
sbit LCD1602_RW=P2^5;
sbit LCD1602_RS=P2^6;
sbit DSPORT=P3^7;

uchar CNCHAR[6] = "摄氏度";

void LcdInit();
void LcdWriteData(uchar dat);
void LcdWriteCom(uchar com);
void LcdDisplay(int);
void UsartConfiguration();
uchar init();
void writebyte(uchar datas);
void change_temper();
void read_tempercom();
uint read_temper();
uchar readbyte();
void DelayMs(unsigned int x);

void main(void)	
{
	UsartConfiguration();
	LcdInit();			 //初始化LCD1602
	LcdWriteCom(0x88);	//写地址 80表示初始地址
	LcdWriteData('C'); 
	EA=1;
	EX0=1;
	IT0=1;
	while(1);
	//{
	//	LcdDisplay(read_temper());

//		Delay1ms(1000);//1s钟刷一次
//	}				
}

void DelayMs(unsigned int x)   //0.14ms误差 0us
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<13; i++)
 {}
 }
}

void LcdDisplay(int temp) 	 //lcd显示
{
    
  	unsigned char i, datas[] = {0, 0, 0, 0, 0}; //定义数组
	float tp;  
	if(temp< 0)				//当温度值为负数
  	{
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
		SBUF='-';//将接收到的数据放入到发送寄存器
		while(!TI);			         //等待发送数据完成
		TI=0;						 //清除发送完成标志位
	    LcdWriteData('-');  		//显示负
		//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算由?.5，还是在小数点后面。
 
  	}
 	else
  	{			
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
	    LcdWriteData('+'); 		//显示正
		SBUF='+';//将接收到的数据放入到发送寄存器
		while(!TI);			         //等待发送数据完成
		TI=0;						 //清除发送完成标志位
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		//如果温度是正的那么，那么正数的原码就是补码它本身
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算加上0.5，还是在小数点后面。
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;

	LcdWriteCom(0x82);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[0]); //百位 
	SBUF = '0'+datas[0];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	LcdWriteCom(0x83);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[1]); //十位
	SBUF = '0'+datas[1];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x84);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[2]); //个位 
	SBUF = '0'+datas[2];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x85);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’
	SBUF = '.';//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x86);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[3]); //显示小数点  
	SBUF = '0'+datas[3];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x87);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[4]); //显示小数点 
	SBUF = '0'+datas[4];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	for(i=0; i<6; i++)
	{
	 	SBUF = CNCHAR[i];//将接收到的数据放入到发送寄存器
		while (!TI);			         //等待发送数据完成
		TI = 0;
	}

	 
}


void UsartConfiguration()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF3;				//计数器初始值设置，注意波特率是4800的
	TL1=0XF3;
//	ES=1;						//打开接收中断
//	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}


/****************
温度传感器部分
****************/
uchar init()
{
	uchar i=0;
	DSPORT=0;
	i = 70;				   //将总线拉低480us~960us
	while(i--);//延时642us
	DSPORT=1;				 //然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
	i=0;
	while(DSPORT)		   //等待DS18B20拉低总线
	{
		 DelayMs(1);
		 i++;		  
		 if(i>5)
		 {
		 	return 0;			 //初始化失败
		 }
	}
	return 1;
}

void writebyte(uchar datas)
{
	uchar i,j=1;
	for(i=0;i<8;i++)
	{
		DSPORT=0;		   //每写入一位数据之前先把总线拉低1us
		//j++;		  //此处延时1us,好像又影响不大
		DSPORT=datas&0x01;
		j=6;
		while(j--); //延时68us，持续时间最少60us
		DSPORT=1;		 //然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		datas>>=1;
	}
}

uchar readbyte()
{
	uchar dat=0,temp;
	uint i ,j;
	for(i=0;i<8;i++)
	{
		DSPORT=0;	 //先将总线拉低1us
		j++;
		DSPORT=1;	//然后释放总线
		j++;	  //此处延时有变化，等待6us
		j++;
		temp=DSPORT;	 //读取数据，从最低位开始读取
		/*将byte左移一位，然后与上右移7位后的bi，注意移动之后移掉那位补0。*/
		dat=(dat>>1)|(temp<<7);
		j = 4;		//读取完之后等待48us再接着读取下一个数
		while(j--);;			 //好像没影响
	}
	return dat;		//不是temp
}

void change_temper()
{
	init();
	DelayMs(1);			   //无影响
	writebyte(0xcc);		  //跳过ROM操作命令	
	writebyte(0x44);		   //温度转换命令
}

void read_tempercom()
{
   	init();
	//DelayMs(1);
	writebyte(0xcc);
	writebyte(0xbe);		 //发送读取温度命令
}

uint read_temper()
{
	uchar tml,tmh;
	uint t=0;
	change_temper();			//先写入转换命令
	read_tempercom();			//然后等待转换完后发送读取温度命令
	tml=readbyte();				//读取温度值共16位，先读低字节
	tmh=readbyte();				//再读高字节
	t=tmh;
	t<<=8;			//移8位
	t=tml|t;

	return t;
}
/********************
 LCD液晶部分
********************/

void LcdWriteCom(uchar com)	  //写入命令
{
	LCD1602_E = 0;     //使能
	LCD1602_RS = 0;	   //选择发送命令
	LCD1602_RW = 0;	   //选择写入
	
	LCD1602_DATAPINS = com;     //放入命令
	DelayMs(1);		//等待数据稳定

	LCD1602_E = 1;	          //写入时序
	DelayMs(5);	  //保持时间
	LCD1602_E = 0;
}

void LcdWriteData(uchar dat)			//写入数据
{
	LCD1602_E = 0;	//使能清零
	LCD1602_RS = 1;	//选择输入数据
	LCD1602_RW = 0;	//选择写入

	LCD1602_DATAPINS = dat; //写入数据
	DelayMs(1);

	LCD1602_E = 1;   //写入时序
	DelayMs(5);   //保持时间
	LCD1602_E = 0;
}

void LcdInit()						  //LCD初始化子程序
{
 	LcdWriteCom(0x38);  //开显示
	LcdWriteCom(0x0c);  //开显示不显示光标
	LcdWriteCom(0x06);  //写一个指针加1
	LcdWriteCom(0x01);  //清屏
	LcdWriteCom(0x80);  //设置数据指针起点
}
void int0() interrupt 0
{
	LcdDisplay(read_temper());
}