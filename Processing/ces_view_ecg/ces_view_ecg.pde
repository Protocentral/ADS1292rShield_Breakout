//////////////////////////////////////////////////////////////////////////////////////////
//
//   GUI for controlling the ADS1292r Shield & Breakout
//
//   Created: Balasundari, May 2016
//
//   Requires g4p_control graphing library for processing.  Built on V4.1
//   Downloaded from Processing IDE Sketch->Import Library->Add Library->G4P Install
//
/////////////////////////////////////////////////////////////////////////////////////////

import g4p_controls.*;                       // Processing GUI Library to create buttons, dropdown,etc.,
import processing.serial.*;                  // Serial Library

// Java Swing Package For prompting message
import java.awt.*;
import static javax.swing.JOptionPane.*;

// File Packages to record the data into a text file
import javax.swing.JFileChooser;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.BufferedReader;

// Date Format
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

// General Java Package
import java.math.*;

/************** Packet Validation  **********************/
private static final int CESState_Init = 0;
private static final int CESState_SOF1_Found = 1;
private static final int CESState_SOF2_Found = 2;
private static final int CESState_PktLen_Found = 3;

/*CES CMD IF Packet Format*/
private static final int CES_CMDIF_PKT_START_1 = 0x0A;
private static final int CES_CMDIF_PKT_START_2 = 0xFA;
private static final int CES_CMDIF_PKT_STOP = 0x0B;

/*CES CMD IF Packet Indices*/
private static final int CES_CMDIF_IND_LEN = 2;
private static final int CES_CMDIF_IND_LEN_MSB = 3;
private static final int CES_CMDIF_IND_PKTTYPE = 4;
private static int CES_CMDIF_PKT_OVERHEAD = 5;

/************** Packet Related Variables **********************/

int ecs_rx_state = 0;                                        // To check the state of the packet
int CES_Pkt_Len;                                             // To store the Packet Length Deatils
int CES_Pkt_Pos_Counter, CES_Pkt_Data_Counter1, CES_Pkt_Data_Counter2;
int CES_Pkt_PktType;                                         // To store the Packet Type
char DataRcvPacket1[] = new char[10];                        // Receive the 1st channel raw data from the packet.
char DataRcvPacket2[] = new char[10];                        // Recieve the 2nd channel raw data from the packet.
int pSize = 1000;                                            // Total Size of the buffer
int arrayIndex = 0;                                          // Increment Variable for the buffer
float time = 0;                                              // X axis increment variable
float[] xdata = new float[pSize];                            // Buffer for time (x axis)
float[] ecgdata = new float[pSize];                          // Buffer for ecg (y axis)
float[] respdata = new float[pSize];                         // Buffer for respiration (y axis)
float[] bpmArray = new float[pSize];                         // Buffer for ecg to calculate bpm
float[] minArray = new float[pSize];                         // Buffer for hold minimum values
float[] maxArray = new float[pSize];                         // Buffer for hold maximum values

/************** User Defined Class Object **********************/

HelpWidget helpWidget;
HeaderButton headerButton;
Graph g, g1;
BPM hr;

/************** Graph Related Variables **********************/

double maxe, mine, maxr, minr;                         // To Calculate the Minimum and Maximum of the Buffer
double ecgVoltage=20;                                  // To store the current ecg value
double respirationVoltage=20;                          // To store the current respiration value
boolean startPlot = false;                             // Conditional Variable to start and stop the plot

/************** File Related Variables **********************/

boolean logging = false;                                // Variable to check whether to record the data or not
FileWriter output;                                      // In-built writer class object to write the data to file
JFileChooser jFileChooser;                              // Helps to choose particular folder to save the file
Date date, MPDate;                                      // Variables to record the date related values                              
BufferedWriter bufferedWriter;
DateFormat dateFormat;

/************** Port Related Variables **********************/

Serial port = null;                                     // Oject for communicating via serial port
String[] comList;                                       // Buffer that holds the serial ports that are paired to the laptop
boolean serialSet, portSelected;                        // Conditional Variable for connecting to the selected serial port
char inString = '\0';                                   // To receive the bytes from the packet
String selectedPort;                                    // Holds the selected port number

/*********************************************** Set Up Function *********************************************************/

////////////////////////////////////////////////////////////////////////////////
//
//  This Function is executed only once
//  The Objects for classes are initialized here
//
///////////////////////////////////////////////////////////////////////////////

/*************************************************************************************************************************/

public void setup() {
  // fullScreen();
  size(1000, 600, JAVA2D);

  /* G4P created Methods */

  createGUI();
  customGUI();

  /**********************/

  /* Object initialization for User-defined classes */

  headerButton = new HeaderButton(0, 0, width, 50);
  helpWidget = new HelpWidget(0, height - 30, width, 40); 
  hr = new BPM();
  g = new Graph(0, 100, width-50, 200);
  g1 = new Graph(0, 350, width-50, 200);
  g.GraphColor = color(0, 255, 0);
  g1.GraphColor = color(0, 255, 0);

  setChartSettings();                                    // graph function to set minimum and maximum for axis

  /*******  Initializing zero for buffer ****************/

  for (int i=0; i<pSize; i++) 
  {
    time = time + 1;
    xdata[i]=time;
    ecgdata[i] = 0;
    respdata[i] = 0;
  }
  time = 0;

  /*****************************************************/
}

