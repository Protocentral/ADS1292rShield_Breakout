#include <ads1292r.h>
#include <SPI.h>
#include <Process.h>
#include <FIR.h>

#define BPM_WINDOW 10
#define FILTERTAPS 5
#define APIKEY        "QRLlAi3IqpTr4DKIoI7dZrVPA8H9QIDU5hMZItzBMnN1bj4S"   // your pachube api key 
#define FEEDID        1853201959                             // your feed ID

// set up net client info:
const unsigned long postingInterval = 1000;  //delay between updates to xively.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";
long int bpm = 0; 

ads1292r ADS1292;
FIR fir;

volatile char DataPacketHeader[5];
volatile char DataPacketFooter[2];
volatile int datalen = 135;
unsigned long time;

volatile byte SPI_RX_Buff[150] ;
//volatile static int SPI_RX_Buff_Count = 0;
volatile char *SPI_RX_Buff_Ptr;
volatile int Responsebyte = false;
volatile unsigned int pckt =0 , buff=0,t=0 , j1=0,j2=0;
volatile unsigned long int EEG_Ch1_Data[150],EEG_Ch2_Data[150];
volatile unsigned char datac[150];
unsigned long ueegtemp = 0,Pkt_Counter=0;
signed long seegtemp=0, value;
volatile int i;
signed long adcval_prev;

float feegtemp ,output;
 

void setup() 
{
    Bridge.begin();       
  
    // initalize the  data ready andchip select pins:
    pinMode(ADS1292_DRDY_PIN, INPUT);  //6
    pinMode(ADS1292_CS_PIN, OUTPUT);    //7
    pinMode(ADS1292_START_PIN, OUTPUT);  //5
    pinMode(ADS1292_PWDN_PIN, OUTPUT);  //4
  
    // Serial.begin(9600);  // Baudrate for serial communica
    //initalize ADS1292 slave
    ADS1292.ads1292_Init();
    //ADS1292.ads1292_Reset();
  
    Serial.begin(9600);
  
    float coef[FILTERTAPS] = { 0.021, 0.096, 0.146, 0.096, 0.021};
    fir.setCoefficients(coef);

    //declare gain coefficient to scale the output back to normal
    float gain = 1; // set to 1 and input unity to see what this needs to be
    fir.setGain(gain);

    Serial.println("Xively client");
    lastRequest = millis();
}

void loop() 
{  
    long now = millis();


    //Do update and send every postingInterval
    if (now - lastRequest >= postingInterval) {      
    updateData();
    sendData();
    lastRequest = now;
  }  
} 
  


//It averages the bpm over a length of BPM_WINDOW to avoid sudden changes
int average_BPM( int bpm)
{
	static int bpm_sample_count;
	static int bpm_store[BPM_WINDOW];
	int bpm_sum =0, bpm_avg=0;

	if (bpm_sample_count < BPM_WINDOW)
		bpm_store[bpm_sample_count++] = bpm;

	else	
	{
		for(i=0 ; i<BPM_WINDOW-1 ; i++) 
			bpm_store[i] = bpm_store[i+1] ; 
		
		bpm_store[BPM_WINDOW-1] = bpm;
	}


        Serial.println("\nbpm_store\t");
	for(i=0 ; i<BPM_WINDOW ; i++)
        {
		Serial.print(bpm_store[i]);
                Serial.print("\t");
        } 


     	Serial.print("\nbpm_sample_count\t");
	Serial.println(bpm_sample_count);

	for(i=0 ; i<BPM_WINDOW ; i++)
	    bpm_sum += bpm_store[i];

        Serial.print("\nbpm_sum\t");
	Serial.println(bpm_sum);

	bpm_avg = bpm_sum/(bpm_sample_count);

	Serial.print("\nbpm_avg\t");
	Serial.println(bpm_avg);

	return (bpm_avg);
}
  

