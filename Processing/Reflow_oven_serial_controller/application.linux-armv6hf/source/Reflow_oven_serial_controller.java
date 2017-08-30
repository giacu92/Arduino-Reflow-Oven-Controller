import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import g4p_controls.*; 
import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class Reflow_oven_serial_controller extends PApplet {

// G4P


// Comunicazione seriale


Serial myPort;

int left_byte = 100;
int right_byte = 10;

int startX = 60;
int stopY  = 50;
int stopX  = 736;
int startY = 370;
int stepX  = (stopX-50-startX)/12;
int stepY  = (startY-stopY-20)/6 ;

int X, Y, oldX, oldY = 0;

int TEMPERATURE_SOAK_MIN   = 150;
int TEMPERATURE_SOAK_MAX   = 177;
int TEMPERATURE_REFLOW_MAX = 230;
int TEMPERATURE_COOLDOWN   = 100;

Table table = new Table();
boolean connected = false;

public void setup()
{
  
  frameRate(10);
  createGUI();

  COM_list.setItems(Serial.list(), 0);
  labelX.setVisible(false);
  labelY.setVisible(false);
  
  table.addColumn("time", Table.INT);
  table.addColumn("temperature", Table.FLOAT);
}

public void draw()
{
  background(220);
  aggiornaInterfaccia();
  
  if (connected)  serialReceive();
}
  
public void serialReceive()
{
  try
  {
    if(myPort.available() > 0)
    {
      String[] parametri;
      String riga = myPort.readString(); // mi aspetto "(timeStamp,setpoint,input,output)"
      print(riga);
      
      int iniPac = riga.indexOf("(");
      int endPac = riga.indexOf(")");
      if(iniPac != -1 && endPac != -1 && endPac-iniPac > 1)
      {
        labelSerialReceive.setText("got: " + riga);
        
        riga = riga.substring(iniPac+1, endPac);
        parametri = split(riga, ','); // (timeStamp[0],setpoint[1],input[2],output[3])
        
        println("parametri[0] = " + parametri[0]);
        println("parametri[1] = " + parametri[1]);
        println("parametri[2] = " + parametri[2]);
        println("parametri[3] = " + parametri[3]);
        
        //Se ho ricevuto "  [L]" o "  [LF]" vado solo a modificare le temperature 
        if (parametri[0].equals(" [L]") || parametri[0].equals(" [LF]") || parametri[0].equals(" [C]"))
        {
          TEMPERATURE_SOAK_MIN   = Integer.valueOf(parametri[1]);
          TEMPERATURE_SOAK_MAX   = Integer.valueOf(parametri[2]);
          TEMPERATURE_REFLOW_MAX = Integer.valueOf(parametri[3]);
          
          tf_TEMP_SOAK_MIN.setText(Integer.toString(TEMPERATURE_SOAK_MIN));
          tf_TEMP_SOAK_MAX.setText(Integer.toString(TEMPERATURE_SOAK_MAX));
          tf_TEMP_REFL_MAX.setText(Integer.toString(TEMPERATURE_REFLOW_MAX));
          
          println("temperature_soak_min:" + TEMPERATURE_SOAK_MIN);
          println("temperature_soak_max:" + TEMPERATURE_SOAK_MAX);
          println("temperature_reflow_max:" + TEMPERATURE_REFLOW_MAX);
        }
        else //ho ricevuto i valori da plottare
        {
          labelX.setText(parametri[0] + " s" );
          labelY.setText(parametri[2] + " \u00b0C");
          
          //arrotondo i valori per piazzarli nel grafico e li aggiungo alla tabella
          int new_time = round(map(Integer.valueOf(parametri[0]), 0, 30, 0, stepX));
          float new_temp = round(map(Float.valueOf(parametri[2]), 0, 50, 0, stepY));
          
          table.addRow();
          int n = table.getRowCount();
          table.setInt(n-1, "time", new_time+startX+10);
          table.setFloat(n-1, "temperature", startY-new_temp);
        }
      }
    }
  }
  catch (Exception e) {println("exception");}
}
public void aggiornaInterfaccia()
{
  //Creo il grafico:
  int g_w = 700;
  int g_h = 500;
  PGraphics graficoProfiloSfondo = createGraphics(g_w, g_h);
  graficoProfiloSfondo.beginDraw();
    // AXIS:
    graficoProfiloSfondo.stroke(0);
    graficoProfiloSfondo.strokeWeight(3);  // Thicker
    graficoProfiloSfondo.line(startX,startY, stopX, startY); // X AXIS
    graficoProfiloSfondo.line(startX,startY, startX, stopY); // Y AXIS
    //lineette
    graficoProfiloSfondo.strokeWeight(1);
    for(int i=stepX; i<stopX-startX-10; i=i+stepX)
      graficoProfiloSfondo.line(startX+i,startY-5,startX+i,startY+5);
    for(int i=stepY; i<startY-stopY-10; i=i+stepY)
      graficoProfiloSfondo.line(startX-5,startY-i,startX+5,startY-i);
  
    //Plotto le linee del grafico di sfondo in base alle temperature stabilite
    graficoProfiloSfondo.stroke(150);
    int TEMP_SOAK_MIN_PL   = startY - round(map(TEMPERATURE_SOAK_MIN,   0, 50, 0, stepY));
    int TEMP_SOAK_MAX_PL   = startY - round(map(TEMPERATURE_SOAK_MAX,   0, 50, 0, stepY));
    int TEMP_REFLOW_MAX_PL = startY - round(map(TEMPERATURE_REFLOW_MAX, 0, 50, 0, stepY));
    int TEMP_COOL_PL       = startY - round(map(TEMPERATURE_COOLDOWN,   0, 50, 0, stepY));
    
    graficoProfiloSfondo.line(startX,startY,stepX*2.5f+startX,TEMP_SOAK_MIN_PL);
    graficoProfiloSfondo.line(stepX*2.5f+startX, TEMP_SOAK_MIN_PL, stepX*6+startX, TEMP_SOAK_MAX_PL);
    graficoProfiloSfondo.line(stepX*6+startX, TEMP_SOAK_MAX_PL, stepX*8+startX, TEMP_REFLOW_MAX_PL);
    graficoProfiloSfondo.line(stepX*8+startX, TEMP_REFLOW_MAX_PL, stepX*10+startX, TEMP_SOAK_MAX_PL);
    graficoProfiloSfondo.line(stepX*10+startX, TEMP_SOAK_MAX_PL, stepX*12+startX, TEMP_COOL_PL);
    
    //Plotto le linee di divisione tra le varie aree di reflow
    graficoProfiloSfondo.stroke(180);
    graficoProfiloSfondo.line(startX+stepX*2.5f,stopY+30,startX+stepX*2.5f,startY);
    graficoProfiloSfondo.line(startX+stepX*6,stopY+30,startX+stepX*6,startY);
    graficoProfiloSfondo.line(startX+stepX*10,stopY+30,startX+stepX*10,startY);
    
    //Scrivo i nomi delle aree:
    graficoProfiloSfondo.stroke(0);
    graficoProfiloSfondo.fill(0, 30, 225);
    graficoProfiloSfondo.textSize(16);
    graficoProfiloSfondo.text("Preheat",  startX+40 , 70);
    graficoProfiloSfondo.text("Soak",     startX+200, 70);
    graficoProfiloSfondo.text("Reflow",   startX+390, 70);
    graficoProfiloSfondo.text("Cooldown", startX+550, 70);
    
    //creo l'insieme di punti che mi d\u00e0 il grafico
    graficoProfiloSfondo.stroke(0);
    graficoProfiloSfondo.strokeWeight(2);
    for(int i=0; i<table.getRowCount(); i++)
    {
      TableRow row = table.getRow(i);
      X = row.getInt("time");
      Y = (int)row.getFloat("temperature");
      
      line(oldX, oldY, X, Y);
      oldX = X;
      oldY = Y;
    }
    if(table.getRowCount() > 0)
    {
      TableRow first_row = table.getRow(0);
      oldX = first_row.getInt("time");
      oldY = (int)first_row.getFloat("temperature");
    }
    
  graficoProfiloSfondo.endDraw();
  image(graficoProfiloSfondo, 10, 0);
}
// Imposta il profilo, lo disegna e invia i dati al controller per
// l'impostazione dei parametri
public void buttonSetTemp_click(GButton source, GEvent event)
{
  //leggo dalle textfield
  int t_soak_min   = Integer.valueOf(tf_TEMP_SOAK_MIN.getText());
  int t_soak_max   = Integer.valueOf(tf_TEMP_SOAK_MAX.getText());
  int t_refl_max = Integer.valueOf(tf_TEMP_REFL_MAX.getText());
  int t_cooldown   = Integer.valueOf(tf_TEMP_COOL.getText());

  //scrivo tutto sulla stringa da mandare
  String dataToSend = "(" + t_soak_min   + ","
                          + t_soak_max   + ","
                          + t_refl_max   + ","
                          + t_cooldown   + ")";
  
  //converto tutto in array di byte e provo a mandare sulla seriale
  byte[] dataBytes = dataToSend.getBytes();
  if (connected)
  {
    try                  {  myPort.write(dataBytes);  } 
    catch (Exception e)  {  println("can't write the port: " + e);}
  }
}

public void buttonLeft_click(GButton source, GEvent event)
{
  myPort.write(left_byte);
}

public void buttonRight_click(GButton source, GEvent event)
{
  myPort.write(right_byte);
}

public void buttonClear_click(GButton source, GEvent event)
{
  try{  myPort.clear();} catch(Exception e) {}
  table.clearRows();
  labelSerialReceive.setText(" ");
  
  //Reimposto il grafico:
  TEMPERATURE_SOAK_MIN   = 140;
  TEMPERATURE_SOAK_MAX   = 177;
  TEMPERATURE_REFLOW_MAX = 230;
  TEMPERATURE_COOLDOWN   = 100;
  
  tf_TEMP_SOAK_MIN.setText(Integer.toString(TEMPERATURE_SOAK_MIN));
  tf_TEMP_SOAK_MAX.setText(Integer.toString(TEMPERATURE_SOAK_MAX));
  tf_TEMP_REFL_MAX.setText(Integer.toString(TEMPERATURE_REFLOW_MAX));
  tf_TEMP_COOL.setText(Integer.toString(TEMPERATURE_COOLDOWN));
}

public void buttonStop_click(GButton source, GEvent event)
{
  myPort.write(999);
}

public void buttonConnect_click(GButton source, GEvent event)
{ 
  try
  {
    //mi connetto o disconnetto dalla porta (se ho l'eccezione chiudo e mi disconnetto)
    myPort = new Serial(this, COM_list.getSelectedText(), 57600);
    myPort.clear();
    
    //se connessione a buon fine scrivo un po' di label e coloro il tasto
    serial_ID.setText("connected to: " + COM_list.getSelectedText());
    
    button_connect.setLocalColorScheme(GCScheme.RED_SCHEME);
    button_connect.setText("disconnect");
    
    labelX.setVisible(true);
    labelY.setVisible(true);
    
    connected = true;
  }
  catch (Exception e)
  {
    //stampo l'eccezione per debug
    println("Eccezione di connessione: " + e);
    
    myPort.clear();
    myPort.stop();
    
    serial_ID.setText("not connected");
    serial_ID.setTextItalic();
    
    button_connect.setLocalColorScheme(GCScheme.BLUE_SCHEME);
    button_connect.setText("connect");
        
    labelX.setVisible(false);
    labelY.setVisible(false);
    
    connected = false;
  } 
}

// Dichiarazione oggetti della GUI:
GButton button_left; 
GButton button_right; 
GButton button_clear;
GButton button_stop; 
GButton button_connect; 
GButton button_setTemp;

GLabel serial_ID; 
GLabel labelX; 
GLabel labelY; 
GLabel labelSerialReceive;

GDropList COM_list;

GTextField tf_TEMP_SOAK_MIN;
GTextField tf_TEMP_SOAK_MAX;
GTextField tf_TEMP_REFL_MAX;
GTextField tf_TEMP_COOL;

// ******** Function createGUI() ********
public void createGUI()
{
  G4P.messagesEnabled(false);
  G4P.setGlobalColorScheme(GCScheme.BLUE_SCHEME);
  G4P.setCursor(ARROW);
  surface.setTitle("GFE Reflow Oven data grapher");
  
  tf_TEMP_SOAK_MIN = new GTextField(this, startX+50 , 80, 58, 20);
  tf_TEMP_SOAK_MIN.setText(Integer.toString(TEMPERATURE_SOAK_MIN));
  
  tf_TEMP_SOAK_MAX = new GTextField(this, startX+200, 80, 58, 20);
  tf_TEMP_SOAK_MAX.setText(Integer.toString(TEMPERATURE_SOAK_MAX));
  
  tf_TEMP_REFL_MAX = new GTextField(this, startX+398, 80, 58, 20);
  tf_TEMP_REFL_MAX.setText(Integer.toString(TEMPERATURE_REFLOW_MAX));
  
  tf_TEMP_COOL = new GTextField(this, startX+570, 80, 58, 20);
  tf_TEMP_COOL.setText(Integer.toString(TEMPERATURE_COOLDOWN));
  
  button_setTemp = new GButton(this, 720, 80, 40, 20);
  button_setTemp.setText("SET");
  button_setTemp.addEventHandler(this, "buttonSetTemp_click");
  
  button_left = new GButton(this, 60, 390, 80, 50);
  button_left.setText("left");
  button_left.addEventHandler(this, "buttonLeft_click");
  
  button_right = new GButton(this, 260, 390, 80, 50);
  button_right.setText("right");
  button_right.addEventHandler(this, "buttonRight_click");
  
  button_clear = new GButton(this, 460, 390, 80, 50);
  button_clear.setText("clear");
  button_clear.addEventHandler(this, "buttonClear_click");
  
  button_stop = new GButton(this, 660, 390, 80, 50);
  button_stop.setText("stop");
  button_stop.addEventHandler(this, "buttonStop_click");
    
  button_connect = new GButton(this, 330, 10, 80, 50);
  button_connect.setText("connect");
  button_connect.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  button_connect.addEventHandler(this, "buttonConnect_click");
  
  serial_ID = new GLabel(this, 450, 10, 300, 20);
  serial_ID.setText("not connected");
  serial_ID.setTextItalic();
  serial_ID.setOpaque(false);
  
  COM_list = new GDropList(this, 60, 10, 240, 80, 3);
  
  labelSerialReceive = new GLabel(this, 450, 40, 300, 20);
  labelSerialReceive.setTextAlign(GAlign.LEFT, GAlign.MIDDLE);
  
  labelX = new GLabel(this, 120, 40, 80, 20);
  labelX.setTextAlign(GAlign.LEFT, GAlign.MIDDLE);
  labelX.setText("x:");
  labelX.setOpaque(false);
  
  labelY = new GLabel(this, 220, 40, 80, 20);
  labelY.setTextAlign(GAlign.LEFT, GAlign.MIDDLE);
  labelY.setText("y:");
  labelY.setOpaque(false);
}
  public void settings() {  size(800, 450, JAVA2D); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "Reflow_oven_serial_controller" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
