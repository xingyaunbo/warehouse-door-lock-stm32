// Mifare RC522 RFID Card reader 13.56 MHz
// STM32F103 RFID RC522 SPI1 / UART / USB / Keil HAL / 2017 vk.com/zz555

// PA0  - (OUT)	LED2
// PA1	- (IN)	BTN1
// PA4  - (OUT)	SPI1_NSS (Soft)
// PA5  - (OUT)	SPI1_SCK
// PA6  - (IN)	SPI1_MISO (Master In)
// PA7  - (OUT)	SPI1_MOSI (Master Out)
// PA9	- (OUT)	TX UART1 (RX-RS232)
// PA10	- (IN)	RX UART1 (TX-RS232)
// PA11 - (OUT) USB_DM
// PA12 - (OUT) USB_DP
// PA13 - (IN) 	SWDIO
// PA14 - (IN) 	SWDCLK
// PC13 - (OUT)	LED1

// MFRC522		STM32F103		DESCRIPTION
// CS (SDA)		    PA4					SPI1_NSS	Chip select for SPI
// SCK				PA5					SPI1_SCK	Serial Clock for SPI
// MOSI				PA7 				SPI1_MOSI	Master In Slave Out for SPI
// MISO				PA6					SPI1_MISO	Master Out Slave In for SPI
// IRQ				-						Irq
// GND				GND					Ground
// RST				3.3V				Reset pin (3.3V)
// VCC				3.3V				3.3V power

#include "string.h"
#include "rc522.h"


// RC522
extern unsigned char  MFRC522_Check(unsigned char * id);
extern unsigned char  MFRC522_Compare(unsigned char * CardID, unsigned char * CompareID);
extern void MFRC522_WriteRegister(unsigned char  addr, unsigned char  val);
extern unsigned char  MFRC522_ReadRegister(unsigned char  addr);
extern void MFRC522_SetBitMask(unsigned char  reg, unsigned char  mask);
extern void MFRC522_ClearBitMask(unsigned char  reg, unsigned char  mask);
extern unsigned char  MFRC522_Request(unsigned char  reqMode, unsigned char * TagType);
extern unsigned char  MFRC522_ToCard(unsigned char  command, unsigned char * sendData, unsigned char  sendLen, unsigned char * backData, unsigned short* backLen);
extern unsigned char  MFRC522_Anticoll(unsigned char * serNum);
extern void MFRC522_CalulateCRC(unsigned char * pIndata, unsigned char  len, unsigned char * pOutData);
extern unsigned char  MFRC522_SelectTag(unsigned char * serNum);
extern unsigned char  MFRC522_Auth(unsigned char  authMode, unsigned char  BlockAddr, unsigned char * Sectorkey, unsigned char * serNum);
extern unsigned char  MFRC522_Read(unsigned char  blockAddr, unsigned char * recvData);
extern unsigned char  MFRC522_Write(unsigned char  blockAddr, unsigned char * writeData);
extern void MFRC522_Init(void);
extern void MFRC522_Reset(void);
extern void MFRC522_AntennaOn(void);
extern void MFRC522_AntennaOff(void);
extern void MFRC522_Halt(void);

unsigned char  SPI1SendByte(unsigned char  data) {
//    unsigned char writeCommand[1];
//    unsigned char readValue[1];

//    writeCommand[0] = data;
//    HAL_SPI_TransmitReceive(&hspi1, (unsigned char *)&writeCommand, (unsigned char *)&readValue, 1, 10);
//    return readValue[0];

    //while (SPI1->SR & SPI_SR_BSY);								// STM32F030 - ждем конец передачи
    //while (SPI1->SR & SPI_I2S_FLAG_BSY);					// STM32F103 - ждем конец передачи

    //while (!(SPI1->SR & SPI_SR_TXE));     				// убедиться, что предыдущая передача завершена (STM32F103)
    //SPI1->DR = data;															// вывод в SPI1
    //while (!(SPI1->SR & SPI_SR_RXNE));     				// ждем окончания обмена (STM32F103)
    //for (unsigned char  i=0; i<50; i++) {};
    //data = SPI1->DR;															// читаем принятые данные
    //return data;
    
    return DAL_SPI1_ReadWriteByte(data);
}

