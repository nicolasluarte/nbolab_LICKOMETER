/******************************************************************
Operant Licking Behavior Interface v3
11/11/2019
FEATURES: 
[1] This is a general purpose interface program that can take different programs from arduino as inputs
[2] Takes a message from Arduino coded as: H,lick,flag,lick,flag,lick,flag,
[3] Records separately a raw data file with the arduino data and one with licks and events
[4] Processing needs to start the data reception from Arduino
where: s
   - H is the header
   - [lick, flag]  is whether there is a lick in the drinking tube and whether that lick triggered a 
                   reward delivery. The position of the pair in the array indicates 
   
CHANGES FROM VERSION 2
1.- Use of checboxes on menu. Simpler code to mantain dataset
2.- Only saves event data, not raw lick data
3.- Corrects bug that was not allowing sensors to turn off when needed
Written by Claudio Perez-Leighton.  
*******************************************************************/
// LIBRARIES
import processing.serial.*;
import controlP5.*;
import java.util.Arrays; 

// PRE-COMPILED CLASSES
class Checkbox {
  int xpos, ypos;
  boolean b;
  Checkbox(){
    b = false;
  }
  void render(int _xpos, int _ypos){
    xpos = _xpos;
    ypos = _ypos;    
    fill(150);
    stroke(0);
    rect(xpos, ypos, 20, 20);
    if(b){
      line(xpos, ypos, xpos+20, ypos+20);
      line(xpos, ypos+20, xpos+20, ypos);
    }
    fill(0);    
  }
  void click(){
    if(isOver()){
      b=!b;
    }
  }
  boolean isOver(){
    return(mouseX>xpos&&mouseX<xpos+20&&mouseY>ypos&&mouseY<ypos+20);
  }
}

class Button {
  int xpos, ypos;
  int xdim, ydim;
  boolean b;
  Button(){
    b = false;
  }
  void render(int _xpos, int _ypos, int _xdim, int _ydim){
    xpos = _xpos;
    ypos = _ypos;  
    xdim = _xdim;
    ydim = _ydim;  
    fill(150, 10,10);
    stroke(0);
    rect(xpos, ypos, xdim, ydim);
    if(isOver()){
      fill(10, 150,10);
      rect(xpos, ypos, xdim, ydim);
      }  
    fill(0);  
    }
  void click(){
    if(isOver()){
      b=!b;
    } 
  }  
  boolean isOver(){
    return(mouseX>xpos&&mouseX<xpos + xdim &&mouseY>ypos&&mouseY<ypos + ydim);
  }
}
// VARIABLES
// Define variables for serial commmunication
Serial[] mySerialPorts = new Serial[4];
String[] myPorts; // Array to store serial ports  
int[] myPortsIndex; // array to store pointers for ports
int[] myArdIndex = new int[4]; // stores port information for each arduino, based on position within array.
int index = 0; // This is a check for the maximum number of arduinos the program can take
boolean configButtonOver = false; // Config Menu
boolean pumpButtonOver = false; // Config Menu

// define arrays to store touch information from each drinking tube - This is the information that goes into the raw data file
int[][] dtubeTouchCurrent = new int[4][3]; // store current status of whether the drinking tube is being touched
int[][] dtubeTouchLast = new int[4][3]; // store last status of whether drinking tube changed
int[][] dtubeFlagCurrent = new int [4][3]; // store current value of event trigger info by arduino
int[][] dtubeFlagLast = new int [4][3]; // store last value of event trigger info by arduino changed

// define arrays to store licks and event information from each drinking tube - This is the information that goes into the Events data file
int[][] dtubeLicks = new int[4][3]; // store real time drinking tube licks - Licks are defined based on when the 
int[][] dtubeEvents = new int[4][3]; // store cumulative drinking tube touches

// Variables for message decoding from arduino
char HEADER = 'H';    // character to identify the start of a message
char HEADER_START = 'P';    // character to identify the start of a message
short LF = 10;        // ASCII linefeed
PFont f;              // Declare PFont variable

// output for file
PrintWriter outputEvents;

// Variables for control of experiment
int[] pause = {1,1,1,1}; // One for each arduino

