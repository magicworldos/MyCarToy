#include <timer.h>
#include <led.h>
#include <nrf.h>

uint32_t recv_buff[NRF_BUFF_SIZE] = { 0 };

int p_head = 0;
int p_tail = 0;

u8 RX_BUF[RX_PLOAD_WIDTH]; //接收数据缓存
u8 TX_BUF[TX_PLOAD_WIDTH]; //发射数据缓存
u8 TX_ADDRESS[TX_ADR_WIDTH] = { 0x34, 0x43, 0x10, 0x10, 0x01 }; // 定义一个静态发送地址
u8 RX_ADDRESS[RX_ADR_WIDTH] = { 0x34, 0x43, 0x10, 0x10, 0x01 };

void nrf_buff_append(uint32_t dat)
{
	recv_buff[p_head] = dat;
	p_head++;
	p_head %= NRF_BUFF_SIZE;
	if (p_head == p_tail)
	{
		p_tail++;
		p_tail %= NRF_BUFF_SIZE;
	}
}

int nrf_buff_size(void)
{
	int size = p_head - p_tail;
	if (size < 0)
	{
		size += NRF_BUFF_SIZE;
	}
	return size;
}

int nrf_buff_get(uint32_t *buff)
{
	int s = nrf_buff_size();
	if (s > 0)
	{
		for (int i = 0; i < s; i++)
		{
			buff[i] = recv_buff[p_tail];
			p_tail++;
			p_tail %= NRF_BUFF_SIZE;
		}
	}
	return s;
}

void nrf_exti(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);

	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：SPI_NRF_Init
 * 描述  ：SPI的 I/O配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void nrf_init(void)
{
#ifndef __MASTER
	nrf_exti();
#endif

	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	NRF_CSN_HIGH();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
}

void nrf_send_buff(uint32_t *buff, int length)
{
	for (int i = 0; i < length; i++)
	{
		NRF_Tx_Dat(&buff[i]);
	}
}

void nrf_recv_buff(uint32_t *buff, int *length)
{

}
/*
 * 函数名：SPI_NRF_RW
 * 描述  ：用于向NRF读/写一字节数据
 * 输入  ：写入的数据
 * 输出  ：读取得的数据
 * 调用  ：内部调用
 */
u8 SPI_NRF_RW(u8 dat)
{
	/* 当 SPI发送缓冲器非空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
	}
	/* 通过 SPI2发送一字节数据 */
	SPI_I2S_SendData(SPI1, dat);

	/* 当SPI接收缓冲器为空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
	}
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}

/*
 * 函数名：SPI_NRF_WriteReg
 * 描述  ：用于向NRF特定的寄存器写入数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 dat:将要向寄存器写入的数据
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：内部调用
 */
u8 SPI_NRF_WriteReg(u8 reg, u8 dat)
{
	u8 status;
	NRF_CE_LOW();
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();

	/*发送命令及寄存器号 */
	status = SPI_NRF_RW(reg);

	/*向寄存器写入数据*/
	SPI_NRF_RW(dat);

	/*CSN拉高，完成*/
	NRF_CSN_HIGH();

	/*返回状态寄存器的值*/
	return (status);
}

/*
 * 函数名：SPI_NRF_ReadReg
 * 描述  ：用于从NRF特定的寄存器读出数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 * 输出  ：寄存器中的数据
 * 调用  ：内部调用
 */
u8 SPI_NRF_ReadReg(u8 reg)
{
	u8 reg_val;

	NRF_CE_LOW();
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();

	/*发送寄存器号*/
	SPI_NRF_RW(reg);

	/*读取寄存器的值 */
	reg_val = SPI_NRF_RW(NOP);

	/*CSN拉高，完成*/
	NRF_CSN_HIGH();

	return reg_val;
}

/*
 * 函数名：SPI_NRF_ReadBuf
 * 描述  ：用于从NRF的寄存器中读出一串数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 pBuf：用于存储将被读出的寄存器数据的数组，外部定义
 bytes: pBuf的数据长度
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：外部调用
 */
u8 SPI_NRF_ReadBuf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status, byte_cnt;

	NRF_CE_LOW();
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();

	/*发送寄存器号*/
	status = SPI_NRF_RW(reg);

	/*读取缓冲区数据*/
	for (byte_cnt = 0; byte_cnt < bytes; byte_cnt++)
	{
		pBuf[byte_cnt] = SPI_NRF_RW(NOP); //从NRF24L01读取数据
	}

	/*CSN拉高，完成*/
	NRF_CSN_HIGH();

	return status; //返回寄存器状态值
}

/*
 * 函数名：SPI_NRF_WriteBuf
 * 描述  ：用于向NRF的寄存器中写入一串数据
 * 输入  ：reg:NRF的命令+寄存器地址。
 pBuf：存储了将要写入写寄存器数据的数组，外部定义
 bytes: pBuf的数据长度
 * 输出  ：NRF的status寄存器的状态
 * 调用  ：外部调用
 */
u8 SPI_NRF_WriteBuf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status, byte_cnt;
	NRF_CE_LOW();
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();

	/*发送寄存器号*/
	status = SPI_NRF_RW(reg);

	/*向缓冲区写入数据*/
	for (byte_cnt = 0; byte_cnt < bytes; byte_cnt++)
	{
		SPI_NRF_RW(*pBuf++); //写数据到缓冲区
	}

	/*CSN拉高，完成*/
	NRF_CSN_HIGH();

	return (status); //返回NRF24L01的状态
}

