// Need G4P library
import g4p_controls.*;
import processing.serial.*;

Serial myPort;

int left_byte = 100;
int right_byte = 10;
boolean check = false;

int startX = 60;
int stopY  = 60;
int stopX  = 740;
int startY = 380;

int   val1, val2 = 0;
float val3, val4 = 0;

public void setup(){
  
  
  size(800, 450, JAVA2D);
  createGUI();
  customGUI();
  // Place your setup code here
  COM_list.setItems(Serial.list(), 0);
}

public void draw()
{
  background(220);
  
  stroke(0);
  strokeWeight(3);  // Thicker
  line(startX,startY, stopX, startY); // X LINE
  line(startX,startY, startX, stopY); // Y LINE
  stroke(150);
  strokeWeight(1);
  line(startX,startY,185+startX, startY-192);
  
  //if (COM_list.getText().substring(0, 19) == "/dev/cu.wchusbserial") check = true;
  
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
  
  val1 = (int)map(val1, 0, 400, startX, stopX);
  val2 = (int)map(val2, 0, 260, stopY, startY);
  line(startX, val2, stopX, val2);
}
  
// Use this method to add additional statements
// to customise the GUI controls
public void customGUI(){

}