// Date and time values
String startDate = day() + "/" + month() + "/" + year() + " - " + hour() + ":" + minute() + ":" + second();
String currentDate;

// Create menus and variables for menu interaction
int currentMenu = 1;
String[] menusList = {"MENU PRINCIPAL", // mainMenu
                      "CONFIGURACION PUERTOS", // portMenu 
                      "CONFIGURACION LICOMETROS", // configMenu 
                      "RECOLECCION DE DATOS"}; // settingsMenu
// Variables for draingTest menu references
int[] xRefTestMenu = {10,510,10,510};
int[] yRefTestMenu = {10,10,300,300};
int[] xRefConfigMenu = {250,640,250,640};
int[] yRefConfigMenu = {10,10,270,270};
boolean[] overButtonFlag = {false, false, false, false};

// Create interactive variables
Checkbox[] portBox = new Checkbox[10]; // This implies that up to 10 ports can be drawn
Button[] buttonTest = new Button[10];
Button[][] buttonTimeAssign = new Button[4][3];

Checkbox[][] configActiveBox = new Checkbox[4][3]; // This implies that up to 10 ports can be drawn
ControlP5 cp5; // you always need the main class
int startTime, endTime; // variables to store current time for end and beggining of dropdown menu
boolean [][] spoutActive = new boolean[4][3]; // 0 is inactive, 1 is active
int[][][] timeActive = new int[4][3][2]; // arduino, port, start, end

// Variables to indicate that request to Arduino was sent to start data collection
boolean[] firstContact = {false, false, false, false};
boolean[] requestContact = {false, false, false, false};

void setup() {
   //FORMAT OPTIONS
   size(1030,590); // screen size
   f = createFont("Verdana",18,true); // Create Font 

  // List all the available serial ports
  myPorts = Serial.list();
  myPorts = Arrays.copyOfRange(myPorts, 25, 33);;
  myPortsIndex = new int[myPorts.length]; // This is just for reference display
  for(int i = 0; i < myPorts.length; i++) {
    myPortsIndex[i] = 999;
    }
    
  printArray(Serial.list()); // Print to console

  // Populate arrays for information to zero
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      dtubeTouchCurrent[i][j] = 0;
      dtubeTouchLast[i][j] = 0;
      dtubeFlagCurrent[i][j] = 0;
      dtubeFlagLast[i][j] = 0;
      dtubeLicks[i][j] = 0; 
      dtubeEvents[i][j] = 0;
      spoutActive[i][j] = false;
      }
    }

  // Create checkboxes
  for (int i = 0; i <= myPorts.length - 1; i++) { 
     portBox[i] = new Checkbox();
     buttonTest[i] = new Button();
    }
  for (int i = 0; i <= 3; i++) { 
    for (int q = 0; q <= 2; q++) {
     configActiveBox[i][q] = new Checkbox();
     buttonTimeAssign[i][q] = new Button();
     }
    } 
  
  // Create dropwdown list
  cp5 = new ControlP5(this);
  DropdownList dropListStart = cp5.addDropdownList("INICIO").setPosition(10, 50);
  DropdownList dropListEnd = cp5.addDropdownList("TERMINO").setPosition(120, 50);
  
  for (int i=0; i<= 24; i++) {
    String h = i + ":00";
    dropListStart.addItem(h, i);
    dropListEnd.addItem(h, i);
  }
  startTime = 0; // By default, all ports are active all day
  endTime = 0;

  // Populate arrays about activity
  for (int i = 0; i < 4; i++) { 
    for (int q = 0; q < 3; q++) {
     timeActive[i][q][0] = startTime; // default values
     timeActive[i][q][1] = endTime;    
     }
    } 
 
  // Create output files  
  String fileNameEvents = getTimeStamp() + "events_.txt";
  outputEvents = createWriter(fileNameEvents); 
  }

void draw() {
  // Update output
  outputEvents.flush();
  
  hideList();  
  switch(currentMenu) {
    case 1: mainMenu(); break;
    case 2: portMenu(); break;
    case 3: configMenu(); break;
    case 4: testMenu(); break;
  } 
}

