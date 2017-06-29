#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ads1292r.h"

volatile char SPI_D_Buff[9];
volatile uint8_t  SPI_Dummy_Buff[30];
unsigned char DataPacketHeader[16];
volatile signed long s32DaqVals[8];
uint8_t data_len = 8;
volatile uint8_t SPI_RX_Buff[15] ;
volatile static int SPI_RX_Buff_Count = 0;
volatile char *SPI_RX_Buff_Ptr;
volatile bool ads1292dataReceived =false;
unsigned long uecgtemp = 0;
signed long secgtemp=0;
int i,j;

spi_device_handle_t spi;

void ads1292_Init(void)
{
    gpio_set_direction(ADS1292_DRDY_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(ADS1292_CS_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ADS1292_START_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ADS1292_PWDN_PIN, GPIO_MODE_OUTPUT);

    spi_bus_config_t buscfg=
    {
       .miso_io_num=PIN_NUM_MISO,
       .mosi_io_num=PIN_NUM_MOSI,
       .sclk_io_num=PIN_NUM_CLK,
       .quadwp_io_num=-1,
       .quadhd_io_num=-1,
    };

    spi_device_interface_config_t devcfg=
    {
       .clock_speed_hz=1000000,                 //Clock out at 1 MHz
       .mode=1,                                //SPI mode 1
       .spics_io_num=PIN_NUM_CS,               //CS pin
       .queue_size=7,
    };

    spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi);

    ads1292_Reset();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ads1292_Disable_Start();
    ads1292_Enable_Start();
    ads1292_Hard_Stop();
    ads1292_Start_Data_Conv_Command(spi);
    ads1292_Soft_Stop(spi);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    ads1292_Stop_Read_Data_Continuous(spi);					// SDATAC command
    vTaskDelay(300 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_CONFIG1, 0x00); 		//Set sampling rate to 125 SPS
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_CONFIG2, 0b10100000);	//Lead-off comp off, test signal disabled
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_LOFF, 0b00010000);		//Lead-off defaults
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_CH1SET, 0b01000000);	//Ch 1 enabled, gain 6, connected to electrode in :[originaldata 0b01000000]
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_CH2SET, 0b01100000);	//Ch 2 enabled, gain 6, connected to electrode in  predata:0b01100000
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_RLDSENS, 0b00101100);	//RLD settings: fmod/16, RLD enabled, RLD inputs from Ch2 only
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_LOFFSENS, 0x00);		//LOFF settings: all disabled
    vTaskDelay(10 / portTICK_PERIOD_MS);
  								//Skip register 8, LOFF Settings default
    ads1292_Reg_Write(spi, ADS1292_REG_RESP1, 0b11110010);		//Respiration: MOD/DEMOD turned only, phase 0
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Reg_Write(spi, ADS1292_REG_RESP2, 0b00000011);		//Respiration: Calib OFF, respiration freq defaults
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Start_Read_Data_Continuous(spi);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ads1292_Enable_Start();
}

