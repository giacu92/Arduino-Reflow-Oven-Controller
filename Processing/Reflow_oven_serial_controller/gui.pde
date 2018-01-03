public void COMlist_click(GDropList source, GEvent event)
{
  //COM_list.setItems(Serial.list(), COM_list.get);
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
  /*TEMPERATURE_SOAK_MIN   = 140;
  TEMPERATURE_SOAK_MAX   = 177;
  TEMPERATURE_REFLOW_MAX = 230;
  TEMPERATURE_COOLDOWN   = 100;*/
  
  tf_TEMP_SOAK_MIN.setText(Integer.toString(TEMPERATURE_SOAK_MIN));
  tf_TEMP_SOAK_MAX.setText(Integer.toString(TEMPERATURE_SOAK_MAX));
  tf_TEMP_REFL_MAX.setText(Integer.toString(TEMPERATURE_REFLOW_MAX));
  tf_TEMP_COOL.setText(Integer.toString(TEMPERATURE_COOLDOWN));
}

public void buttonStop_click(GButton source, GEvent event)
{
  myPort.write(stop_byte);
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
  button_left.setText("LEFT");
  button_left.addEventHandler(this, "buttonLeft_click");
  
  button_right = new GButton(this, 260, 390, 80, 50);
  button_right.setText("RIGHT");
  button_right.addEventHandler(this, "buttonRight_click");
  
  button_clear = new GButton(this, 460, 390, 80, 50);
  button_clear.setText("CLEAR");
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
  COM_list.addEventHandler(this, "COMlist_click");
  
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