// MAIN MENU 
void mainMenu() {
  background(255);
  fill(0);
  textAlign(CENTER, BOTTOM);  
  textSize(26);
  int yref = 50;
  text("SISTEMA DE ALIMENTACION OPERANTE", 515,50);
  line(10,55,1020,55); // prettier display
  textAlign(LEFT, BOTTOM); text("Presione los numeros para acceder a menus", 100, yref + 50);
  textSize(20);
  textAlign(LEFT, BOTTOM);
                      
  for (int i = 1; i <= menusList.length; i++) {
    String s = "[" + i + "] " + menusList[i-1];
    text(s, 100, yref + 50 + i * 50);
    }
}

// PORT MENU
void portMenu() { 
  background(255);
  fill(0);
  
  // x and y reference values for drawing
  int xref = 150; 
  int yref = 100;
  
  // MENU TITLE
  textSize(26); textAlign(CENTER, CENTER);
  text("Seleccione puertos seriales", 515,20);
  String s = "Seleccione un puerto usando el checkbox gris y despues use el boton rojo para verificar conexion";
  fill(150); rect(600,50,300,100);
  fill(0); textSize(18); text(s, 600, 50, 300, 100);  // Text wraps within text box
  
  // Display port information and checkboxes
  textSize(20); textAlign(LEFT, BOTTOM);
  for (int i = 0; i <= myPorts.length - 1; i++) { 
    portBox[i].render(xref - 60, (yref - 20) + i * 40);
    buttonTest[i].render(xref - 30, (yref - 20) + i * 40, 20,20);
    fill(0);
    textAlign(LEFT, BOTTOM); text(myPorts[i], xref, yref + i * 40);
   
    // If clicked, add port to the active list
    if (portBox[i].b && myPortsIndex[i] == 999) {
      myArdIndex[index] = i;
      myPortsIndex[i] = index;    
      mySerialPorts[index] = new Serial(this, myPorts[i], 9600);
      index++;     
      }  
    }
}

// CONFIG MENU
void configMenu() { 
  background(255);
  fill(0);
  textFont(f,18); // Specify font to be used
  textAlign(CENTER, CENTER);

  // If a port has been selected
  if (index != 0) {
    text("SELECCION HORAS", 100, 30); // Title 
    // Display status of Arduinos
    for (int i = 0; i < index; i++) {
      drawConfigPanel(i, Serial.list()[myArdIndex[i]], xRefConfigMenu[i], yRefConfigMenu[i]); // First argument indicates number of arduino 
      }
      
    // Create button to submit data to all Arduinos
    configButtonOver = overButton(10, 210, 150, 200);    
    if (!configButtonOver) {
        fill(150, 10,10); rectMode(CORNER); rect(10, 150, 200,50); fill(0);       
        } else {
          fill(10, 150,10); rectMode(CORNER); rect(10, 150, 200,50); fill(0);
          }
   textAlign(CENTER);text("ENVIAR CONFIGURACION",5,150, 200, 75);
        
   // Create button to test all pumps
   pumpButtonOver = overButton(10, 210, 225, 275);      
   if (!pumpButtonOver) {
        fill(150, 10,10); rectMode(CORNER); rect(10, 225, 200,50); fill(0);       
        } else {
          fill(10, 150,10); rectMode(CORNER); rect(10, 225, 200,50); fill(0);
          }
   textAlign(CENTER);text("PROBAR BOMBAS",5,240, 225, 75);
    } else {
      text("ASIGNE PUERTOS PARA COMENZAR", width / 2, height / 2);      
      }
}