/*********************************************** Draw Function *********************************************************/

////////////////////////////////////////////////////////////////////////////////
//
//  This Function is executed repeatedly according the Frame Refresh Rate  
//
///////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************************************/

public void draw() {
  background(0);

  // When Port is selected from the dropdown, 
  //    then this function is called to connect with the selected port.

  while (portSelected == true && serialSet == false)
    startSerial();

  // Auto scaling of the graph with Minimum And Maximum
  if ((maxe != g.yMax))
    g.yMax = (int)maxe+1;
  if ((maxr != g.yMax))
    g1.yMax = (int)maxr+1;
  if ((mine != g.yMin))
    g.yMin = (int)mine;
  if ((mine != g.yMin))
    g1.yMin = (int)minr;

  // When the port is connected and Start button is pressed, then this function is called
  // Method call to trace the graph

  if (startPlot)
  {
    g.LineGraph(xdata, ecgdata);
    g1.LineGraph(xdata, respdata);
  } else
    bpm1.setText("0");

  /****** User Defined Class Call ********/
  headerButton.draw();
  helpWidget.draw();
}

/*********** Method to connect with the selected or inputted port number  *************************/

void startSerial()
{
  try
  {
    port = new Serial(this, selectedPort, 57600);          // Used to connect with the port selected from the drop down
    port.clear();
    serialSet = true;
    start.setEnabled(true);
    start.setLocalColorScheme(GCScheme.GREEN_SCHEME);
    
  }
  catch(Exception e)
  {
    showMessageDialog(null, "Port is busy", "Alert", ERROR_MESSAGE);
    System.exit (0);
  }
}

/*********************************************** Serial Port Event Function *********************************************************/

///////////////////////////////////////////////////////////////////
//
//  Event Handler To Read the packets received from the Device
//
//////////////////////////////////////////////////////////////////

void serialEvent (Serial blePort) 
{
  inString = blePort.readChar();
  ecsProcessData(inString);
}

/*********************************************** Getting Packet Data Function *********************************************************/

///////////////////////////////////////////////////////////////////////////
//  
//  The Logic for the below function :
//      //  The Packet recieved is separated into header, footer and the data
//      //  If Packet is not received fully, then that packet is dropped
//      //  The data separated from the packet is assigned to the buffer
//      //  If Record option is true, then the values are stored in the text file
//
//////////////////////////////////////////////////////////////////////////

