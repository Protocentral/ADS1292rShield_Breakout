ADS1292R ECG/RESPIRATION SHIELD 
================================

[![ECG](https://www.protocentral.com/3059-tm_thickbox_default/ads1292r-ecgrespiration-shield-v2.jpg)  
*ADS1292R ECG/Respiration Shield for Arduino- v2 (PC-4128)*](https://www.protocentral.com/arduino-shields/818-ads1292r-ecgrespiration-shield-v2.html)

[![bob](https://www.protocentral.com/2729-tm_thickbox_default/ads1292r-ecgrespiration-breakout-board.jpg)  
*ADS1292R ECG/Respiration Breakout for Arduino- v2 (PC-4116)*](https://www.protocentral.com/biomedical/783-ads1292r-ecgrespiration-breakout-board.html?search_query=ads12&results=3)

Easily monitor ECG and respiration using your Arduino with this plug-in shield. The version 2 of this product adds a new SPI pin header making it compatible with newer Arduino devices including the Arduino Yun and 3.5mm connector for the electrodes. 
We now include the electrodes and cable also with the shield
Just plug it into an Arduino and you're ready to go. The 3.5 mm circular connector provides an easy way to connect electodes to the shield. The other end of this cable has snaps for standard ECG electrodes. We also include a pakc of 10 disposable EG electrodes. It accepts two ECG electrodes and one Driven Right Leg (DRL) electrode for common mode noise reduction. 
Another interesting feature of this shield is that you can also measure the respiratory activity using the same two electrodes connected to the shield. The ADS1292R uses a method known as impedance pneumography to measure respiration using the changes in chest impedance caused during respiration. 


Features:
----------
* ADS1292R Analog Front End IC
* Onboard 3.3V voltage regulator for low noise
* Onboard logic level transalators for Arduino interface
* Prototyping area for adding addtional components

Includes:
----------
* ADS1292R Arduino shield
* Electrode cable with 3.5mm connector and ECG electrodes connectors
* Pack of 10 disposable stick-on ECG electrodes.

Repository Contents
-------------------
* **/Libraries** - Arduino library and example sketches.
* **/Hardware** - All Eagle design files (.brd, .sch)
* **/extras** - includes the datasheet

Connecting the shield to your Arduino
-------------------------------------
 Connect the ECG/Respiration shield to the Arduino by stacking it on top of your Arduino. This shield uses the SPI interface  to communicate with the Arduino. Since this includes the ICSP header, which is used on newer Arduinos for SPI communication,  this shield is also compatible newer Arduino boards such as the Arduino Yun and Due.
 
 ![ConnectingShield](http://www.protocentral.com/img/cms/ads1292r_shield/image002.png) 
 
Wiring the Breakout to your Arduino
------------------------------------
 If you have bought the breakout the connection with the Arduino board is as follows:
 
|ads1292r pin label| Arduino Connection   |Pin Function      |
|----------------- |:--------------------:|-----------------:|
| VDD              | +5V                  |  Supply voltage  |             
| PWDN/RESET       | D4                   |  Reset           |
| START            | D5                   |  Start Input     |
| DRDY             | D6                   |  Data Ready Outpt|
| CS               | D7                   |  Chip Select     |
| MOSI             | D11                  |  Slave In        |
| MISO             | D12                  |  Slave Out       |
| SCK              | D13                  |  Serial Clock    |
| GND              | Gnd                  |  Gnd             |
 
Installing the Arduino libraries 
---------------------------------
 The Arduino library contains the functions required to communicate with the ADS1292R ECG/Respiration Shield. Please follow   the following steps to get the library downloaded and working:
 Download the library ZIP file from the following link:

 https://github.com/Protocentral/ADS1292rShield_Breakout

 Unzip the ZIP file and it should result in a folder called "ads1292r_shield". Copy this folder into your Arduino IDE’s       libraries folder. Depending on your operating system, this could be either within your Arduino IDE’s foldesualizing r or in  your documents folder. For more information, please check http://arduino.cc/en/Guide/Libraries
 
Running the Example sketch
--------------------------
 If you have correctly installed the libraries, the example sketeches should now be available from within Arduino. Open up    your Arduino IDE and select ads1292r>examples>ads_1292r_breakout_brainbay to open the ECG displaying example for visualization use brainbay freeware(explained below), and  ads1292r>example>Xively_writeYUN7_BPMfiltered_avg to open the   Xively iot HR display example. 

This sketch is designed to read the data from the electrodes in real-time and stream them thorugh the UART to a receiving software. 

Using Brainbay
----------------
 Brainbay is an open-source application originally designed for bio-feedback applications, it can be easily used for          visualizing the ECG in real-time. Brainbay can be downloaded from the following link:
 https://github.com/Protocentral/ADS1292rShield_Breakout/tree/master/BrainbayFreeware
 You can use the Windows installer and follow the instructions on the screen to install Brainbay on your computer
 Brainbay now has to be configured to work for our data format and that can be achieved by the using the configuration file   https://github.com/Protocentral/ADS1292rShield_Breakout/tree/master/BrainbayFreeware
 In Brainbay, use Design>Load Design to load the configuration file that you downloaded in the previous link.
 Right click the EEG block in brainbay, and select the right COM port and baud rate 57600, press connect and then press play  (F7). In Brainbay, now the following are displayed


![ECGRealTimeDispaly](http://www.protocentral.com/img/cms/ads1292r_shield/image008.png)  

 Channel 1 (the red trace) shows the raw ECG obtained from the board
 Channel 2(the green trace) shows the filtered ECG which is run through several stages of filters.
 BPM is calculated by a threshold detector built into the Brainbay software.

The filtered ECG waveform (generated from an ECG simulator) from the output should look like this:

![FilteredECG](http://www.protocentral.com/img/cms/ads1292r_shield/Ecg_BB_plot.png)

Using Processing
----------------
 Processing is an open source creative coding framework based on Java. If you are familiar with the Arduino environment, the Processing IDE is nearly identical. 

**Note:** The code for ECG is executed using processing ide version 2.2.1 only.
 
 The following are the steps to run the code:

### 1.Download Processing for your operating system

 First, download the processing ide version 2.2.1 from the link

* MAC: "http://download.processing.org/processing-2.2.1-macosx.zip"
* Linux 32-bit: "http://download.processing.org/processing-2.2.1-linux32.tgz"
* Linux 64-bit: "http://download.processing.org/processing-2.2.1-linux64.tgz"
* Windows 32-bit: "http://download.processing.org/processing-2.2.1-windows32.zip"
* Windows 64-bit: "http://download.processing.org/processing-2.2.1-windows64.zip"

 Once it's finished downloading, unzip it and place the Processing .app or .exe where you typically place your applications or programs.

### 2. Download the ces_view_ecg Processing code

 a. Download the necessary files & directories or clone to your desktop from github.

 b. Unzip the download. It should be called ads1292r_shield after unzip/extract it.

 c. Locate the Processing sketchbook directory on your computer. This should have been created automatically when you installed processing. Depending on your operating system, this directory's path is:

* On Windows: c:/My Documents/Processing/
* On MAC: /Users/your_user_name/Documents/Processing/
* On Linux: /Home/your_user_name/sketchbook/

**Note:** this directory should be called "Processing" on Windows and Mac, and "Sketchbook" on Linux. This directory should already have a subdirectory called "libraries." If it does not, create the subdirectory.

 d. Now, from the ads1292r_shield directory that you downloaded and unzipped in parts (a) and (b) above, copy the ADS1292rShield_Breakout\Processing\ces_view_ecg directory and paste it in the Processing sketchbook directory that you located in part (c) above.

 e. Finally, copy the controlP5 & gwoptics directories from ADS1292rShield_Breakout\Processing\libraries and paste them into the libraries directory of your Processing sketchbook.

 f. Now everything is where it should be!

### 3. Open Processing & launch the ces_view_ecg

 a. If Processing is currently open, close it. The new libraries you added won't be recognized until you restart the application.

 b. Double-click any of the .pde files in the ces_view_ecg directory and all of the ecg code should open in the Processing IDE.

 c. Click the "run" button on the top left of the IDE, and the code should run! If it does not, make sure you installed your libraries correctly and that are using the version 2.2.1 of Processing.

 d. Once the GUI is running, select the port connect with ECG from the "SELECT PORT" dropdown.

![Processing Sketch](https://github.com/Protocentral/ADS1292rShield_Breakout/blob/master/Processing/Final-Output/ECG-Generated.png)

 e. Click "START" button after it is enabled.

 f. The ECG wave is generated with the values obtained from the ADS1292rShield/Breakout Board.

![Processing Sketch](Processing\Final-Output\ECG-Generated.png)

Connecting the ECG Electrodes
------------------------------
 A 3-electrode cable along with a standard stereo jack is provided along with the shield to connect the electrodes to the     shield. The electrode input connector is highlighted in the below picture.
 
 ![ConnectingTheElectrodes](http://www.protocentral.com/img/cms/ads1292r_shield/image006.png)
 
 The other side of the electrode connector would connect to snap-on electrodes attached to the body. For testing purposes,    you can use an ECG simulator to provide inputs to the board. 

 Warning:
 When connecting the electodes to the body, it is safer to disconnect the mains power source to the Arduino. For example, if  you are using the Arduino along with a laptop, disconnecting the battery charger from the laptop would be a safe option.
 
Placing the Electrodes on the body
---------------------------------
![Wearing the Electrode](https://www.protocentral.com/img/p/3/5/5/1/3551.jpg)

License Information
-------------------
This product is open source!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to Protocentral and release anything derivative under the same license.