// TEST MENU  
void testMenu() {  
    background(255);
  fill(0);
  textFont(f,18); // Specify font to be used
  textAlign(CENTER, CENTER);

  // If a port has been selected
  if (index != 0) {
    // Send info to each arduino about time active - display status of Arduinos
    for (int ardIndex = 0; ardIndex < index; ardIndex++) {
      // If first contact has been established, send information about whether port is active to Arduino    
      if (firstContact[ardIndex]) { 
        // Update active status of spouts
        for (int i = 0; i <= 2; i++) { setActiveStatus(ardIndex, i); }
        char f = setActiveFlag(spoutActive[ardIndex][0],
                               spoutActive[ardIndex][1],
                               spoutActive[ardIndex][2]);
                           
          mySerialPorts[ardIndex].write(f); // HEADER FOR MESSAGE
          } 
    // Get information about panels  
    drawInfoPanel(ardIndex, Serial.list()[myArdIndex[ardIndex]], xRefTestMenu[ardIndex], yRefTestMenu[ardIndex]); // First argument indicates number of arduino  
    }  
  } else {
      text("ASIGNE PUERTOS PARA COMENZAR", width / 2, height / 2);      
      }
}

// METHODS
// Create config display
void drawConfigPanel(int ardIndex, String Port, int xref, int yref) {
  // Print basic information
  
  fill(220); // grey color for background
  textFont(f,18); // Specify font to be used
  textAlign(LEFT, BOTTOM);
  rect(xref, yref, 380, 250); // Create rectangle to contain information
  fill(0); 
  text("ARDUINO " + ardIndex, xref + 10,yref + 20); // Include port
  text(Port,xref + 10,yref + 40); // Include port
  text("INICIO", xref+225, yref + 120); // Titles of times
  text("FIN", xref+300, yref + 120); // Titles of times
  
  // Draw checkboxes and buttons
  for (int i = 0; i < 3; i++) {
    int q = i + 1;
    if (i == 1){
      fill(0); textAlign(LEFT, BOTTOM); text("PLACA RESET", xref + 10, yref + 150 + i * 30); // Titles of bebederos
    }
    else{
    fill(0); textAlign(LEFT, BOTTOM); text("BEBEDERO " + q, xref + 10, yref + 150 + i * 30); // Titles of bebederos
    }
    configActiveBox[ardIndex][i].render(xref + 150, (yref - 20) + 150 + i * 30);
    buttonTimeAssign[ardIndex][i].render(xref + 180, (yref - 20) + 150 + i * 30, 20,20);
    
    // Print selected times to screen
    text(timeActive[ardIndex][i][0] + ":00", xref+225, yref + 150 + i * 30);  
    text(timeActive[ardIndex][i][1] + ":00", xref+300, yref + 150 + i * 30); 
  }
}

