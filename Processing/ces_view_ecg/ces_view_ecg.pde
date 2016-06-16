import javax.swing.JFileChooser;
import controlP5.*;
import processing.serial.*;

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

ControlP5 controlP5;
DropdownList portList;
Textlabel graphTitle, bpm1;
Button recordButton, finishRecord, startButton, stopButton, closeButton;
int colorValue;
HelpWidget helpWidget;
HeaderButton headerButton;
boolean visibility=false;

/************** Graph Related Variables **********************/

double max, min;
double receivedVoltage=20;

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

boolean start = false, Serialevent = false;
String msgs;
int startTime = 0;

int pSize = 1000;
float[] xdata = new float[pSize];
float[] ydata = new float[pSize];
int arrayIndex = 1;
Graph g;
float time =0;
BPM hr;

/*********************************************** Set Up Function *********************************************************/

void setup()
{
  size(1200, 700);
  frameRate(100);
  //  fullScreen();
  smooth();

  /************** Adding Elements to the ControlP5 **********************/

  controlP5 = new ControlP5(this);
  PFont p = createFont("Arial", 18); 
  controlP5.setFont(p);

  startButton = controlP5.addButton("START")
    .setValue(255)
    .setPosition(250, 7)
    .setSize(100, 35);

  stopButton = controlP5.addButton("STOP")
    .setValue(255)
    .setPosition(250, 7)
    .setSize(100, 35)
    .setVisible(false);

  closeButton = controlP5.addButton("CLOSE")
    .setValue(255)
    .setPosition(500, 7)
    .setSize(100, 35);                     

  graphTitle = controlP5.addTextlabel("Graph1")
    .setText("Electrocardiogram")
    .setPosition(width/2-150, 65)
    .setColorValue(255)
    .setFont(createFont("Arial", 25));          

  recordButton = controlP5.addButton("RECORD")
    .setValue(255)
    .setPosition(380, 7)
    .setSize(100, 35);

  finishRecord = controlP5.addButton("DONE")
    .setValue(255)
    .setPosition(380, 7)
    .setSize(100, 35)
    .setVisible(false);

  bpm1 = controlP5.addTextlabel("BPM")
    .setPosition(width-300, 105)
    .setColorValue(255)
    .setFont(createFont("Arial", 105));                        

  controlP5.setColorBackground(color(23, 172, 6));
  controlP5.setColorActive(color(53, 249, 31));

  portList = controlP5.addDropdownList("Select_Port")
    .setLabel("SELECT PORT")
    .setPosition(5, 7)
    .setItemHeight(35)
    .setBarHeight(35)
    .close();

  colorValue = controlP5.getController("RECORD").getColor().getBackground();

  setLock(controlP5.getController("START"), true);
  setLock(controlP5.getController("STOP"), true);
  setLock(controlP5.getController("RECORD"), true);            

  /************** Creating Graph Class Objects **********************/


  date = new Date();
  logo = loadImage("logo.png");
  customize(portList);
  headerButton = new HeaderButton(0, 0, width, 50);

  helpWidget = new HelpWidget(0, height - 30, width, 40); 
  g = new Graph(30, 250, width-20, 400);

  setChartSettings();
  for (int i=0; i<pSize; i++) 
  {
    time = time + 1;
    xdata[i]=time;
    ydata[i] = 0;
  }
  time = 0;
  g.GraphColor = color(0, 255, 0);
  hr = new BPM();
}

/*********************************************** Draw Function *********************************************************/

void draw()
{
  while (portSelected == true && serialSet == false)
  {
    startSerial(comList);
  }
  background(0);
  headerButton.draw();
  helpWidget.draw();
  if (start)
  {
    g.LineGraph(xdata, ydata);
  }
  else
  {
    bpm1.setText("0");
  }
}

/*********************************************** Dropdown & graph Function *********************************************************/

void customize(DropdownList ddl)
{
  comList = port.list();
  if (comList.length == 0)
  {
    output("No Ports are Availabe");
  } else
  {
    portList.setSize(200, (comList.length * 25)+90);
    for (int j=0; j<comList.length; j++)
    {
      ddl.addItem(comList[j], j); 
      ddl.setColorBackground(color(23, 172, 6));
      ddl.setColorActive(color(0, 0, 0));
    }
  }
}

/*********************************************** ControlP5 Event Function *********************************************************/

public void controlEvent(ControlEvent theEvent)
{
  if (theEvent.isGroup())
  {
    float S= theEvent.group().getValue();
    Ss = int(S);
    portSelected = true;
  }
}

/*********************************************** Opening Port Function ******************************************* **************/

