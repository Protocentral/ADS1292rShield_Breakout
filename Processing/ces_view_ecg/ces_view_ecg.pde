import processing.serial.*;
import g4p_controls.*;
import java.awt.*;
import javax.swing.JFileChooser;

import java.math.*;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.util.Date;
import static javax.swing.JOptionPane.*;

import java.io.FileReader;
import java.io.BufferedReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

/************** Packet Validation  **********************/
private static final int CESState_Init = 0;
private static final int CESState_SOF1_Found = 1;
private static final int CESState_SOF2_Found = 2;
private static final int CESState_PktLen_Found = 3;

private static final int CESState_EOF_Wait = 98;
private static final int CESState_EOF_Found = 99;

/*CES CMD IF Packet Format*/
private static final int CES_CMDIF_PKT_START_1 = 0x0A;
private static final int CES_CMDIF_PKT_START_2 = 0xFA;
private static final int CES_CMDIF_PKT_STOP = 0x0B;

/*CES CMD IF Packet Indices*/
private static final int CES_CMDIF_IND_START_1 = 0;
private static final int CES_CMDIF_IND_START_2 = 1;
private static final int CES_CMDIF_IND_LEN = 2;
private static final int CES_CMDIF_IND_LEN_MSB = 3;
private static final int CES_CMDIF_IND_PKTTYPE = 4;
private static final int CES_CMDIF_IND_DATA0 = 5;
private static final int CES_CMDIF_IND_DATA1 = 6;
private static final int CES_CMDIF_IND_DATA2 = 7;

/* CES OTA Data Packet Positions */
private static int CES_OTA_DATA_PKT_POS_LENGTH = 0;
private static int CES_OTA_DATA_PKT_POS_CMD_CAT = 1;
private static int CES_OTA_DATA_PKT_POS_DATA_TYPE = 2;
private static int CES_OTA_DATA_PKT_POS_SENS_TYPE = 3;
private static int CES_OTA_DATA_PKT_POS_RSVD = 4;
private static int CES_OTA_DATA_PKT_POS_SENS_ID = 5;
private static int CES_OTA_DATA_PKT_POS_DATA = 6;

private static int CES_OTA_DATA_PKT_OVERHEAD = 6;
private static int CES_CMDIF_PKT_OVERHEAD = 5;

/************** Packet Related Variables **********************/

int ecs_rx_state = 0;
int CES_Pkt_Len;
int CES_Pkt_Pos_Counter, CES_Pkt_Data_Counter1, CES_Pkt_Data_Counter2;
int CES_Pkt_PktType;
char DataRcvPacket1[] = new char[1000];
char DataRcvPacket2[] = new char[1000];

/************** ControlP5 Related Variables **********************/

int colorValue;
HelpWidget helpWidget;
HeaderButton headerButton;
boolean visibility=false;

/************** Graph Related Variables **********************/

double maxe, mine, maxr, minr;
double ecgVoltage=20;
double respirationVoltage=20;

/************** File Related Variables **********************/

boolean logging = false;
FileWriter output;
JFileChooser jFileChooser;
Date date;
FileReader readOutput;
String line;
BufferedWriter bufferedWriter;
DateFormat dateFormat;

/************** Port Related Variables **********************/

Serial port = null;
int Ss = -1;
String[] comList;
boolean serialSet;
boolean portSelected = false;
String portName;
char inString = '\0';

/************** Logo Related Variables **********************/

PImage logo;

/************** General Variables **********************/

boolean startPlot = false, Serialevent = false;
String msgs;
int startTime = 0;

int pSize = 1000;
float[] xdata = new float[pSize];
float[] ecgdata = new float[pSize];
float[] respdata = new float[pSize];
float[] bpmArray = new float[pSize];
float[] minArray = new float[pSize];
float[] maxArray = new float[pSize];
int arrayIndex = 1;
Graph g, g1;
float time = 0;
BPM hr;

String selectedPort;

public void setup() {
  size(800, 480, JAVA2D);
  // fullScreen();
  createGUI();
  customGUI();

  headerButton = new HeaderButton(0, 0, width, 50);

  helpWidget = new HelpWidget(0, height - 30, width, 40); 
  g = new Graph(10, 100, width-20, 100);
  g1 = new Graph(10, 300, width-20, 100);

  setChartSettings();
  for (int i=0; i<pSize; i++) 
  {
    time = time + 1;
    xdata[i]=time;
    ecgdata[i] = 0;
  }
  time = 0;
  g.GraphColor = color(0, 255, 0);
  g1.GraphColor = color(0, 255, 0);
  hr = new BPM();
}