// Create basic display
void drawInfoPanel(int ardIndex, String Port, int xref, int yref) {
  
  // Print basic information
  fill(220); // grey color for background
  textAlign(LEFT, BOTTOM);
  textFont(f,18); // Specify font to be used
  rect(xref, yref, 480, 275); // Create rectangle to contain information
  fill(0); text("ARDUINO " + ardIndex, xref + 10,yref + 20); // Include port
  fill(0); text(Port,xref + 10,yref + 40); // Include port
  
  // Change font to print information
  textFont(f,16); // Specify font to be used

  // Put start and current date
  text("Inicio:  " + startDate, xref + 25, yref + 60);
  text("Actual: " + day() + "/" + month() + "/" + year() + " - " + hour() + ":" + minute() + ":" + second(), xref + 25, yref + 80);
  
  // Check status of experiment
  String status = "recolectando datos...";
  fill(10,100,27);
  if (firstContact[ardIndex] == false) { fill(127,0,0); status = "NO INICIADO..."; }
  if (firstContact[ardIndex] == false && requestContact[ardIndex] == true) { fill(127,0,0); status = "CONECTANDO..."; }
  if (pause[ardIndex] == 1 && firstContact[ardIndex] == true) { fill(127,0,0); status = "PAUSA..."; }
  text("Estado: " + status, xref + 25, yref + 100);
  fill(0);
  // Put information about experiment
  int yref2 = 150; // Create reference for all relative info

  // Titles
  fill(0);
  text("activo",xref + 150, yref + yref2);  
  text("señal",xref + 225, yref + yref2);  
  text("licks",xref + 320, yref + yref2);
  text("#eventos",xref + 400, yref + yref2);
  
  // Real time signal
  textFont(f,14); // Specify font to be used
  int[] ypos = {yref + yref2 + 25, yref + yref2 + 45, yref + yref2 + 65};
  for (int i = 0; i < 3; i++) {
    int q = i + 1;
    if (i == 1){
      fill(0); textAlign(LEFT, BOTTOM); text("PLACA RESET", xref + 10, yref + 150 + i * 30); // Titles of bebederos
    }
    else{
      fill(0); textAlign(LEFT, BOTTOM); text("BEBEDERO " + q, xref + 10, yref + 150 + i * 30); // Titles of bebederos
    }
    
    // Activate traffic light index for activity
    int s = 0; if (spoutActive[ardIndex][i]) { s = 1; }
    color col1 = trafficLight(s); // color for signal
    fill(col1); circle(xref + 175, ypos[i]-6, 10); // create signal
    
    // Activate traffic light index for real time data
    color col2 = trafficLight(dtubeTouchCurrent[ardIndex][i]); // color for signal
    fill(col2); circle(xref + 250, ypos[i]-6, 10); // create signal
    }
 
  // Cumulative licks
  fill(0); for (int i = 0; i < 3; i++) { text(dtubeLicks[ardIndex][i],xref + 325, ypos[i]); }
  
  // Events triggered
  fill(0); for (int i = 0; i < 3; i++) { text(dtubeEvents[ardIndex][i], xref + 425, ypos[i]); }
  
  // Line to separate options
  line(xref + 10,ypos[2] + 10,xref + 480,ypos[2] + 10);

// OPTIONS FOR CONTROL
  // START-PAUSE EXPERIMENT
  fill(80,80,100); rect(xref + 175, ypos[2] + 20, 150,20); fill(200,0,0); text(" START / PAUSA", xref + 190, ypos[2]+38);
  // Check if mouse is over button area
  overButtonFlag[ardIndex] = overButton(xref + 175, xref + 325, ypos[2] + 25, ypos[2] + 45);
  if (overButtonFlag[ardIndex]) { 
      // Change colors of button is mouse moves over       
      fill(10,80,10); rect(xref + 175, ypos[2] + 20, 150,20);
      fill(255); text(" START / PAUSA", xref + 190, ypos[2]+38);
      }   
}

// This function reads the event from serial Port and catches the information
void serialEvent(Serial p) {   
  int[] sensorInt = new int[6]; // initialize array - fthere are three pairs of [lick, flag] 
  String message = p.readStringUntil(LF); // read serial data
  if(message != null) {   
    print(message);
    String [] data  = message.split(","); // Split the comma-separated message. This should have eight characters to be a valid message
    if(data[0].charAt(0) == HEADER && data.length == 8) {      // check for header character in the first field       
      for( int i = 1; i < data.length-1; i++) { // skip the header and terminating cr and lf
             int q = i - 1;
             sensorInt[q] = Integer.parseInt(data[i]);   
            }      

      int ardIndex = 0; String flag = "0";                  
      if (p == mySerialPorts[0] && pause[0] == 0 && firstContact[0] == true) { ardIndex = 0; flag = "1"; }
      if (p == mySerialPorts[1] && pause[1] == 0 && firstContact[1] == true) { ardIndex = 1; flag = "1"; }
      if (p == mySerialPorts[2] && pause[2] == 0 && firstContact[2] == true) { ardIndex = 2; flag = "1"; }
      if (p == mySerialPorts[3] && pause[3] == 0 && firstContact[3] == true) { ardIndex = 3; flag = "1"; }
  
      // This flag indicates that data should be collected
      if(flag == "1") {
          int[][] index = { {0,2,4},   // Index to iterate over licks - columns are sensor per arduino
                            {1,3,5} }; // Index to iterate for intake events
 
        for(int i = 0; i < 3; i++) {
          int touchIndex = index[0][i]; // position within message for touch status
          int flagIndex = index[1][i]; // position within message for flag status
          dtubeTouchCurrent[ardIndex][i] = sensorInt[touchIndex]; // get current touch status
          dtubeFlagCurrent[ardIndex][i] = sensorInt[flagIndex]; // get current touch status
          
          /* UPDATE TOUCH STATUS AND NUMBER OF LICKS */
          int licksFlag = 0;
          // Check whether status of touch changed - A touch is indicated by 1. Therefore, a new lick implies a change from 0 -> 1
          if (dtubeTouchCurrent[ardIndex][i] == 1 && dtubeTouchLast[ardIndex][i] == 0) {
            dtubeLicks[ardIndex][i]++; // Increase lick counter by 1
            dtubeTouchLast[ardIndex][i] = 1; // update status of last touch
            licksFlag = 1;
            }
            
          // If touch status changed from 1 -> 0, update status of last touch
          if(dtubeTouchCurrent[ardIndex][i] == 0 && dtubeTouchLast[ardIndex][i] == 1) {
            dtubeTouchLast[ardIndex][i] = 0;
            }
          
          /* UPDATE FLAG STATUS AND NUMBER OF EVENTS */
          int eventsFlag = 0;
           // Check whether status of flag changed - An triggered event is indicated by 1. Therefore, a new event implies a change from 0 -> 1
          if (dtubeFlagCurrent[ardIndex][i] == 1 && dtubeFlagLast[ardIndex][i] == 0) {
            dtubeEvents[ardIndex][i]++; // Increase event counter by 1
            dtubeFlagLast[ardIndex][i] = 1; // update status of last touch
            eventsFlag = 1;
            }
            
          // If flag status changed from 1 -> 0, update status of last flag
          if(dtubeFlagCurrent[ardIndex][i] == 0 && dtubeFlagLast[ardIndex][i] == 1) {
            dtubeFlagLast[ardIndex][i] = 0;
            }
                                   
          // Print Events data file - This is the processed information
          if (licksFlag == 1 || eventsFlag == 1) {
            String infoLineEvents = getTimeStamp() + " - " + ardIndex + "," + i +  "," + dtubeLicks[ardIndex][i] + "," + dtubeEvents[ardIndex][i];
            outputEvents.println(infoLineEvents); // Write the raw data to the file
             eventsFlag = 0;
             licksFlag = 0;
            }
        }
        } 
      }     
    }
  }   
  

