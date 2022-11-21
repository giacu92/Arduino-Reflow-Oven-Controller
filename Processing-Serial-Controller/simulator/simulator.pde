 // G4P
import g4p_controls.*;

// Comunicazione seriale
import processing.serial.*;

private static final boolean lightDebug = true;

// Simulator parameters
static final int windowSize = 500;
static double kFactor = 100;
static double yFactor = 0.01;
static double currentTemp = 25.0;

Serial myPort;

int left_byte = 100;
int right_byte = 10;
int stop_byte  = 36;
int tune_byte  = 35;

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

int DURATION_PH             = 75;
int DURATION_SOAK           = 105;
int DURATION_REFLOW_RAMPUP  = 60;


Table table = new Table();
boolean connected = false;

public void setup()
{
  size(800, 450, JAVA2D);
  frameRate(4);
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
  
void serialReceive()
{
  try
  {
    if(myPort.available() > 0)
    {
      String[] parametri;
      String riga = myPort.readString(); // mi aspetto "(timeStamp,setpoint,input,output)"
      print("--> " + riga);
      
      int iniPac = riga.indexOf("(");
      int endPac = riga.indexOf(")");
      if(iniPac != -1 && endPac != -1 && endPac-iniPac > 1)
      {
        labelSerialReceive.setText("got: " + riga);
        
        riga = riga.substring(iniPac+1, endPac);
        parametri = split(riga, ','); // (timeStamp[0],setpoint[1],input[2],output[3])
        
        if (!lightDebug)
        {
          println("parametri[0] = " + parametri[0]); //<>//
          println("parametri[1] = " + parametri[1]);
          println("parametri[2] = " + parametri[2]);
          println("parametri[3] = " + parametri[3]);
          println("parametri[4] = " + parametri[4]);
          println("parametri[5] = " + parametri[5]);
          println("parametri[6] = " + parametri[6]);
        }
        
        //Se ho ricevuto "  [L]" o "  [LF]" vado solo a modificare le temperature 
        if (parametri[0].equals("  [L]") || parametri[0].equals(" [LF]") || parametri[0].equals(" [C]"))
        {
          TEMPERATURE_SOAK_MIN   = Integer.valueOf(parametri[1]);
          TEMPERATURE_SOAK_MAX   = Integer.valueOf(parametri[2]);
          TEMPERATURE_REFLOW_MAX = Integer.valueOf(parametri[3]);
          DURATION_PH            = Integer.valueOf(parametri[4]);
          DURATION_SOAK          = Integer.valueOf(parametri[5]);
          DURATION_REFLOW_RAMPUP = Integer.valueOf(parametri[6]);
          
          tf_TEMP_SOAK_MIN.setText(Integer.toString(TEMPERATURE_SOAK_MIN));
          tf_TEMP_SOAK_MAX.setText(Integer.toString(TEMPERATURE_SOAK_MAX));
          tf_TEMP_REFL_MAX.setText(Integer.toString(TEMPERATURE_REFLOW_MAX));
          tf_PH_DURATION.setText(Integer.toString(DURATION_PH));
          tf_SOAK_DURATION.setText(Integer.toString(DURATION_SOAK));
          tf_REFL_RU_DURATION.setText(Integer.toString(DURATION_REFLOW_RAMPUP));
  
          if (!lightDebug)
          {
            println("temperature_soak_min:" + TEMPERATURE_SOAK_MIN);
            println("temperature_soak_max:" + TEMPERATURE_SOAK_MAX);
            println("temperature_reflow_max:" + TEMPERATURE_REFLOW_MAX);
          }
        }
        else //ho ricevuto i valori da plottare
        {
          labelX.setText(parametri[0] + " s" );
          labelY.setText(parametri[2] + " °C");
          
          //arrotondo i valori per piazzarli nel grafico e li aggiungo alla tabella
          int new_time = round(map(Integer.valueOf(parametri[0]), 0, 30, 0, stepX));
          float new_temp = round(map(Float.valueOf(parametri[2]), 0, 50, 0, stepY));
          
          table.addRow();
          int n = table.getRowCount();
          table.setInt(n-1, "time", new_time+startX+10);
          table.setFloat(n-1, "temperature", startY-new_temp);
          
          // Calcolo nuova temperatura
          float pidValue = Float.valueOf(parametri[3]);
          //println(pidValue);
          
          //double newTemp = new_temp + pidValue/Float.valueOf(windowSize)*kFactor/new_temp - (new_temp-18)*yFactor/kFactor;
          
          double new_temp2 = new_temp*new_temp;
          double new_temp3 = new_temp2*new_temp;
          
          double increment = (3.37   + 0.00378*new_temp  - 0.000115*new_temp2 + 0.000000272*new_temp3) * pidValue/Float.valueOf(windowSize) ;
          double decrement = 0.0803 + 0.000528*new_temp + 0.0000241*new_temp2 - 0.0000000361*new_temp3;
          
          double newTemp = new_temp + increment - decrement;
          
          
          if (!lightDebug)
          {
            println("newTemp = " + newTemp);
          }
          
          //scrivo tutto sulla stringa da mandare
          String dataToSend = "(" + "[S]"      + ","
                                  + String.format("%.2f", newTemp)     + ")";
          
          //converto tutto in array di byte e provo a mandare sulla seriale
          byte[] dataBytes = dataToSend.getBytes();
          if (connected)
          {
            try
            {
              myPort.write(dataBytes);
              println("<-- " + dataToSend);
            } 
            catch (Exception e)  {  println("can't write the port: " + e);}
          }
        }
      }
    }
  }
  catch (Exception e) {println("exception");}
}