void SPI1_WriteReg(unsigned char  address, unsigned char  value) {
    cs_reset();
    SPI1SendByte(address);
    SPI1SendByte(value);
    cs_set();
}

unsigned char  SPI1_ReadReg(unsigned char  address) {
    unsigned char 	val;

    cs_reset();
    SPI1SendByte(address);
    val = SPI1SendByte(0x00);
    cs_set();
    return val;
}

void MFRC522_WriteRegister(unsigned char  addr, unsigned char  val) {
    addr = (addr << 1) & 0x7E;															// Address format: 0XXXXXX0
    SPI1_WriteReg(addr, val);
}

unsigned char  MFRC522_ReadRegister(unsigned char  addr) {
    unsigned char  val;

    addr = ((addr << 1) & 0x7E) | 0x80;
    val = SPI1_ReadReg(addr);
    return val;
}

unsigned char  MFRC522_Check(unsigned char * id) {
    unsigned char  status;
    status = MFRC522_Request(PICC_REQIDL, id);							// Find cards, return card type
    if (status == MI_OK) status = MFRC522_Anticoll(id);			// Card detected. Anti-collision, return card serial number 4 bytes
    MFRC522_Halt();																					// Command card into hibernation
    return status;
}

unsigned char  MFRC522_Compare(unsigned char * CardID, unsigned char * CompareID) {
    unsigned char  i;
    for (i = 0; i < 5; i++) {
        if (CardID[i] != CompareID[i]) return MI_ERR;
    }
    return MI_OK;
}

void MFRC522_SetBitMask(unsigned char  reg, unsigned char  mask) {
    MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
}

void MFRC522_ClearBitMask(unsigned char  reg, unsigned char  mask) {
    MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
}

unsigned char  MFRC522_Request(unsigned char  reqMode, unsigned char * TagType) {
    unsigned char  status;
    unsigned short backBits;																			// The received data bits

    MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);		// TxLastBists = BitFramingReg[2..0]
    TagType[0] = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
    
    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }
    
    return status;
}

unsigned char  MFRC522_ToCard(unsigned char  command, unsigned char * sendData, unsigned char  sendLen, unsigned char * backData, unsigned short* backLen) {
    unsigned char  status = MI_ERR;
    unsigned char  irqEn = 0x00;
    unsigned char  waitIRq = 0x00;
    unsigned char  lastBits;
    unsigned char  n;
    unsigned short i;

    switch (command) {
    case PCD_AUTHENT: {
        irqEn = 0x12;
        waitIRq = 0x10;
        break;
    }
    case PCD_TRANSCEIVE: {
        irqEn = 0x77;
        waitIRq = 0x30;
        break;
    }
    default:
        break;
    }

    MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
    MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
    MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

    // Writing data to the FIFO
    for (i = 0; i < sendLen; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);

    // Execute the command
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);
    if (command == PCD_TRANSCEIVE) MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);		// StartSend=1,transmission of data starts

    // Waiting to receive data to complete
    i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
    do {
        // CommIrqReg[7..0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);																// StartSend=0

    if (i != 0)
    {
        if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (command == PCD_TRANSCEIVE)
            {
                n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
                lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MFRC522_MAX_LEN) 
				{
					n = MFRC522_MAX_LEN;
				}
                for (i = 0; i < n; i++) 
				{
					backData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);		// Reading the received data in FIFO
				}
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    return status;
}

unsigned char  MFRC522_Anticoll(unsigned char * serNum) {
    unsigned char  status;
    unsigned char  i;
    unsigned char  serNumCheck = 0;
    unsigned short unLen;

    MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);												// TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
    
    if (status == MI_OK) {
        // Check card serial number
        for (i = 0; i < 4; i++)
        {          
            serNumCheck ^= serNum[i];
        }
        
        if (serNumCheck != serNum[i]) 
        {
            status = MI_ERR;
        }
    }
    
    return status;
}

