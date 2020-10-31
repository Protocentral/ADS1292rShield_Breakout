//////////////////////////////////////////////////////////////////////////////////////////
//
//   Arduino Library for ADS1292R Shield/Breakout
//
//   Copyright (c) 2017 ProtoCentral
//   Heartrate and respiration computation based on original code from Texas Instruments
//
//   This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   Requires g4p_control graphing library for processing.  Built on V4.1
//   Downloaded from Processing IDE Sketch->Import Library->Add Library->G4P Install
//   If you have bought the breakout the connection with the Arduino board is as follows:
//
//  |ads1292r pin label| Arduino Connection   |Pin Function      |
//  |----------------- |:--------------------:|-----------------:|
//  | VDD              | +5V                  |  Supply voltage  |
//  | PWDN/RESET       | D4                   |  Reset           |
//  | START            | D5                   |  Start Input     |
//  | DRDY             | D6                   |  Data Ready Outpt|
//  | CS               | D7                   |  Chip Select     |
//  | MOSI             | D11                  |  Slave In        |
//  | MISO             | D12                  |  Slave Out       |
//  | SCK              | D13                  |  Serial Clock    |
//  | GND              | Gnd                  |  Gnd             |
//
/////////////////////////////////////////////////////////////////////////////////////////


#include "protocentralAds1292r.h"
#include "ecgRespirationAlgo.h"
#include <SPI.h>

volatile uint8_t global_HeartRate = 0;
volatile uint8_t global_RespirationRate=0;

//Pin declartion the other you need are controlled by the SPI library
const int ADS1292_DRDY_PIN = 6;
const int ADS1292_CS_PIN = 7;
const int ADS1292_START_PIN = 5;
const int ADS1292_PWDN_PIN = 4;

uint8_t DataPacketHeader[30];
uint8_t data_len = 20;

int16_t ecg_wave_buff, ecg_filterout;
int16_t res_wave_buff,resp_filterout;

ads1292r ADS1292R;   // define class ads1292r
ecg_respiration_algorithm ECG_RESPIRATION_ALGORITHM; // define class ecg_algorithm

void setup()
{
  delay(2000);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST); 
  //CPOL = 0, CPHA = 1
  SPI.setDataMode(SPI_MODE1);
  // Selecting 1Mhz clock for SPI
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  pinMode(ADS1292_DRDY_PIN, INPUT);
  pinMode(ADS1292_CS_PIN, OUTPUT);
  pinMode(ADS1292_START_PIN, OUTPUT);
  pinMode(ADS1292_PWDN_PIN, OUTPUT);

  Serial.begin(115200); 
  ADS1292R.ads1292_Init(ADS1292_CS_PIN,ADS1292_PWDN_PIN,ADS1292_START_PIN);
  Serial.println("Initiliziation is done");
}

void loop()
{
  ads1292_output_values ecg_respiration_values;
  boolean ret = ADS1292R.ads1292_ecg_and_respiration_samples(ADS1292_DRDY_PIN,ADS1292_CS_PIN,&ecg_respiration_values);
  
  if (ret == true)
  {
    ecg_wave_buff = (int16_t)(ecg_respiration_values.s_Daq_Vals[1] >> 8) ;  // ignore the lower 8 bits out of 24bits 
    res_wave_buff = (int16_t)(ecg_respiration_values.sresultTempResp>>8) ; 
    
    if(ecg_respiration_values.leadoff_detected == false) 
    {
      ECG_RESPIRATION_ALGORITHM.ECG_ProcessCurrSample(&ecg_wave_buff, &ecg_filterout);   // filter out the line noise @40Hz cutoff 161 order
      ECG_RESPIRATION_ALGORITHM.QRS_Algorithm_Interface(ecg_filterout,&global_HeartRate); // calculate
      //resp_filterout = ECG_RESPIRATION_ALGORITHM.Resp_ProcessCurrSample(res_wave_buff); 
      //ECG_RESPIRATION_ALGORITHM.RESP_Algorithm_Interface(resp_filterout,&global_RespirationRate);
    
    }else{
      ecg_filterout = 0;
      resp_filterout = 0;
    }
    
    DataPacketHeader[0] = CES_CMDIF_PKT_START_1 ;   // Packet header1 :0x0A
    DataPacketHeader[1] = CES_CMDIF_PKT_START_2;    // Packet header2 :0xFA
    DataPacketHeader[2] = (uint8_t) (data_len);     // data length
    DataPacketHeader[3] = (uint8_t) (data_len >> 8);
    DataPacketHeader[4] = CES_CMDIF_TYPE_DATA;      // packet type: 0x02 -data 0x01 -commmand
    DataPacketHeader[5] = ecg_filterout;
    DataPacketHeader[6] = ecg_filterout >> 8;
    DataPacketHeader[7] = res_wave_buff;
    DataPacketHeader[8] = res_wave_buff >> 8;

    DataPacketHeader[19] = global_RespirationRate; 
    DataPacketHeader[21] = global_HeartRate;
    DataPacketHeader[25]= CES_CMDIF_PKT_STOP_1;   // Packet footer1:0x00
    DataPacketHeader[26]= CES_CMDIF_PKT_STOP_2 ;   // Packet footer2:0x0B
   
    for (int i = 0; i < 27; i++)
    {
      Serial.write(DataPacketHeader[i]);     // transmit the data over USB
    } 
  }
}