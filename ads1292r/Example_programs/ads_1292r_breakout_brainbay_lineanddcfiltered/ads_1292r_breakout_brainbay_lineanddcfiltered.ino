#include <FIR.h>
#include <ads1292r.h>
#include <SPI.h>

#define FILTERTAPS 5
#define	CES_CMDIF_PKT_START_1		0x0A
#define	CES_CMDIF_PKT_START_2		0xFA
#define	CES_CMDIF_TYPE_DATA		0x02
#define	CES_CMDIF_PKT_STOP		0x0B

ads1292r ADS1292;
FIR fir;

volatile char DataPacketHeader[5];
volatile char DataPacketFooter[2];
volatile int datalen = 135;
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
 float feegtemp ,output;

void setup() {

        // initalize the  data ready and chip select pins:
        pinMode(ADS1292_DRDY_PIN, INPUT);  //6
        pinMode(ADS1292_CS_PIN, OUTPUT);    //7
        pinMode(ADS1292_START_PIN, OUTPUT);  //5
        pinMode(ADS1292_PWDN_PIN, OUTPUT);  //4
        
        //initalize ADS1292 slave
       ADS1292.ads1292_Init();
       //ADS1292.ads1292_Reset();
        
        DataPacketHeader[0] = CES_CMDIF_PKT_START_1;
        DataPacketHeader[1] = CES_CMDIF_PKT_START_2;
        DataPacketHeader[2] = (datalen);
        DataPacketHeader[3] = (datalen >> 8);
        DataPacketHeader[4] = CES_CMDIF_TYPE_DATA;
        
        DataPacketFooter[0] = 0x00;
        DataPacketFooter[1] = CES_CMDIF_PKT_STOP;
        
	// declare variables for coefficients
	// these should be calculated by hand, or using a tool
	// in case a phase linear filter is required, the coefficients are symmetric
	// for time optimization it seems best to enter symmetric values like below
	float coef[FILTERTAPS] = { 0.021, 0.096, 0.146, 0.096, 0.021};
	fir.setCoefficients(coef);

	  //declare gain coefficient to scale the output back to normal
	float gain = 1; // set to 1 and input unity to see what this needs to be
	fir.setGain(gain);
}

void loop() {

  
  ReadAdc();

}

float Filter(float fInput) {
    static float fLast = 0.0f;
    int a = -1;
    float fResult = fInput+a*fLast;
    fLast = fInput;
    return 0.5*fResult;
    }

void ReadAdc()
{
 if((digitalRead(ADS1292_DRDY_PIN)) == LOW)
  {  
    SPI_RX_Buff_Ptr = ADS1292.ads1292_Read_Data();
    Responsebyte = true;
     
  }

  if(Responsebyte == true)
  {
     for(i = 0; i < 9; i++)
     {
       SPI_RX_Buff[SPI_RX_Buff_Count++] = *(SPI_RX_Buff_Ptr + i);
     }
     Responsebyte = false;
  }
  
  if(SPI_RX_Buff_Count >= 9)
  {  

        
    	pckt = 0;	j1=0;		j2=0;
	for(i=3;i<9;i+=9)
	{

		EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+3];
		EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+4];
		EEG_Ch2_Data[j2++]= (unsigned char)SPI_RX_Buff[i+5];
										
	}	
	
		for(t=0; t< 1 ; t++)
		{	
			buff = 0;
			Pkt_Counter++; //if(Pkt_Counter > 0) Pkt_Counter = 0x00;
					
			datac[buff++] = 0xA0;  // sync0
			datac[buff++] = 36;   //sync1
					
			datac[buff++] = (unsigned char)(Pkt_Counter >> 24);
			datac[buff++] = (unsigned char)(Pkt_Counter >> 16);
			datac[buff++] = (unsigned char)(Pkt_Counter >> 8);
			datac[buff++] = (unsigned char)(Pkt_Counter );
											
                        ueegtemp = (unsigned long) ((EEG_Ch2_Data[pckt]<<16)|(EEG_Ch2_Data[pckt+1]<<8)|EEG_Ch2_Data[pckt+2]);
                        
                      
            		ueegtemp = (unsigned long) (ueegtemp<<8);
			seegtemp = (signed long) (ueegtemp);
			seegtemp = (signed long) (seegtemp>>8);	
                        //feegtemp = (float)((seegtemp)* 2.23517E-05 ); 
                        feegtemp = (float)((seegtemp*50)); 
                        
                        output = fir.process(feegtemp);
                        //output = Filter(output);
                        //seegtemp = (signed long)((output)/ 2.23517E05 );
                        seegtemp = (signed long)((output));
                        //seegtemp = abs(seegtemp);
                        
			pckt+= 3;
							
			datac[buff++] = (unsigned char) (seegtemp);
			datac[buff++] = (unsigned char) (seegtemp>>8);   //>>8
			datac[buff++] = ( unsigned char) (seegtemp >> 16); //>>16
			datac[buff++] = (unsigned char) (seegtemp >>24);

                       // Serial.print(seegtemp);
					
			for(i=0;i<28;i++)  // fill channel 1 buff
			{
	        		datac[buff++] = 0x00 ;
			}						
						
				datac[buff++] = 0xc0;//(uint8_t)seegtemp ;
			
			
			for(i=0; i<39; i++) // transmit the data
			{

                            Serial.write(datac[i]);
								
			}					
				
		}
		
		SPI_RX_Buff_Count = 0;
 
					
	}
					 
  
} 
