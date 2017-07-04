// Need G4P library
import g4p_controls.*;
import processing.serial.*;

Serial myPort;

int left_byte = 100;
int right_byte = 10;
boolean check = false;

int startX = 60;
int stopY  = 50;
int stopX  = 740;
int startY = 370;
int stepX = (stopX-50-startX)/12;
int stepY = (startY-stopY-20)/6;

int   val1, val2 = 0;
float val3, val4 = 0;

public void setup()
{
  size(800, 450, JAVA2D);
  createGUI();
  customGUI();

  COM_list.setItems(Serial.list(), 0);
  labelX.setVisible(false);
  labelY.setVisible(false);
}

public void draw()
{
  background(220);
  

  //if (COM_list.getText().substring(0, 19) == "/dev/cu.wchusbserial") check = true;
  if (COM_list.getText().indexOf("/dev/cu.wchusbserial") > 0 )  check = true;
  
  if (check)
  {
    while (myPort.available() > 0)
    {
      String inByte = myPort.readString();
      println(inByte);
      
      //val1 = Integer.parseInt(inByte.substring(0,1));
      //val2 = Integer.parseInt(inByte.substring(1,2));
      
      textfield1.setText(inByte);
      textfield1.appendText(inByte + " ");
    }
  }
  
  strokeWeight(0);
  fill(0,220,0);
  
  val1 = (int)map(val1, 0, 360, startX, stopX);
  val2 = (int)map(val2, 0, 260, stopY, startY);
  line(startX, val2, stopX, val2);
  
  labelX.setText("x: " + str(val1)    + "s" );
  labelY.setText("y: " + nf(val2,3,2) + "°C");
}
  

public void customGUI()
{
}