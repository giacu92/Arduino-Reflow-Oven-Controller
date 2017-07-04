public void button1_click1(GButton source, GEvent event)
{
  myPort.write(left_byte);
}

public void button2_click1(GButton source, GEvent event)
{
  myPort.write(right_byte);
}

public void button1_click2(GButton source, GEvent event)
{
  textfield1.setText(" ");
}

public void button2_click2(GButton source, GEvent event)
{
  myPort.write(999);
}

public void dropList1_click1(GDropList source, GEvent event)
{
}

public void button3_click1(GButton source, GEvent event)
{ 
  check = true;
  myPort = new Serial(this, COM_list.getSelectedText(), 57600);
  serial_ID.setText("connected to: " + COM_list.getSelectedText());
  button_connect.setLocalColorScheme(GCScheme.BLUE_SCHEME);
  labelX.setVisible(true);
  labelY.setVisible(true);
  
}

public void textfield1_change1(GTextField source, GEvent event)
{
}

GButton button_left; 
GButton button_right; 
GButton button_clear;
GButton button_stop; 
GButton button_connect; 

GLabel serial_ID; 
GLabel labelX; 
GLabel labelY; 

GDropList COM_list; 

GTextField textfield1; 


public void createGUI()
{
  G4P.messagesEnabled(false);
  G4P.setGlobalColorScheme(GCScheme.BLUE_SCHEME);
  G4P.setCursor(ARROW);
  surface.setTitle("GFE Reflow Oven data grapher");
  
  button_left = new GButton(this, 60, 390, 80, 50);
  button_left.setText("left");
  button_left.addEventHandler(this, "button1_click1");
  
  button_right = new GButton(this, 260, 390, 80, 50);
  button_right.setText("right");
  button_right.addEventHandler(this, "button2_click1");
  
  button_clear = new GButton(this, 460, 390, 80, 50);
  button_clear.setText("clear");
  button_clear.addEventHandler(this, "button1_click2");
  
  serial_ID = new GLabel(this, 450, 10, 300, 20);
  serial_ID.setText("not connected");
  serial_ID.setTextItalic();
  serial_ID.setOpaque(false);
  
  button_stop = new GButton(this, 660, 390, 80, 50);
  button_stop.setText("stop");
  button_stop.addEventHandler(this, "button2_click2");
  
  COM_list = new GDropList(this, 60, 10, 240, 80, 3);
  COM_list.setItems(loadStrings("list_833361"), 0);
  COM_list.addEventHandler(this, "dropList1_click1");
  
  button_connect = new GButton(this, 330, 10, 80, 50);
  button_connect.setText("connect");
  button_connect.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  button_connect.addEventHandler(this, "button3_click1");
  
  textfield1 = new GTextField(this, 450, 40, 300, 20, G4P.SCROLLBARS_NONE);
  textfield1.setOpaque(true);
  textfield1.addEventHandler(this, "textfield1_change1");
  
  labelX = new GLabel(this, 120, 40, 80, 20);
  labelX.setTextAlign(GAlign.LEFT, GAlign.MIDDLE);
  labelX.setText("x:");
  labelX.setOpaque(false);
  
  labelY = new GLabel(this, 220, 40, 80, 20);
  labelY.setTextAlign(GAlign.LEFT, GAlign.MIDDLE);
  labelY.setText("y:");
  labelY.setOpaque(false);
}