public void draw() {
  background(0);

  while (portSelected == true && serialSet == false)
  {
    startSerial();
  }
  if ((maxe != g.yMax))
  {
    g.yMax = (int)maxe+1;
  }
  if ((maxr != g.yMax))
  {
    g1.yMax = (int)maxr+1;
  }
  if ((mine != g.yMin))
  {
    g.yMin = (int)mine;
  }
  if ((mine != g.yMin))
  {
    g1.yMin = (int)minr;
  }
  if (startPlot)
  {
    g.LineGraph(xdata, ecgdata);
    g1.LineGraph(xdata, respdata);
  } else
  {
    bpm1.setText("0");
  }
  headerButton.draw();
  helpWidget.draw();
}

/*********************************************** Opening Port Function ******************************************* **************/

void startSerial()
{
  try
  {
    port = new Serial(this, selectedPort, 57600);
    port.clear();
    serialSet = true;
    msgs = "Port "+selectedPort+" is opened Click Start button";
    portName = "\\"+selectedPort+".txt";
  }
  catch(Exception e)
  {
    msgs = "Port "+selectedPort+" is busy";
    showMessageDialog(null, "Port is busy", "Alert", ERROR_MESSAGE);
    System.exit (0);
  }
}

/*********************************************** Serial Port Event Function *********************************************************/

void serialEvent (Serial blePort) 
{
  Serialevent = true;
  inString = blePort.readChar();
  ecsProcessData(inString);
}

/*********************************************** Getting Packet Data Function *********************************************************/

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
      ecs_rx_state=CESState_Init;
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
    if (CES_Pkt_Pos_Counter < CES_CMDIF_PKT_OVERHEAD)  //Read Header
    {
      if (CES_Pkt_Pos_Counter==CES_CMDIF_IND_LEN_MSB)
        CES_Pkt_Len = (int) ((rxch<<8)|CES_Pkt_Len);
      else if (CES_Pkt_Pos_Counter==CES_CMDIF_IND_PKTTYPE)
        CES_Pkt_PktType = (int) rxch;
    } else if ( (CES_Pkt_Pos_Counter >= CES_CMDIF_PKT_OVERHEAD) && (CES_Pkt_Pos_Counter < CES_CMDIF_PKT_OVERHEAD+CES_Pkt_Len+1) )  //Read Data
    {
      if (CES_Pkt_PktType == 2)
      {
        if (CES_Pkt_Data_Counter1 < 4)                                                            //Channel 1 for packet counter
        {
          DataRcvPacket1[CES_Pkt_Data_Counter1]= (char) (rxch);
          CES_Pkt_Data_Counter1++;
        } else                                                                                    //Channel 2 for original data
        {
          DataRcvPacket2[CES_Pkt_Data_Counter2]= (char) (rxch);
          CES_Pkt_Data_Counter2++;
        }
      }
    } else  //All header and data received
    {
      if (rxch==CES_CMDIF_PKT_STOP)
      {     

        int data1 = ecsParsePacket(DataRcvPacket1, DataRcvPacket1.length-1);
        ecgVoltage = (double) (data1/Math.pow(10,3));
        int data2 = ecsParsePacket(DataRcvPacket2, DataRcvPacket2.length-1);
        respirationVoltage = (double) (data2/Math.pow(10,3));

        time = time+1;
        xdata[arrayIndex] = time;
        ecgdata[arrayIndex] = (float)ecgVoltage;
        respdata[arrayIndex]= (float)respirationVoltage;
        bpmArray[arrayIndex] = (float)ecgVoltage;
        maxArray[arrayIndex] = max(ecgdata);
        minArray[arrayIndex] = min(ecgdata);

        arrayIndex++;

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
        ecs_rx_state=CESState_Init;
      } else
      {
        ecs_rx_state=CESState_Init;
      }
    }
    break;

  default:
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


void setChartSettings() {
  g.xLabel="";
  g.yLabel="";
  g.Title="";  
  g.xDiv=10;  
  g.xMax=pSize; 
  g.xMin=0;  
  g.yMax=10; 
  g.yMin=-10;

  g1.xLabel="";
  g1.yLabel="";
  g1.Title="";  
  g1.xDiv=10;  
  g1.xMax=pSize; 
  g1.xMin=0;  
  g1.yMax=10; 
  g1.yMin=-10;
}

double averageValue(float dataArray[])
{

  float total = 0;
  for (int i=0; i<dataArray.length; i++)
  {
    total = total + dataArray[i];
  }

  return total/dataArray.length;
}

// Use this method to add additional statements
// to customise the GUI controls
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
}