void ecsProcessData(char rxch)
{
  switch(ecs_rx_state)
  {
  case CESState_Init:
    if (rxch==CES_CMDIF_PKT_START_1)
      ecs_rx_state=CESState_SOF1_Found;
    break;

  case CESState_SOF1_Found:
    if (rxch==CES_CMDIF_PKT_START_2)
      ecs_rx_state=CESState_SOF2_Found;
    else
      ecs_rx_state=CESState_Init;                    //Invalid Packet, reset state to init
    break;

  case CESState_SOF2_Found:
    ecs_rx_state = CESState_PktLen_Found;
    CES_Pkt_Len = (int) rxch;
    CES_Pkt_Pos_Counter = CES_CMDIF_IND_LEN;
    CES_Pkt_Data_Counter1 = 0;
    CES_Pkt_Data_Counter2 = 0;
    break;

  case CESState_PktLen_Found:
    CES_Pkt_Pos_Counter++;
    if (CES_Pkt_Pos_Counter < CES_CMDIF_PKT_OVERHEAD)                        //Read Header
    {
      if (CES_Pkt_Pos_Counter==CES_CMDIF_IND_LEN_MSB)
        CES_Pkt_Len = (int) ((rxch<<8)|CES_Pkt_Len);
      else if (CES_Pkt_Pos_Counter==CES_CMDIF_IND_PKTTYPE)
        CES_Pkt_PktType = (int) rxch;
    } else if ( (CES_Pkt_Pos_Counter >= CES_CMDIF_PKT_OVERHEAD) && (CES_Pkt_Pos_Counter < CES_CMDIF_PKT_OVERHEAD+CES_Pkt_Len+1) )  //Read Data
    {
      if (CES_Pkt_PktType == 2)
      {
        if (CES_Pkt_Data_Counter1 < 4)
        {
          DataRcvPacket1[CES_Pkt_Data_Counter1]= (char) (rxch);             // Buffer to receive Digital Value for ECG
          CES_Pkt_Data_Counter1++;
        } else                                                                                    
        {
          DataRcvPacket2[CES_Pkt_Data_Counter2]= (char) (rxch);             // Buffer to receive Digital Value for Respiration
          CES_Pkt_Data_Counter2++;
        }
      }
    } else                                                                //All header and data received
    {
      if (rxch==CES_CMDIF_PKT_STOP)
      {     
        // Converting the ADC read value to get ECG and Respiration

        int data1 = ecsParsePacket(DataRcvPacket1, DataRcvPacket1.length-1);
        ecgVoltage = (double) (data1/Math.pow(10, 3));
        int data2 = ecsParsePacket(DataRcvPacket2, DataRcvPacket2.length-1);
        respirationVoltage = (double) (data2/Math.pow(10, 3));

        // Assigning the ECG and Respiration value to the respective buffer

        time = time+1;
        xdata[arrayIndex] = time;
        ecgdata[arrayIndex] = (float)ecgVoltage;
        respdata[arrayIndex]= (float)respirationVoltage;
        bpmArray[arrayIndex] = (float)ecgVoltage;
        maxArray[arrayIndex] = max(ecgdata);
        minArray[arrayIndex] = min(ecgdata);
        arrayIndex++;

        // When the buffer memory is used fully, then we have to replace the data with the existing data
        // Heart Rate is calculated by sending the buffer to BPM class.

        if (arrayIndex == pSize)
        {  
          arrayIndex = 0;
          time = 0;
          if (startPlot)
          {
            hr.bpmCalc(bpmArray);
          } else
          {
            bpm1.setText("0");
          }
        }       

        maxr = max(respdata);
        minr = min(respdata);

        maxe = max(ecgdata);
        mine = min(ecgdata);

        // Logging the data in a txt file

        if (logging == true)
        {
          try {
            date = new Date();
            dateFormat = new SimpleDateFormat("HH:mm:ss");
            output = new FileWriter(jFileChooser.getSelectedFile(), true);
            bufferedWriter = new BufferedWriter(output);
            bufferedWriter.write(dateFormat.format(date)+" : " +(ecgVoltage * 1.9604645E-08)+" V ");
            bufferedWriter.write(dateFormat.format(date)+" : " +(respirationVoltage * 1.9604645E-08)+" V ");
            bufferedWriter.newLine();
            bufferedWriter.flush();
            bufferedWriter.close();
          }
          catch(IOException e) {
            println("It broke!!!");
            e.printStackTrace();
          }
        }
        ecs_rx_state=CESState_Init;                                  // Setting to initial State
      } else                                                         // Packet end not found, drop packet
      {
        ecs_rx_state=CESState_Init;
      }
    }
    break;

  default:
    //Invalid state
    break;
  }
}

/*********************************************** Recursion Function To Reverse The data *********************************************************/

public int ecsParsePacket(char DataRcvPacket[], int n)
{
  if (n == 0)
    return (int) DataRcvPacket[n]<<(n*8);
  else
    return (DataRcvPacket[n]<<(n*8))| ecsParsePacket(DataRcvPacket, n-1);
}

/**************** Setting the Limits for the graph **************/

void setChartSettings() {

  g.xDiv=10;  
  g.xMax=pSize; 
  g.xMin=0;  
  g.yMax=10; 
  g.yMin=-10;

  g1.xDiv=10;  
  g1.xMax=pSize; 
  g1.xMin=0;  
  g1.yMax=10; 
  g1.yMin=-10;
}

/*************** Function to Calculate Average *********************/

double averageValue(float dataArray[])
{

  float total = 0;
  for (int i=0; i<dataArray.length; i++)
  {
    total = total + dataArray[i];
  }

  return total/dataArray.length;
}

/********************************************* User-defined Method for G4P Controls  **********************************************************/

///////////////////////////////////////////////////////////////////////////////
//
//  Customization of controls is done here
//  That includes : Font Size, Visibility, Enable/Disable, ColorScheme, etc.,
//
//////////////////////////////////////////////////////////////////////////////

public void customGUI() {
  comList = port.list();
  String comList1[] = new String[comList.length+1];
  comList1[0] = "SELECT THE PORT";
  for (int i = 1; i <= comList.length; i++)
  {
    comList1[i] = comList[i-1];
  }
  start.setEnabled(false);
  record.setEnabled(false);
  done.setEnabled(false);
  label1.setVisible(false);
  label2.setVisible(false);
  comList = comList1;
  portList.setItems(comList1, 0);
  done.setVisible(false);
  bpm1.setFont(new Font("Arial", Font.PLAIN, 105));
  bpm1.setLocalColor(2, color(255, 255, 255));
  label1.setFont(new Font("Arial", Font.PLAIN, 25));
  label1.setLocalColor(2, color(255, 255, 255));
  label2.setFont(new Font("Arial", Font.PLAIN, 25));
  label2.setLocalColor(2, color(255, 255, 255));
  start.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  record.setLocalColorScheme(GCScheme.CYAN_SCHEME);
}