void ads1292_Reset(void)
{
    gpio_set_level(ADS1292_PWDN_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_PWDN_PIN, LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_PWDN_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void ads1292_Disable_Start(void)
{
	 gpio_set_level(ADS1292_START_PIN, LOW);
   vTaskDelay(20 / portTICK_PERIOD_MS);
}

void ads1292_Enable_Start(void)
{
	 gpio_set_level(ADS1292_START_PIN, HIGH);
   vTaskDelay(20 / portTICK_PERIOD_MS);
}

void ads1292_Hard_Stop (void)
{
	 gpio_set_level(ADS1292_START_PIN, LOW);
   vTaskDelay(20 / portTICK_PERIOD_MS);
}

void ads1292_Start_Data_Conv_Command (spi_device_handle_t spi)
{
    gpio_set_level(ADS1292_CS_PIN, LOW);
    vTaskDelay(2 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_CS_PIN, HIGH);
    vTaskDelay(2 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_CS_PIN, LOW);
    vTaskDelay(2 / portTICK_PERIOD_MS);

    uint8_t data_out=START;

    esp_err_t ret;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&data_out;
    t.user=(void*)0;

    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);


    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(ADS1292_CS_PIN, HIGH);
}

void ads1292_Soft_Stop (spi_device_handle_t spi)
{
  	gpio_set_level(ADS1292_CS_PIN, LOW);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

  	gpio_set_level(ADS1292_CS_PIN, HIGH);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

  	gpio_set_level(ADS1292_CS_PIN, LOW);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

    spi_transaction_t t;
    uint8_t data_out=STOP;
    memset(&t, 0, sizeof(t));
    t.length=8;                     //Command length is 8 bits
    t.tx_buffer=&data_out;               //pointing STOPto tx buffer
    t.user=(void*)0;
    t.flags=SPI_DEVICE_HALFDUPLEX;

    spi_device_transmit(spi, &t);  //Transmit!

    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(ADS1292_CS_PIN, HIGH);
}


void ads1292_Start_Read_Data_Continuous (spi_device_handle_t spi)
{
  	gpio_set_level(ADS1292_CS_PIN, LOW);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

   	gpio_set_level(ADS1292_CS_PIN, HIGH);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

  	gpio_set_level(ADS1292_CS_PIN, LOW);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

  	uint8_t data_out=RDATAC;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=8;
    t.tx_buffer=&data_out;
    t.flags=SPI_DEVICE_HALFDUPLEX;
    t.user=(void*)0;

	  spi_device_transmit(spi, &t); 				 //Send RDATAC to the ADS1x9x
	  vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(ADS1292_CS_PIN, HIGH);
}

void ads1292_Stop_Read_Data_Continuous (spi_device_handle_t spi)
{
    gpio_set_level(ADS1292_CS_PIN, LOW);
    vTaskDelay(2 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_CS_PIN, HIGH);
    vTaskDelay(2 / portTICK_PERIOD_MS);

    gpio_set_level(ADS1292_CS_PIN, LOW);
    vTaskDelay(2 / portTICK_PERIOD_MS);

	  uint8_t data_out=SDATAC;

    spi_transaction_t t;
   	memset(&t, 0, sizeof(t));
    t.length=8;
   	t.tx_buffer=&data_out;
   	t.user=(void*)0;
   	t.flags=SPI_DEVICE_HALFDUPLEX;

  	spi_device_transmit(spi, &t); 		 // Send SDATAC to the ADS1x9x
  	vTaskDelay(2 / portTICK_PERIOD_MS);

	  gpio_set_level(ADS1292_CS_PIN, HIGH);
}


void ads1292_Reg_Write (spi_device_handle_t spi, unsigned char READ_WRITE_ADDRESS, unsigned char DATA)
{
    switch (READ_WRITE_ADDRESS)
    {
        case 1:
          DATA = DATA & 0x87;
    	    break;
        case 2:
                DATA = DATA & 0xFB;
    	    DATA |= 0x80;
    	    break;
        case 3:
    	    DATA = DATA & 0xFD;
    	    DATA |= 0x10;
    	    break;
        case 7:
    	    DATA = DATA & 0x3F;
    	    break;
        case 8:
        	    DATA = DATA & 0x5F;
    	    break;
        case 9:
    	    DATA |= 0x02;
    	    break;
        case 10:
    	    DATA = DATA & 0x87;
    	    DATA |= 0x01;
    	    break;
        case 11:
    	    DATA = DATA & 0x0F;
    	    break;
        default:
    	    break;
    }

 // now combine the register address and the command into one byte:
    uint8_t dataToSend = READ_WRITE_ADDRESS | WREG;

  	gpio_set_level(ADS1292_CS_PIN, LOW);
  	vTaskDelay(2 / portTICK_PERIOD_MS);

   	gpio_set_level(ADS1292_CS_PIN, HIGH);
	  vTaskDelay(2 / portTICK_PERIOD_MS);

  	gpio_set_level(ADS1292_CS_PIN, LOW);
	  vTaskDelay(2 / portTICK_PERIOD_MS);

    uint8_t data_out=dataToSend;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=8;
    t.tx_buffer=&data_out;
    t.user=(void*)0;
    t.flags=SPI_DEVICE_HALFDUPLEX;

    spi_device_transmit(spi, &t); 		 //sends 'dataToSend' to ads1292r

    data_out=0x00;
    spi_device_transmit(spi, &t); 		//sends 0x00 to

    data_out=DATA;
    spi_device_transmit(spi, &t);  //Transmit!

	  vTaskDelay(2 / portTICK_PERIOD_MS);
  	gpio_set_level(ADS1292_CS_PIN, HIGH);

}

static void ads1292r_read_task(void *pvParameters)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.rxlength=72;
    t.flags=SPI_DEVICE_HALFDUPLEX;
    t.rx_buffer=SPI_D_Buff;

    while (true)
    {
       esp_err_t ret;


       if((gpio_get_level(ADS1292_DRDY_PIN)) == LOW)       // Sampling rate is set to 125SPS ,DRDY ticks for every 8ms
       {
           gpio_set_level(ADS1292_CS_PIN, LOW);
           ret=spi_device_transmit(spi, &t); 		// Read the data, stores it to - SPI_D_Buff[]
           assert(ret==ESP_OK);
           gpio_set_level(ADS1292_CS_PIN, HIGH);
           ads1292dataReceived = true;
       }


       if(ads1292dataReceived == true)       // process the data
       {
           j=0;
           for(i=0;i<6;i+=3)                  // data outputs is (24 status bits + 24 bits Respiration data +  24 bits ECG data)
           {
               uecgtemp = (unsigned long) (  ((unsigned long)SPI_D_Buff[i+3] << 16) | ( (unsigned long) SPI_D_Buff[i+4] << 8) |  (unsigned long)   	SPI_D_Buff[i+5]);

               uecgtemp = (unsigned long) (uecgtemp << 8);
               secgtemp = (signed long) (uecgtemp);
               secgtemp = (signed long) (secgtemp >> 8);

               s32DaqVals[j++]=secgtemp;
           }


           DataPacketHeader[0] = CES_CMDIF_PKT_START_1 ;   // Packet header1 :0x0A
           DataPacketHeader[1] = CES_CMDIF_PKT_START_2;    // Packet header2 :0xFA
           DataPacketHeader[2] = (uint8_t) (data_len);     // data length
           DataPacketHeader[3] = (uint8_t) (data_len>>8);
           DataPacketHeader[4] = CES_CMDIF_TYPE_DATA;      // packet type: 0x02 -data 0x01 -commmand

           DataPacketHeader[5] = s32DaqVals[1];            // 4 bytes ECG data
           DataPacketHeader[6] = s32DaqVals[1]>>8;
           DataPacketHeader[7] = s32DaqVals[1]>>16;
           DataPacketHeader[8] = s32DaqVals[1]>>24;

           DataPacketHeader[9] = s32DaqVals[0];            // 4 bytes Respiration data
           DataPacketHeader[10] = s32DaqVals[0]>>8;
           DataPacketHeader[11] = s32DaqVals[0]>>16;
           DataPacketHeader[12] = s32DaqVals[0]>>24;

           DataPacketHeader[13] = CES_CMDIF_PKT_STOP_1;   // Packet footer1:0x00
           DataPacketHeader[14] = CES_CMDIF_PKT_STOP; // Packet footer2:0x0B


           for(i=0; i<15; i++)
           {
              putc(DataPacketHeader[i], stdout);				//sends the ECG/RESP data to serialport
           }
        }

        ads1292dataReceived = false;
        SPI_RX_Buff_Count = 0;
    }
}

