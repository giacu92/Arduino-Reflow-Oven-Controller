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
  
  // AXIS:
  stroke(0);
  strokeWeight(3);  // Thicker
  line(startX,startY, stopX, startY); // X AXIS
  line(startX,startY, startX, stopY); // Y AXIS
  //lineette
  strokeWeight(1);
  for(int i=stepX; i<stopX-startX-10; i=i+stepX)
    line(startX+i,startY-5,startX+i,startY+5);
  for(int i=stepY; i<startY-stopY-10; i=i+stepY)
    line(startX-5,startY-i,startX+5,startY-i);
  
  stroke(150);
  line(startX,startY,stepX*2.5+startX, startY-stepY*3);
  line(stepX*2.5+startX, startY-stepY*3, stepX*6+startX, startY-stepY*4);
  line(stepX*6+startX, startY-stepY*4, stepX*8+startX, startY-stepY*5);
  line(stepX*8+startX, startY-stepY*5, stepX*10+startX, startY-stepY*4);
  line(stepX*10+startX, startY-stepY*4, stepX*12+startX, startY-stepY*2);
  
  stroke(180);
  line(startX+stepX*2.5,stopY+30,startX+stepX*2.5,startY);
  line(startX+stepX*6,stopY+30,startX+stepX*6,startY);
  line(startX+stepX*10,stopY+30,startX+stepX*10,startY);

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
  
  val1 = (int)map(val1, 0, 360, startX, stopX);
  val2 = (int)map(val2, 0, 260, stopY, startY);
  line(startX, val2, stopX, val2);
  
  labelX.setText("x: " + str(val1)    + "s" );
  labelY.setText("y: " + nf(val2,3,2) + "°C");
}
  
// Use this method to add additional statements
// to customise the GUI controls
public void customGUI(){

}