// Need G4P library
import g4p_controls.*;
import processing.serial.*;

Serial myPort;

int left_byte = 100;
int right_byte = 10;
boolean connected = false;

int startX = 60;
int stopY  = 50;
int stopX  = 736;
int startY = 370;
int stepX = (stopX-50-startX)/12;
int stepY = (startY-stopY-20)/6;

int X, Y, oldX, oldY = 0;

int   val1, val2 = 0;
float val3, val4 = 0;

Table table = new Table();

public void setup()
{
  size(800, 450, JAVA2D);
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
  
  serialReceive();
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
      if(iniPac != -1 && endPac != -1)
      {
        riga = riga.substring(iniPac+1, endPac);
        parametri = split(riga, ','); // (timeStamp[0],setpoint[1],input[2],output[3])
        
        //arrotondo i valori per piazzarli nel grafico e li aggiungo alla tabella
        int new_time = round(map(Integer.valueOf(parametri[0]), 0, 30, 0, stepX));
        float new_temp = round(map(Float.valueOf(parametri[2]), 0, 25, 0, stepY));
        
        table.addRow();
        int n = table.getRowCount();
        table.setInt(n-1, "time", new_time+startX+10);
        table.setFloat(n-1, "temperature", startY-new_temp);
        
        labelSerialReceive.setText("got: " + riga);
      }
    }
  }
  catch (Exception e) {println("exception");}
}