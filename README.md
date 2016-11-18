ADS1292R ECG/RESPIRATION SHIELD 
================================

[![ECG](https://www.protocentral.com/3059-large_default/ads1292r-ecgrespiration-shield-v2.jpg)  
*ADS1292R ECG/Respiration Shield for Arduino- v2 (PC-4128)*](https://www.protocentral.com/arduino-shields/818-ads1292r-ecgrespiration-shield-v2.html)

[![bob](https://www.protocentral.com/2729-thickbox_default/ads1292r-ecgrespiration-breakout-board.jpg)  
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

 Unzip the ZIP file and it should result in a folder called "ads1292r_shield". Copy this folder into your Arduino IDE’s libraries folder. Depending on your operating system, this could be either within your Arduino IDE’s foldesualizing r or in  your documents folder. For more information, please check http://arduino.cc/en/Guide/Libraries
 
Running the Example sketch
--------------------------
 If you have correctly installed the libraries, the example sketeches should now be available from within Arduino. Open up    your Arduino IDE and select ads1292r>examples>ads_1292r_breakout_brainbay to open the ECG displaying example for visualization use brainbay freeware(explained below), and  ads1292r>example>Xively_writeYUN7_BPMfiltered_avg to open the   Xively iot HR display example. 

This sketch is designed to read the data from the electrodes in real-time and stream them thorugh the UART to a receiving software. 


Using Processing - Data Visualisation Software (New!)
-----------------------------------------------------
 Processing is a data visualisation software, in existence since 2001, used by artists and scientists alike. Its an open source coding framework based on Java. If you are familiar with the Arduino environment, the Processing IDE is similar and you wont have much of a learning curve to climb!
 
 The following are the steps to run the code:

### 1.Download Processing for your operating system

 Download the processing ide latest version from the link

* [MAC] (http://download.processing.org/processing-3.1.1-macosx.zip)
* [Linux 32-bit] (http://download.processing.org/processing-3.1.1-linux32.tgz)
* [Linux 64-bit] (http://download.processing.org/processing-3.1.1-linux64.tgz)
* [Windows 32-bit] (http://download.processing.org/processing-3.1.1-windows32.zip)
* [Windows 64-bit] (http://download.processing.org/processing-3.1.1-windows64.zip)

 Once downloaded, unzip the archive and install the app as per your OS.

### 2. Download the Processing code for ECG visualisation

 a. Download the necessary files & directories or clone to your desktop from github.

 b. Unzipping the archive should make a folder by name ads1292r_shield that contains the visualisation code.

 c. Locate the Processing sketchbook directory on your computer. This should have been created automatically when you installed processing. Depending on your operating system, the path will be as follows:

* On Windows: c:/My Documents/Processing/
* On MAC: /Users/your_user_name/Documents/Processing/
* On Linux: /Home/your_user_name/sketchbook/

**Note:** This directory appears as "Processing" on Windows/Mac, and goes by the name "Sketchbook" on Linux. Create a subdirectory by name "libraries if one doesn't exist already.

 d. From the above mentioned "ads1292r_shield" directory Copy/Move the contents of the ADS1292rShield_Breakout/Processing/ces_view_ecg folder to the Processing sketchbook directory which is also mentioned above (Locate the Processing sketchbook)

 e. Finally, copy the controlP5 and G4P directories from ADS1292rShield_Breakout\Processing\libraries and paste them into the libraries directory of your Processing sketchbook.

 f. You are all set now to get your first ECG reading visualised from the ADS1292R shield!

### 3. Open Processing & launch the ces_view_ecg

 a. If Processing IDE was open, close it and reopen to refresh the libraries and sketches. The repositories are not refreshed if the IDE was open while the Sketches and Libraries were being updated.

 b. Double-click any of the .pde files in the ces_view_ecg directory to open all of the ecg code in the Processing IDE.

 c. If everything done so far was good, clicking the "run" button on the top left corner of the IDE, should run the code! If it does not, make sure you installed your libraries correctly.

 d. Once the GUI is running, select the port connect with ECG from the "SELECT PORT" dropdown as shown in the figure below

![Port Selection](https://github.com/Protocentral/ADS1292rShield_Breakout/blob/master/Processing/Final-Output/Port-Selection.png)

 e. Once the port selection is appropriate the START button gets enabled. Click "START" to initiate visualisation

 f. You should see the ECG wave generated with the values obtained from the ADS1292rShield Breakout Board as shown below.

![ECG Form in Processing](https://github.com/Protocentral/ADS1292rShield_Breakout/blob/master/Processing/Final-Output/ECG-Generated.png)

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
===================

This product is open source! Both, our hardware and software are open source and licensed under the following licenses:

Hardware
---------

**All hardware is released under [Creative Commons Share-alike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/).**
![CC-BY-SA-4.0](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

Software
--------

**All software is released under the MIT License(http://opensource.org/licenses/MIT).**

Please check [*LICENSE.md*](LICENSE.md) for detailed license descriptions.