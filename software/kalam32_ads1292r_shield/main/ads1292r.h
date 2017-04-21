#ifndef ads1292r_h
#define ads1292r_h

#define CONFIG_SPI_MASTER_DUMMY   0xFF

// Register Read Commands
#define  RREG   0x20;
#define WREG    0x40;
#define START	0x08
#define STOP	0x0A
#define RDATAC  0x10

//This mode is the default mode at power-up.
#define SDATAC		0x11		//Stop Read Data Continuously mode
#define RDATA	0x12 			//Read data by command; supports multiple read back.

#define ADS1292_DRDY_PIN 26
#define ADS1292_CS_PIN 17
#define ADS1292_START_PIN 16
#define ADS1292_PWDN_PIN 2


#define ADS1292_REG_ID			0x00
#define ADS1292_REG_CONFIG1		0x01
#define ADS1292_REG_CONFIG2		0x02
#define ADS1292_REG_LOFF		0x03
#define ADS1292_REG_CH1SET		0x04
#define ADS1292_REG_CH2SET		0x05
#define ADS1292_REG_RLDSENS		0x06
#define ADS1292_REG_LOFFSENS   		 0x07
#define ADS1292_REG_LOFFSTAT   		 0x08
#define ADS1292_REG_RESP1	   	 0x09
#define ADS1292_REG_RESP2	0x0A

#define  CES_CMDIF_PKT_START_1     0x0A
#define  CES_CMDIF_PKT_START_2     0xFA
#define  CES_CMDIF_TYPE_DATA       0x02
#define  CES_CMDIF_PKT_STOP        0x0B
#define  CES_CMDIF_PKT_STOP_1      0x00

#ifndef HIGH
#define HIGH 1
#define LOW 0
#endif

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   12

#define PIN_NUM_DC   21

void ads1292_Init(void);
void ads1292r_start(void);
void ads1292r_read_into_buffer(void);

void ads1292_Reset(void);
void ads1292_Reg_Write (spi_device_handle_t spi, unsigned char READ_WRITE_ADDRESS, unsigned char DATA);
void ads1292_Reg_Read (unsigned char READ_WRITE_ADDRESS);
void ads1292_SPI_Command_Data(spi_device_handle_t spi, unsigned char data_in);
void ads1292_Disable_Start(void);
void ads1292_Enable_Start(void);
void ads1292_Hard_Stop (void);
void ads1292_Start_Data_Conv_Command(spi_device_handle_t spi);
void ads1292_Soft_Stop (spi_device_handle_t spi);
void ads1292_Start_Read_Data_Continuous (spi_device_handle_t spi);
void ads1292_Stop_Read_Data_Continuous (spi_device_handle_t spi);

#endif
