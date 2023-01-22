#ifdef AUTOTUNE_BETA
  void AutoTuneHelper(boolean start)
  {
    if(start)
      ATuneModeRemember = reflowOvenPID.GetMode();
    else
      reflowOvenPID.SetMode(ATuneModeRemember);
  }
#endif

#ifdef USE_LCD_KEYPAD_SHIELD
  
  int read_LCD_buttons()
  {
   int adc_key_in = analogRead(0);      // read the value from the sensor 
   // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
   // we add approx 50 to those values and check to see if we are close
   //if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
   // For V1.1 us this threshold
   /*
   if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 250)  return btnUP; 
   if (adc_key_in < 450)  return btnDOWN; 
   if (adc_key_in < 650)  return btnLEFT; 
   if (adc_key_in < 850)  return btnSELECT;  
    */
   // For V1.0 comment the other threshold and use the one below:
   delay(100);
   if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 195)  return btnUP; 
   if (adc_key_in < 380)  return btnDOWN; 
   if (adc_key_in < 555)  return btnLEFT; 
   if (adc_key_in < 790)  return btnSELECT;   
  
   return btnNONE;  // when all others fail, return this...
  }
  
  void profileBuild()
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp PH:");
    lcd.setCursor(0,1);
    lcd.print("+");
    lcd.setCursor(13,1);
    lcd.print("OK");
    lcd.setCursor(9,0);
    lcd.print("0");
    
    int val = btnNONE;
    val = read_LCD_buttons();
    int t_cur = 0, cursor = 9;
    int temp_set[3] = {0};
    
    while(cursor < 12) // need a pull-up resistor (10KΩ)
    {
      val = read_LCD_buttons();
      if (val == btnLEFT)
      {
        t_cur++;
        if(cursor == 9 && t_cur > 1 || t_cur > 9) t_cur = 0; 
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        delay(50);
      }
      else if(val == btnRIGHT)
      {
        temp_set[cursor-9] = t_cur;
        cursor++;
        t_cur = 0;
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        debounce();
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp PH: ");
    lcd.print(temp_set[0]);
    lcd.print(temp_set[1]);
    lcd.print(temp_set[2]);
    TEMPERATURE_SOAK_MIN = aToInt(temp_set)+1;
  
    delay(1000);
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp SOAK:");
    lcd.setCursor(0,1);
    lcd.print("+");
    lcd.setCursor(13,1);
    lcd.print("OK");
    lcd.setCursor(11,0);
    lcd.print("0");
    
    val = read_LCD_buttons();
    cursor = 11;
    temp_set[3] = {0};
    
    while(cursor < 14) // need a pull-up resistor (10KΩ)
    {
      val = read_LCD_buttons();
      if (val == btnLEFT)
      {
        t_cur++;
        if(cursor == 11 && t_cur > 1 || t_cur > 9) t_cur = 0; 
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        delay(50);
      }
      else if(val == btnRIGHT)
      {
        temp_set[cursor-11] = t_cur;
        cursor++;
        t_cur = 0;
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        debounce();
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp SOAK: ");
    lcd.print(temp_set[0]);
    lcd.print(temp_set[1]);
    lcd.print(temp_set[2]);
    TEMPERATURE_SOAK_MAX = aToInt(temp_set)+1;
  
    delay(1000);
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp REFL:");
    lcd.setCursor(0,1);
    lcd.print("+");
    lcd.setCursor(13,1);
    lcd.print("OK");
    lcd.setCursor(11,0);
    lcd.print("0");
    
    val = read_LCD_buttons();
    cursor = 11;
    temp_set[3] = {0};
    
    while(cursor < 14) // need a pull-up resistor (10KΩ)
    {
      val = read_LCD_buttons();
      if (val == btnLEFT)
      {
        t_cur++;
        if(cursor == 11 && t_cur > 2 || t_cur > 9) t_cur = 0; 
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        delay(50);
      }
      else if(val == btnRIGHT)
      {
        temp_set[cursor-11] = t_cur;
        cursor++;
        t_cur = 0;
        lcd.setCursor(cursor,0);
        lcd.print(t_cur);
        debounce();
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp REFL: ");
    lcd.print(temp_set[0]);
    lcd.print(temp_set[1]);
    lcd.print(temp_set[2]);
    TEMPERATURE_REFLOW_MAX = aToInt(temp_set)+1;
    delay(1000);
  
    type = " [C]";
    sendProfile();
  }
  
  void debounce()
  {
    while(read_LCD_buttons() != btnNONE)
    {delay(100);}
  }
  
  int aToInt(int arr[3])
  {
    int val = 0;
    
    for (int i=0; i<3; i++)
    {
      val = val + pow(10,(2-i)) * arr[i];
    }
  
    return val;
  }
#endif

byte updateTempSimulator()
{
  //Leggo la stringa in arrivo dalla seriale.
  char dataIn[100] = {""};
  Serial.readBytes(dataIn, 100);
  String receivedString(dataIn);
  //data = dataIn[0];
  //Serial.println("Received string: " + receivedString);

  int iniPac = receivedString.indexOf('(');
  int endPac = receivedString.indexOf(')');

  //se ci sono i caratteri di inizio e fine pacchetto è un messaggio,
  //altrimenti ho ricevuto solo un dato da inserire in data.
  if (iniPac != -1 && endPac != -1 && endPac - iniPac > 1)
  {
    String packetIn = receivedString;
    packetIn = packetIn.substring(iniPac + 1, endPac); //tolgo le parentesi

    // Simulator support
    if (receivedString.indexOf("[S],") >= 0)
    {
      //Serial.println("flag");
      isSimulation = true;
      // Override temps
      input = packetIn.substring(4, packetIn.length()).toDouble();
      return 0;
    }
  }
  else
  {
    return dataIn[0];
  }
}

double getTemp()
{
  double t1 = thermocouple.readCelsius();
  #ifdef USE_TC2
    double in_tc2 = tc2.readCelsius();
    if (isnan(t1))    { t1 = in_tc2; }
    else if (!isnan(in_tc2))   { t1 = (t1 + in_tc2) / 2.0; }   //average value of the two tc
  #endif
  //Serial.println("TC input = " + (String)t1);
  return t1;
}