void MFRC522_CalculateCRC(unsigned char *  pIndata, unsigned char  len, unsigned char * pOutData) {
    unsigned char  i, n;

    MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);													// CRCIrq = 0
    MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);													// Clear the FIFO pointer
    // Write_MFRC522(CommandReg, PCD_IDLE);

    // Writing data to the FIFO
    for (i = 0; i < len; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata + i));
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

    // Wait CRC calculation is complete
    i = 0xFF;
    do {
        n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04));																						// CRCIrq = 1

    // Read CRC calculation result
    pOutData[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
    pOutData[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

unsigned char  MFRC522_SelectTag(unsigned char * serNum) {
    unsigned char  i;
    unsigned char  status;
    unsigned char  size;
    unsigned short recvBits;
    unsigned char  buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i = 0; i < 5; i++) buffer[i + 2] = *(serNum + i);
    MFRC522_CalculateCRC(buffer, 7, &buffer[7]);		//??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    if ((status == MI_OK) && (recvBits == 0x18)) size = buffer[0];
    else size = 0;
    return size;
}

unsigned char  MFRC522_Auth(unsigned char  authMode, unsigned char  BlockAddr, unsigned char * Sectorkey, unsigned char * serNum) {
    unsigned char  status;
    unsigned short recvBits;
    unsigned char  i;
    unsigned char  buff[12];

    // Verify the command block address + sector + password + card serial number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i = 0; i < 6; i++) buff[i + 2] = *(Sectorkey + i);
    for (i = 0; i < 4; i++) buff[i + 8] = *(serNum + i);
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
    if ((status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08))) status = MI_ERR;
    return status;
}

unsigned char  MFRC522_Read(unsigned char  blockAddr, unsigned char * recvData) {
    unsigned char  status;
    unsigned short unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    MFRC522_CalculateCRC(recvData, 2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
    if ((status != MI_OK) || (unLen != 0x90)) status = MI_ERR;
    return status;
}

unsigned char  MFRC522_Write(unsigned char  blockAddr, unsigned char * writeData) {
    unsigned char  status;
    unsigned short recvBits;
    unsigned char  i;
    unsigned char  buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    MFRC522_CalculateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
    if (status == MI_OK) {
        // Data to the FIFO write 16Byte
        for (i = 0; i < 16; i++) buff[i] = *(writeData + i);
        MFRC522_CalculateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
    }
    return status;
}

void MFRC522_Init(void) {
    MFRC522_Reset();
    MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
    MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
    MFRC522_WriteRegister(MFRC522_REG_RF_CFG, 0x7F);				// 48dB gain
    MFRC522_WriteRegister(MFRC522_REG_RX_THRESHOLD, 0x84);////////////////////////////<<<<<<<<<<<<<<<<<<<<
    MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
    MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
    MFRC522_AntennaOn();																		// Open the antenna
}

void MFRC522_Reset(void) {
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

void MFRC522_AntennaOn(void) {
    unsigned char  temp;

    temp = MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);
    if (!(temp & 0x03)) MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_AntennaOff(void) {
    MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_Halt(void) {
    unsigned short unLen;
    unsigned char  buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    MFRC522_CalculateCRC(buff, 2, &buff[2]);
    MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

static unsigned char str[16];
static unsigned char id[4] = {0, 0, 0, 0};

/****************************************************************
***函数名称：MFRC55Request
***函数功能：RC522读取ID操作
***输入参数：无
***返回值：无
***备注：放在任务中读取，建议10ms运行一次 
*****************************************************************/
void MFRC55Request(void)
{
    if (!MFRC522_Request(PICC_REQIDL, str)) 
    {
        if (!MFRC522_Anticoll(str)) 
        {
            for (unsigned char i = 0; i < 4; i++)
            {
                id[i] = str[i];
            }
        }
    }
}

/****************************************************************
***函数名称：GetRC522ID
***函数功能：获取读到的RF ID
***输入参数：idBuf， id缓存
***返回值：1，id数据正常，0，id数据异常
***备注：ID数据长度为4
*****************************************************************/
unsigned char GetRC522ID(unsigned char *idBuf)
{
    if ((id[0] != 0x00) && (id[1] != 0x00) && (id[2] != 0x00) && (id[3] != 0x00))
    {
        memcpy(idBuf, id, 0x04);
        memset(id, 0x00, 0x04);
        
        return 1;
    }
    
    return 0;
}

/****************************************************************
***函数名称：MFRC522Init
***函数功能：MFRC522初始化
***输入参数：无
***返回值：无
***备注：
*****************************************************************/
void MFRC522Init(void)
{
    memset(id, 0x00, 0x04);
    DAL_SPI1_Init();
    MFRC522_Init();
}