// Get current date and time
String getTimeStamp() {  
  String timestamp = year() + nf(month(),2) + nf(day(),2) + "-"  + nf(hour(),2) + nf(minute(),2) + nf(second(),2) + "-" + nf(millis() / 1e3,1,0);
  return(timestamp);
}

// Return color for traffic light indicator based on values 0: red, 1: green
color trafficLight(int i) {
// Create colors
color redCol = color(255, 0, 0);
color greenCol = color(0, 255, 0);
color yellowCol = color(255, 255, 0);  
color c = yellowCol;
  switch(i) {
    case 0: 
      c = redCol;         
    break;
    case 1: 
      c = greenCol;
    break;
    }  
return(c);    
}

void exit() {
  outputEvents.flush();
  outputEvents.close();
  super.exit();
}

void keyReleased() {
  // Check for menu
  if(key == '1') { currentMenu = 1; }
  if(key == '2') { currentMenu = 2; }
  if(key == '3') { currentMenu = 3; }
  if(key == '4') { currentMenu = 4; }
  } 
  
boolean overButton(int _xref1, int _xref2, int _yref1, int _yref2) {
   if(mouseX > _xref1 && mouseX < _xref2 &&
      mouseY > _yref1 && mouseY < _yref2) {
        return(true); 
      } else {
         return(false); 
      }        
}
  
// Hides dropdown lists if menu is not 3 and no ports have been asigned  
void hideList() {
  if (currentMenu == 3 && index != 0) {
    cp5.show();
    }
  else {
    cp5.hide();    
  }
}

 
// Check active status against check and time
void setActiveStatus(int ardIndex, int spoutIndex) {
  if (configActiveBox[ardIndex][spoutIndex].b) {
    // Get start and end time             
    int hStart = timeActive[ardIndex][spoutIndex][0];
    int hEnd = timeActive[ardIndex][spoutIndex][1];
    int current = hour();        
  
    if (hStart <= current & current < hEnd) { 
      spoutActive[ardIndex][spoutIndex] = true;
      } else {
       spoutActive[ardIndex][spoutIndex] = false;
      }
  } else { spoutActive[ardIndex][spoutIndex] = false; }
}
// ControlEvent monitors clicks on the gui
void controlEvent(ControlEvent theEvent) {
  if(theEvent.isController()) { 
    if(theEvent.getController().getName() == "INICIO") { startTime = int(theEvent.getController().getValue()); }
    if(theEvent.getController().getName() == "TERMINO") { endTime = int(theEvent.getController().getValue()); }
    }
}