void updateData() {

    signed long  adcval, val, val_high = 0, val_low =0; 
    long int t1, t2;
  
    int flag = 0;
    adcval =  ReadADC();  

    adcval_prev = adcval; 
    
    val_high = val_low =  adcval;
    
    // caluculate the higs and lows for 2 sec ie 375 samples    
    for(int i=0; i<250; i++)            
    {  
        adcval =  ReadADC();
        val = adcval;
        if(val > val_high)
            val_high = val;
        if(val < val_low)
            val_low = val;
          
        Serial.print(val);
        Serial.print("\t");
    }

    Serial.println("val_low");
    Serial.println(val_low);
    Serial.println("val_high");
    Serial.println(val_high);
    Serial.println("val_high - val_low"); 
    Serial.println(val_high - val_low); 

    // caluculate the timeperiod between 2 peaks ie >90% of signal for next 2 sec
    for(int i=0; i<250; i++)                                                     
    {  
        adcval =  ReadADC();
        val = adcval;
        //int calval = map(val, val_low, val_high, 0, 100000);
        // calval = map(val, val_low, val_high, 0, 10000)
        //int calval = map(val, 0, val_high, 0, 100000);
      
        Serial.print(val);
        Serial.print("\t");
      
        if (val < val_low*0.70)
        {
            if (flag == 0)
            {
                t1 = millis();
                flag = 1;
                Serial.println("\npeak1 time");
                Serial.println(t1);
                Serial.println("peak1 value");
                Serial.println(val);
                //delay(100);                                                //QRS duration is 200ms
            }
            else if (flag == 1)
            {
                t2 = millis();
                Serial.println("\npeak2 time");
                Serial.println(t2);
                Serial.println("peak2 value");
                Serial.println(val);              
                flag = 2;
               
                Serial.println("timeperiod");                                 //caluculating timeperiod of beats
                Serial.println(t2 - t1); 
              
                Serial.print("BPM is ---------------->");   
                bpm = (60000)/(t2-t1);                                       //Caluculating the BPM from timeperiod.
                Serial.println(bpm); 
                //delay(100);                                                //QRS duration is 200ms
                bpm = average_BPM(bpm);
            }
             Serial.print("peakfound value"); 
             Serial.println(val);        
             
             while ((adcval =  ReadADC()) < val_low*0.70)                     // wait for the peak to end
             {
                 delay (1);
             }
        }          
    } 
    delay(250);


    
    dataString = "BPM,";

    dataString += bpm;
    // delay(1000);  
}  


//IIR filter to tune out the dc component.
float Filter(float fInput) {                                                                  
    static float fLast = 0.0f;
    int a = -1;
    float fResult = fInput+a*fLast;
    fLast = fInput;
    return 0.5*fResult;
    }


signed long  ReadADC(void)
{
    volatile int SPI_RX_Buff_Count = 0;
    while(1)  
    {
        //volatile int SPI_RX_Buff_Count = 0;
  	if((digitalRead(ADS1292_DRDY_PIN)) == LOW)
	{  
            SPI_RX_Buff_Ptr = ADS1292.ads1292_Read_Data();
            Responsebyte = true;
            //Serial.print("DRDY low: "); 
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
					
                //datac[buff++] = 0xA0;  // sync0
               // datac[buff++] = 36;   //sync1
					
                datac[buff++] = (unsigned char)(Pkt_Counter >> 24);
                datac[buff++] = (unsigned char)(Pkt_Counter >> 16);
                datac[buff++] = (unsigned char)(Pkt_Counter >> 8);
                datac[buff++] = (unsigned char)(Pkt_Counter );
											
                ueegtemp = (unsigned long) ((EEG_Ch2_Data[pckt]<<16)|(EEG_Ch2_Data[pckt+1]<<8)|EEG_Ch2_Data[pckt+2]);
                //Serial.print(ueegtemp,HEX);        
                //Serial.print("  ");                
                ueegtemp = (unsigned long) (ueegtemp<<8);
                seegtemp = (signed long) (ueegtemp);
                seegtemp = (signed long) (seegtemp>>8);	
            
                feegtemp = (float)((seegtemp)*50); 
                        
                output = fir.process(feegtemp);                                                                    //FIR filtering for smoothening the ECG
                output = Filter(output);                                                                          //IIR filter to filter out dc
                seegtemp = (signed long)((output));
                            
                //seegtemp = abs(seegtemp);
                return(seegtemp);					
                //delay(100);
            }
		
            SPI_RX_Buff_Count = 0;
        }
    }						
} 

//To send the BPM to Xiveley
void sendData() {
  // form the string for the API header parameter:
  String apiString = "X-ApiKey: ";
  apiString += APIKEY;

  // form the string for the URL parameter:
  String url = "https://api.xively.com/v2/feeds/";
  url += FEEDID;
  url += ".csv";

  // Send the HTTP PUT request

  // Is better to declare the Process here, so when the
  // sendData function finishes the resources are immediately
  // released. Declaring it global works too, BTW.
  Process xively;
  Serial.print("\n\nSending data... ");
  
  xively.begin("curl");
  xively.addParameter("-k");
  xively.addParameter("--request");
  xively.addParameter("PUT");
  xively.addParameter("--data");
  xively.addParameter(dataString);
  xively.addParameter("--header");
  xively.addParameter(apiString); 
  xively.addParameter(url);

  
  Serial.println(dataString);
  Serial.println(apiString);
  Serial.println(url);

  xively.run();
  Serial.println("done!");

      // If there's incoming data from the net connection,
  // send it out the Serial:
  while (xively.available()>0) {
    char c = xively.read();
    Serial.write(c);
  }
}