void ads1292r_read_into_buffer(void)
{
    esp_err_t ret;

    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.rxlength=72;
    t.flags=SPI_DEVICE_HALFDUPLEX;
    t.rx_buffer=SPI_D_Buff;

    if((gpio_get_level(ADS1292_DRDY_PIN)) == LOW)       // Sampling rate is set to 125SPS ,DRDY ticks for every 8ms
    {
        gpio_set_level(ADS1292_CS_PIN, LOW);
        ret=spi_device_transmit(spi, &t); 		// Read the data, stores it to - SPI_D_Buff[]
        assert(ret==ESP_OK);
        gpio_set_level(ADS1292_CS_PIN, HIGH);
        ads1292dataReceived = true;
    }

    if(ads1292dataReceived == true)       // process the data
    {
        j=0;
        for(i=0;i<6;i+=3)                  // data outputs is (24 status bits + 24 bits Respiration data +  24 bits ECG data)
        {
            uecgtemp = (unsigned long) (  ((unsigned long)SPI_D_Buff[i+3] << 16) | ( (unsigned long) SPI_D_Buff[i+4] << 8) |  (unsigned long)   	SPI_D_Buff[i+5]);

            uecgtemp = (unsigned long) (uecgtemp << 8);
            secgtemp = (signed long) (uecgtemp);
            secgtemp = (signed long) (secgtemp >> 8);

            s32DaqVals[j++]=secgtemp;
        }


        DataPacketHeader[0] = CES_CMDIF_PKT_START_1 ;   // Packet header1 :0x0A
        DataPacketHeader[1] = CES_CMDIF_PKT_START_2;    // Packet header2 :0xFA
        DataPacketHeader[2] = (uint8_t) (data_len);     // data length
        DataPacketHeader[3] = (uint8_t) (data_len>>8);
        DataPacketHeader[4] = CES_CMDIF_TYPE_DATA;      // packet type: 0x02 -data 0x01 -commmand

        DataPacketHeader[5] = s32DaqVals[1];            // 4 bytes ECG data
        DataPacketHeader[6] = s32DaqVals[1]>>8;
        DataPacketHeader[7] = s32DaqVals[1]>>16;
        DataPacketHeader[8] = s32DaqVals[1]>>24;

        DataPacketHeader[9] = s32DaqVals[0];            // 4 bytes Respiration data
        DataPacketHeader[10] = s32DaqVals[0]>>8;
        DataPacketHeader[11] = s32DaqVals[0]>>16;
        DataPacketHeader[12] = s32DaqVals[0]>>24;

        DataPacketHeader[13] = CES_CMDIF_PKT_STOP_1;   // Packet footer1:0x00
        DataPacketHeader[14] = CES_CMDIF_PKT_STOP; // Packet footer2:0x0B


        /*
        for(i=0; i<15; i++)
        {
           putc(DataPacketHeader[i], stdout);				//sends the ECG/RESP data to serialport
        }
        */
     }
}

void ads1292r_start(void)
{
    TaskHandle_t ads_read_task;
    xTaskCreate(&ads1292r_read_task, "ads_read_task", 4096, NULL, 4, &ads_read_task);
}
