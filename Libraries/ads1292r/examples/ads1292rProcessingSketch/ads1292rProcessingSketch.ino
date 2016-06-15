#include <ads1292r.h>
#include <SPI.h>
uint8_t DataPacketHeader[16];
ads1292r ADS1292;
//Packet format
#define  CES_CMDIF_PKT_START_1   0x0A
#define CES_CMDIF_PKT_START_2   0xFA
#define CES_CMDIF_TYPE_DATA   0x02
#define CES_CMDIF_PKT_STOP    0x0B

uint8_t data_len = 8;
unsigned long time;

volatile byte SPI_RX_Buff[150] ;
volatile static int SPI_RX_Buff_Count = 0;
volatile char *SPI_RX_Buff_Ptr;
volatile int Responsebyte = false;
volatile unsigned int pckt =0 , buff=0,t=0 , j1=0,j2=0;
volatile unsigned long int EEG_Ch1_Data[150],EEG_Ch2_Data[150];
volatile unsigned char datac[150];
unsigned long ueegtemp = 0,Pkt_Counter=0;
signed long seegtemp=0;
volatile int i;
volatile long packet_counter=0;


void setup() 
{
  // initalize the  data ready and chip select pins:
  pinMode(ADS1292_DRDY_PIN, INPUT);  //6
  pinMode(ADS1292_CS_PIN, OUTPUT);    //7
  pinMode(ADS1292_START_PIN, OUTPUT);  //5
  pinMode(ADS1292_PWDN_PIN, OUTPUT);  //4
  
  // Serial.begin(9600);  // Baudrate for serial communica
  //initalize ADS1292 slave
  ADS1292.ads1292_Init();
  //ADS1292.ads1292_Reset();
  

}

void loop() 
{
  if((digitalRead(ADS1292_DRDY_PIN)) == LOW)
  {  
    SPI_RX_Buff_Ptr = ADS1292.ads1292_Read_Data();
    Responsebyte = true;
    //Serial.print("DRDY low: "); 
  }

  if(Responsebyte == true)
  {
    /*Serial.print("start Time: ");
    time = millis();
    //prints time since program started
    Serial.println(time);
    */   
    for(i = 0; i < 9; i++)
    {
      SPI_RX_Buff[SPI_RX_Buff_Count++] = *(SPI_RX_Buff_Ptr + i);
    }
    Responsebyte = false;
  }
  
  if(SPI_RX_Buff_Count >= 9)
  {     
    pckt = 0; j1=0;   j2=0;
    for(i=3;i<9;i+=9)
    {
      //udi_cdc_putc(SPI_RX_Buff[i]);
      //EEG_Ch1_Data[j1++]=  SPI_RX_Buff[i+0];
      //EEG_Ch1_Data[j1++]= SPI_RX_Buff[i+1];
      //EEG_Ch1_Data[j1++]= SPI_RX_Buff[i+2];
    


      EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+3];
      EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+4];
      EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+5];

    }
    //seegtemp = -89485;  
     packet_counter++;
     
    ueegtemp = (unsigned long) ((EEG_Ch2_Data[0]<<16)|(EEG_Ch2_Data[1]<<8)|EEG_Ch2_Data[2]);
    ueegtemp = (unsigned long) (ueegtemp<<8);
    seegtemp = (signed long) (ueegtemp);
    seegtemp = (signed long) (seegtemp>>8); 
     //Serial.println(volt_int32);Serial.println(volt);

    DataPacketHeader[0] = 0x0A;
    DataPacketHeader[1] = 0xFA;
    DataPacketHeader[2] = (uint8_t) (data_len);
    DataPacketHeader[3] = (uint8_t) (data_len>>8);
    DataPacketHeader[4] = 0x02;

    DataPacketHeader[5] = packet_counter;
    DataPacketHeader[6] = packet_counter>>8;
    DataPacketHeader[7] = packet_counter>>16;
    DataPacketHeader[8] = packet_counter>>24; 

    DataPacketHeader[9] = seegtemp;
    DataPacketHeader[10] = seegtemp>>8;
    DataPacketHeader[11] = seegtemp>>16;
    DataPacketHeader[12] = seegtemp>>24; 

    DataPacketHeader[13] = 0x00;
    DataPacketHeader[14] = 0x0b;

    for(i=0; i<15; i++) // transmit the data
    {
    //    Serial.write(DataPacketHeader[i]);
        Serial.write(DataPacketHeader[i]);
        //udi_cdc_putc(datac[i]);
        //cpu_delay_us(90, 48000000);   
     } 

             
            
  // Serial.println( seegtemp);
    }
    
    SPI_RX_Buff_Count = 0;
         
  }           


    


