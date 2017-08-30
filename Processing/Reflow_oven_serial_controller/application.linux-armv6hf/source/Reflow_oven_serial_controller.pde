// G4P
import g4p_controls.*;

// Comunicazione seriale
import processing.serial.*;

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
  size(800, 450, JAVA2D);
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
  
void serialReceive()
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
          labelY.setText(parametri[2] + " °C");
          
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