char setConfigFlag(Checkbox c1, Checkbox c2, Checkbox c3) {
 char f = '0';
 if (!c1.b && !c2.b && !c3.b) { f = '0'; }
 if (c1.b && !c2.b && !c3.b) { f = '1'; }
 if (!c1.b && c2.b && !c3.b) { f = '2'; }
 if (!c1.b && !c2.b && c3.b) { f = '3'; }
 if (c1.b && c2.b && !c3.b) { f = '4'; }
 if (c1.b && !c2.b && c3.b) { f = '5'; }
 if (!c1.b && c2.b && c3.b) { f = '6'; }
 if (c1.b && c2.b && c3.b) { f = '7'; }
 return(f);
}

char setActiveFlag(boolean c1, boolean c2, boolean c3) {
 char f = '0';
 if (!c1 && !c2 && !c3) { f = '0'; }
 if (c1 && !c2 && !c3) { f = '1'; }
 if (!c1 && c2 && !c3) { f = '2'; }
 if (!c1 && !c2 && c3) { f = '3'; }
 if (c1 && c2 && !c3) { f = '4'; }
 if (c1 && !c2 && c3) { f = '5'; }
 if (!c1 && c2 && c3) { f = '6'; }
 if (c1 && c2 && c3) { f = '7'; }
 return(f);
}   
   
void mousePressed(){
  // PORT MENU
  if (currentMenu == 2) {
    for(int i=0; i< myPorts.length; i++){
      if (!portBox[i].b) {
          portBox[i].click();
        }
      }
      
    // Iterate over buttons and test
      for(int i=0; i< myPorts.length; i++){
       if(buttonTest[i].isOver() & portBox[i].b) {  
          int _index = myPortsIndex[i];    
          mySerialPorts[_index].write('T');         
         }
      }
    }

  // CONFIG MENU
  if (currentMenu == 3) {
    // Update start and end times
    for (int ardIndex = 0; ardIndex < index; ardIndex++) {
       for(int i=0; i < buttonTimeAssign[ardIndex].length; i++){
         // Activate checkbox
         configActiveBox[ardIndex][i].click();
          
          // Update Time active  
          if (buttonTimeAssign[ardIndex][i].isOver() && configActiveBox[ardIndex][i].b) {  
            timeActive[ardIndex][i][0] = startTime;
            timeActive[ardIndex][i][1] = endTime;
            }
          }
       }
       
      // Check if configuration has been sent
      if (configButtonOver) {
        for (int ardIndex = 0; ardIndex < index; ardIndex++) {
          char f = setConfigFlag( configActiveBox[ardIndex][0], 
                                  configActiveBox[ardIndex][1], 
                                  configActiveBox[ardIndex][2]);
          mySerialPorts[ardIndex].write(f);             
          }
        }
        
      // Test pumps and calibrate
      if (pumpButtonOver) {                  
        for (int ardIndex = 0; ardIndex < index; ardIndex++) {
          mySerialPorts[ardIndex].write('B');    

          }
      }    
  }
  
  // OPERATING MENU
  if (currentMenu == 4) {
    for (int ardIndex = 0; ardIndex < index; ardIndex++) {    
      if (overButtonFlag[ardIndex]) {
        // Start contact with Arduino and program
        if (!firstContact[ardIndex]) {
              requestContact[ardIndex] = true; // This is the signal to send data to Arduino to start collecting the experimentd
              pause[ardIndex] = 0;
             
             // Start test
             mySerialPorts[ardIndex].write('S');
             firstContact[ardIndex] = true;
             }   
         
         // Check for pause status
         if (firstContact[ardIndex]) {
           if (pause[ardIndex] == 0) {         
               pause[ardIndex] = 1;
             } else {            
               pause[ardIndex] = 0;
             }               
        }
      }
     }
  }
}
