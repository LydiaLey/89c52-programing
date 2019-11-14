#include<reg51.h>	
#define LCD1602_DATAPINS P0

typedef	 unsigned int	  uint;
typedef unsigned char	 uchar;

sbit LCD1602_E=P2^7;
sbit LCD1602_RW=P2^5;
sbit LCD1602_RS=P2^6;
sbit DSPORT=P3^7;

uchar CNCHAR[6] = "���϶�";

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
	LcdInit();			 //��ʼ��LCD1602
	LcdWriteCom(0x88);	//д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('C'); 
	EA=1;
	EX0=1;
	IT0=1;
	while(1);
	//{
	//	LcdDisplay(read_temper());

//		Delay1ms(1000);//1s��ˢһ��
//	}				
}

void DelayMs(unsigned int x)   //0.14ms��� 0us
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<13; i++)
 {}
 }
}

void LcdDisplay(int temp) 	 //lcd��ʾ
{
    
  	unsigned char i, datas[] = {0, 0, 0, 0, 0}; //��������
	float tp;  
	if(temp< 0)				//���¶�ֵΪ����
  	{
	  	LcdWriteCom(0x80);		//д��ַ 80��ʾ��ʼ��ַ
		SBUF='-';//�����յ������ݷ��뵽���ͼĴ���
		while(!TI);			         //�ȴ������������
		TI=0;						 //���������ɱ�־λ
	    LcdWriteData('-');  		//��ʾ��
		//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//����?.5��������С������档
 
  	}
 	else
  	{			
	  	LcdWriteCom(0x80);		//д��ַ 80��ʾ��ʼ��ַ
	    LcdWriteData('+'); 		//��ʾ��
		SBUF='+';//�����յ������ݷ��뵽���ͼĴ���
		while(!TI);			         //�ȴ������������
		TI=0;						 //���������ɱ�־λ
		tp=temp;//��Ϊ���ݴ�����С�������Խ��¶ȸ���һ�������ͱ���
		//����¶���������ô����ô������ԭ����ǲ���������
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//�����0.5��������С������档
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;

	LcdWriteCom(0x82);		  //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[0]); //��λ 
	SBUF = '0'+datas[0];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;
	
	LcdWriteCom(0x83);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[1]); //ʮλ
	SBUF = '0'+datas[1];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;

	LcdWriteCom(0x84);		//д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[2]); //��λ 
	SBUF = '0'+datas[2];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;

	LcdWriteCom(0x85);		//д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('.'); 		//��ʾ ��.��
	SBUF = '.';//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;

	LcdWriteCom(0x86);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[3]); //��ʾС����  
	SBUF = '0'+datas[3];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;

	LcdWriteCom(0x87);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[4]); //��ʾС���� 
	SBUF = '0'+datas[4];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;
	for(i=0; i<6; i++)
	{
	 	SBUF = CNCHAR[i];//�����յ������ݷ��뵽���ͼĴ���
		while (!TI);			         //�ȴ������������
		TI = 0;
	}

	 
}


void UsartConfiguration()
{
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X20;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XF3;				//��������ʼֵ���ã�ע�Ⲩ������4800��
	TL1=0XF3;
//	ES=1;						//�򿪽����ж�
//	EA=1;						//�����ж�
	TR1=1;					//�򿪼�����
}


/****************
�¶ȴ���������
****************/
uchar init()
{
	uchar i=0;
	DSPORT=0;
	i = 70;				   //����������480us~960us
	while(i--);//��ʱ642us
	DSPORT=1;				 //Ȼ���������ߣ����DS18B20������Ӧ�Ὣ��15us~60us����������
	i=0;
	while(DSPORT)		   //�ȴ�DS18B20��������
	{
		 DelayMs(1);
		 i++;		  
		 if(i>5)
		 {
		 	return 0;			 //��ʼ��ʧ��
		 }
	}
	return 1;
}

void writebyte(uchar datas)
{
	uchar i,j=1;
	for(i=0;i<8;i++)
	{
		DSPORT=0;		   //ÿд��һλ����֮ǰ�Ȱ���������1us
		//j++;		  //�˴���ʱ1us,������Ӱ�첻��
		DSPORT=datas&0x01;
		j=6;
		while(j--); //��ʱ68us������ʱ������60us
		DSPORT=1;		 //Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
		datas>>=1;
	}
}

uchar readbyte()
{
	uchar dat=0,temp;
	uint i ,j;
	for(i=0;i<8;i++)
	{
		DSPORT=0;	 //�Ƚ���������1us
		j++;
		DSPORT=1;	//Ȼ���ͷ�����
		j++;	  //�˴���ʱ�б仯���ȴ�6us
		j++;
		temp=DSPORT;	 //��ȡ���ݣ������λ��ʼ��ȡ
		/*��byte����һλ��Ȼ����������7λ���bi��ע���ƶ�֮���Ƶ���λ��0��*/
		dat=(dat>>1)|(temp<<7);
		j = 4;		//��ȡ��֮��ȴ�48us�ٽ��Ŷ�ȡ��һ����
		while(j--);;			 //����ûӰ��
	}
	return dat;		//����temp
}

void change_temper()
{
	init();
	DelayMs(1);			   //��Ӱ��
	writebyte(0xcc);		  //����ROM��������	
	writebyte(0x44);		   //�¶�ת������
}

void read_tempercom()
{
   	init();
	//DelayMs(1);
	writebyte(0xcc);
	writebyte(0xbe);		 //���Ͷ�ȡ�¶�����
}

uint read_temper()
{
	uchar tml,tmh;
	uint t=0;
	change_temper();			//��д��ת������
	read_tempercom();			//Ȼ��ȴ�ת������Ͷ�ȡ�¶�����
	tml=readbyte();				//��ȡ�¶�ֵ��16λ���ȶ����ֽ�
	tmh=readbyte();				//�ٶ����ֽ�
	t=tmh;
	t<<=8;			//��8λ
	t=tml|t;

	return t;
}
/********************
 LCDҺ������
********************/

void LcdWriteCom(uchar com)	  //д������
{
	LCD1602_E = 0;     //ʹ��
	LCD1602_RS = 0;	   //ѡ��������
	LCD1602_RW = 0;	   //ѡ��д��
	
	LCD1602_DATAPINS = com;     //��������
	DelayMs(1);		//�ȴ������ȶ�

	LCD1602_E = 1;	          //д��ʱ��
	DelayMs(5);	  //����ʱ��
	LCD1602_E = 0;
}

void LcdWriteData(uchar dat)			//д������
{
	LCD1602_E = 0;	//ʹ������
	LCD1602_RS = 1;	//ѡ����������
	LCD1602_RW = 0;	//ѡ��д��

	LCD1602_DATAPINS = dat; //д������
	DelayMs(1);

	LCD1602_E = 1;   //д��ʱ��
	DelayMs(5);   //����ʱ��
	LCD1602_E = 0;
}

void LcdInit()						  //LCD��ʼ���ӳ���
{
 	LcdWriteCom(0x38);  //����ʾ
	LcdWriteCom(0x0c);  //����ʾ����ʾ���
	LcdWriteCom(0x06);  //дһ��ָ���1
	LcdWriteCom(0x01);  //����
	LcdWriteCom(0x80);  //��������ָ�����
}
void int0() interrupt 0
{
	LcdDisplay(read_temper());
}