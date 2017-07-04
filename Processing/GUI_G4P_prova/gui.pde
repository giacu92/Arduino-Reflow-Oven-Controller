public void buttonLeft_click1(GButton source, GEvent event)
{
  myPort.write(left_byte);
}

public void buttonRight_click1(GButton source, GEvent event)
{
  myPort.write(right_byte);
}

public void buttonClear_click2(GButton source, GEvent event)
{
  myPort.clear();
  table.clearRows();
  labelSerialReceive.setText(" ");
}

public void buttonStop_click2(GButton source, GEvent event)
{
  myPort.write(999);
}

public void dropList1_click1(GDropList source, GEvent event)
{
}

public void buttonConnect_click1(GButton source, GEvent event)
{ 
  connected = true;
  
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
  }
  catch (Exception e)
  {
    //stampo l'eccezione per debug
    println(e);
    
    //disabilito il flag "connected"
    connected = false;
    
    myPort.clear();
    myPort.stop();
    labelX.setVisible(false);
    labelY.setVisible(false);
    button_connect.setLocalColorScheme(GCScheme.BLUE_SCHEME);
    button_connect.setText("connect");
    serial_ID.setText("not connected");
    serial_ID.setTextItalic();
  } 
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
GLabel labelSerialReceive;

GDropList COM_list; 


public void createGUI()
{
  G4P.messagesEnabled(false);
  G4P.setGlobalColorScheme(GCScheme.BLUE_SCHEME);
  G4P.setCursor(ARROW);
  surface.setTitle("GFE Reflow Oven data grapher");
  
  button_left = new GButton(this, 60, 390, 80, 50);
  button_left.setText("left");
  button_left.addEventHandler(this, "buttonLeft_click1");
  
  button_right = new GButton(this, 260, 390, 80, 50);
  button_right.setText("right");
  button_right.addEventHandler(this, "buttonRight_click1");
  
  button_clear = new GButton(this, 460, 390, 80, 50);
  button_clear.setText("clear");
  button_clear.addEventHandler(this, "buttonClear_click2");
  
  button_stop = new GButton(this, 660, 390, 80, 50);
  button_stop.setText("stop");
  button_stop.addEventHandler(this, "buttonStop_click2");
    
  button_connect = new GButton(this, 330, 10, 80, 50);
  button_connect.setText("connect");
  button_connect.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  button_connect.addEventHandler(this, "buttonConnect_click1");
  
  serial_ID = new GLabel(this, 450, 10, 300, 20);
  serial_ID.setText("not connected");
  serial_ID.setTextItalic();
  serial_ID.setOpaque(false);
  
  COM_list = new GDropList(this, 60, 10, 240, 80, 3);
  COM_list.setItems(loadStrings("list_833361"), 0);
  COM_list.addEventHandler(this, "dropList1_click1");
  
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