/*
 * 函数名：NRF_RX_Mode
 * 描述  ：配置并进入接收模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();

	SPI_NRF_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //写RX节点地址

	SPI_NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01); //使能通道0的自动应答

	SPI_NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01); //使能通道0的接收地址

	SPI_NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL); //设置RF通信频率

	SPI_NRF_WriteReg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //选择通道0的有效数据宽度

	SPI_NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启

	SPI_NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0f); //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式

	/*CE拉高，进入接收模式*/
	NRF_CE_HIGH();
}

/*
 * 函数名：NRF_TX_Mode
 * 描述  ：配置发送模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void NRF_TX_Mode(void)
{
	NRF_CE_LOW();

	SPI_NRF_WriteBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); //写TX节点地址

	SPI_NRF_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK

	SPI_NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01); //使能通道0的自动应答

	SPI_NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01); //使能通道0的接收地址

	SPI_NRF_WriteReg(NRF_WRITE_REG + SETUP_RETR, 0x1a); //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次

	SPI_NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL); //设置RF通道为CHANAL

	SPI_NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启

	SPI_NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0e); //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断

	/*CE拉高，进入发送模式*/
	NRF_CE_HIGH();
	//CE要拉高一段时间才进入发送模式
	timer_delay_ms(1);
}

/*
 * 函数名：NRF_Check
 * 描述  ：主要用于NRF与MCU是否正常连接
 * 输入  ：无	
 * 输出  ：SUCCESS/ERROR 连接正常/连接失败
 * 调用  ：外部调用
 */
u8 NRF_Check(void)
{
	u8 buf[5] = { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 };
	u8 buf1[5];
	u8 i;

	/*写入5个字节的地址.  */
	SPI_NRF_WriteBuf(NRF_WRITE_REG + TX_ADDR, buf, 5);

	/*读出写入的地址 */
	SPI_NRF_ReadBuf(TX_ADDR, buf1, 5);

	/*比较*/
	for (i = 0; i < 5; i++)
	{
		if (buf1[i] != 0xC2)
		{
			break;
		}
	}

	if (i == 5)
	{
		return SUCCESS; //MCU与NRF成功连接
	}
	return ERROR; //MCU与NRF不正常连接
}

/*
 * 函数名：NRF_Tx_Dat
 * 描述  ：用于向NRF的发送缓冲区中写入数据
 * 输入  ：txBuf：存储了将要发送的数据的数组，外部定义	
 * 输出  ：发送结果，成功返回TXDS,失败返回MAXRT或ERROR
 * 调用  ：外部调用
 */
u8 NRF_Tx_Dat(u8 *txbuf)
{
	u8 state;

	/*ce为低，进入待机模式1*/
	NRF_CE_LOW();

	/*写数据到TX BUF 最大 32个字节*/
	SPI_NRF_WriteBuf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);

	/*CE为高，txbuf非空，发送数据包 */
	NRF_CE_HIGH();

	/*等待发送完成中断 */
	while (NRF_Read_IRQ() != 0)
	{
	}

	/*读取状态寄存器的值 */
	state = SPI_NRF_ReadReg(STATUS);

	/*清除TX_DS或MAX_RT中断标志*/
	SPI_NRF_WriteReg(NRF_WRITE_REG + STATUS, state);

	SPI_NRF_WriteReg(FLUSH_TX, NOP); //清除TX FIFO寄存器

	/*判断中断类型*/
	if (state & MAX_RT) //达到最大重发次数
	{
		return MAX_RT;
	}
	else if (state & TX_DS) //发送完成
	{
		return TX_DS;
	}
	return ERROR; //其他原因发送失败
}

/*
 * 函数名：NRF_Rx_Dat
 * 描述  ：用于从NRF的接收缓冲区中读出数据
 * 输入  ：rxBuf：用于接收该数据的数组，外部定义	
 * 输出  ：接收结果，
 * 调用  ：外部调用
 */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state;
	NRF_CE_HIGH(); //进入接收状态
	/*等待接收中断*/
	while (NRF_Read_IRQ() != 0)
	{
	}

	NRF_CE_LOW(); //进入待机状态
	/*读取status寄存器的值  */
	state = SPI_NRF_ReadReg(STATUS);

	/* 清除中断标志*/
	SPI_NRF_WriteReg(NRF_WRITE_REG + STATUS, state);

	/*判断是否接收到数据*/
	if (state & RX_DR) //接收到数据
	{
		SPI_NRF_ReadBuf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); //读取数据
		SPI_NRF_WriteReg(FLUSH_RX, NOP); //清除RX FIFO寄存器
		return RX_DR;
	}
	return ERROR; //没收到任何数据
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		//IRQ引脚低电平有效
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == RESET)
		{
#ifndef __MASTER
			NRF_CE_LOW();
			u8 state = SPI_NRF_ReadReg(STATUS);
			SPI_NRF_WriteReg(NRF_WRITE_REG + STATUS, state);
			if (state & RX_DR)
			{
				uint32_t dat = 0;
				SPI_NRF_ReadBuf(RD_RX_PLOAD, &dat, RX_PLOAD_WIDTH);
				SPI_NRF_WriteReg(FLUSH_RX, NOP);
				nrf_buff_append(dat);
			}
#else
#endif
		}

	}
#ifndef __MASTER
	NRF_CE_LOW();
	SPI_NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0f);
	NRF_CE_HIGH();
#else
#endif
	EXTI_ClearITPendingBit(EXTI_Line5);
}