void startSerial(String[] theport)
{
  try
  {
    port = new Serial(this, theport[Ss], 57600);
    port.clear();
    serialSet = true;
    msgs = "Port "+comList[Ss]+" is opened Click Start button";
    portName = "\\"+comList[Ss]+".txt";
    setLock(controlP5.getController("START"), false);
    setLock(controlP5.getController("Select_Port"), true);
  }
  catch(Exception e)
  {
    msgs = "Port "+comList[Ss]+" is busy";
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
          // DataRcvPacket1[CES_Pkt_Data_Counter1]= (char) (rxch);
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

        int data2 = ecsParsePacket(DataRcvPacket2, DataRcvPacket2.length-1);
        receivedVoltage = (double) data2/(Math.pow(10, 3));
        //  println(receivedVoltage);
        time = time+1;
        xdata[arrayIndex] = time;
        ydata[arrayIndex] = (float)receivedVoltage;
        arrayIndex++;

        if (arrayIndex == pSize)
        {  
          arrayIndex = 0;
          time = 0;
          if (start)
          {
            hr.bpmCalc(ydata);
          }
          else
          {
              bpm1.setText("0");
          }
        }       

        max = max(ydata);
        min = min(ydata);

        if (max >= g.yMax)
        {
          g.yMax = (int)max;
        }
        if (min <= g.yMin)
        {
          g.yMin = (int)min;
        }
        if (logging == true)
        {
          try {
            date = new Date();
            dateFormat = new SimpleDateFormat("HH:mm:ss");
            output = new FileWriter(jFileChooser.getSelectedFile(), true);
            bufferedWriter = new BufferedWriter(output);
            bufferedWriter.write(dateFormat.format(date)+" : " +receivedVoltage);
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

/*********************************************** Dropdown Event Function *********************************************************/

public void Select_Port(int theValue)
{
  Ss = theValue;
  portSelected = true;
  output("Port "+comList[Ss]+" is selected");
}

/*********************************************** Button Event Function *********************************************************/

public void START(int theValue) {
  try {
    if (!start)
    {
      //   draw();
      startButton.hide();
      stopButton.show();
      stopButton.bringToFront();
      start = true;
      setLock(controlP5.getController("STOP"), false);
      setLock(controlP5.getController("RECORD"), false);
    }
  }
  catch(Exception e)
  {
  }
}

public void STOP(int theValue) {
  try {
    Ss = -1;
    start = false;
    stopButton.hide();
    startButton.show();
    startButton.bringToFront();
  }
  catch(Exception e)
  {
  }
}

public void RECORD() {
  try {
    recordButton.hide();
    setLock(controlP5.getController("START"), true);
    setLock(controlP5.getController("STOP"), true);
    setLock(controlP5.getController("CLOSE"), true);
    jFileChooser = new JFileChooser();
    jFileChooser.setSelectedFile(new File("log.txt"));
    jFileChooser.showSaveDialog(null);
    String filePath = jFileChooser.getSelectedFile()+"";
    if ((filePath.equals("log.txt"))||(filePath.equals("null")))
    {
      recordButton.show();
      recordButton.bringToFront();
      setLock(controlP5.getController("START"), false);
      setLock(controlP5.getController("STOP"), false);
      setLock(controlP5.getController("CLOSE"), false);
    } else
    {    
      finishRecord.show();
      finishRecord.bringToFront();
      logging = true;
      date = new Date();
      output = new FileWriter(jFileChooser.getSelectedFile(), true);
      bufferedWriter = new BufferedWriter(output);
      bufferedWriter.newLine();
      bufferedWriter.write(date+"");
      bufferedWriter.newLine();
      bufferedWriter.flush();
      bufferedWriter.close();
    }
  }
  catch(Exception e)
  {
  }
}

public void DONE() {
  try {
    if (logging == true)
    {
      showMessageDialog(null, "Log File Saved successfully");
      finishRecord.hide();
      recordButton.show();
      recordButton.bringToFront();
      setLock(controlP5.getController("START"), false);
      setLock(controlP5.getController("STOP"), false);
      setLock(controlP5.getController("CLOSE"), false);
      logging = false;
    }
  }
  catch(Exception e)
  {
  }
}
public void CLOSE() {
  try {

    if (visibility)
    {
      if (port != null)
      {
        port.stop();
        port = null;
        exit();
      } else
      {
        exit();
      }
    } else
      visibility = true;
  }
  catch(Exception e)
  {
  }
}

/*********************************************** Button Enable & Disable Function *********************************************************/

void setLock(Controller theController, boolean theValue) {
  theController.setLock(theValue);
  if (theValue) {
    theController.setColorBackground(color(220, 225, 242));
  } else {
    theController.setColorBackground(color(colorValue));
  }
}


void setChartSettings() {
  
  g.xDiv=10;  
  g.xMax=pSize; 
  g.xMin=0;  
  g.yMax=-200; 
  g.yMin=-50;
}