/*
   Revision  Description
   ========  ===========
   1.1       Arduino Nano v3 for GFE Hand Made Reflow Oven Controller board v1.1
             - Compiled for Arduino Nano.
             - Display changed to 16x2 Hitachi HD44780 Compatible.
             - added data output to diplay temperature vs time.
             - added possibility to choose between lead free of leaded solder paste profiles.
   
   1.2       Arduino Nano v3 for GFE Hand Made Reflow Oven Controller board v1.2
             - Added sounds.
             - Added temperature self check. The process self stops if unable to drive the oven.

   Temperature (Degree Celcius)                 Magic Happens Here!
   245-|                                               x  x
       |                                            x        x
       |                                         x              x
       |                                      x                    x
   200-|                                   x                          x
       |                              x    |                          |   x
       |                         x         |                          |       x
       |                    x              |                          |
   150-|               x                   |                          |
       |             x |                   |                          |
       |           x   |                   |                          |
       |         x     |                   |                          |
       |       x       |                   |                          |
       |     x         |                   |                          |
       |   x           |                   |                          |
   30 -| x             |                   |                          |
       |<  60 - 90 s  >|<    90 - 120 s   >|<       90 - 120 s       >|
       | Preheat Stage |   Soaking Stage   |       Reflow Stage       | Cool
    0  |_ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
                                                                  Time (Seconds)
*/

// ***** INCLUDES *****
#include <SPI.h>
#include <LiquidCrystal.h>
#include "Adafruit_MAX31855.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

//#define USE_LCD_KEYPAD_SHIELD

// ***** CONSTANTS *****
#define TEMPERATURE_ROOM 50

#define TEMPERATURE_COOL_MIN 100
#define SENSOR_SAMPLING_TIME 1000
#define SOAK_TEMPERATURE_STEP 2
#define SOAK_MICRO_PERIOD 7000
#define DEBOUNCE_PERIOD_MIN 50

// ***** PID PARAMETERS *****
// Pre-Heating:
#define PID_KP_PREHEAT 60    //100
#define PID_KI_PREHEAT 0.05    //0.025
#define PID_KD_PREHEAT 20    //20
// Soaking:
#define PID_KP_SOAK 100  //300
#define PID_KI_SOAK 0.05       //0.05
#define PID_KD_SOAK 100          //250
// Reflowing:
#define PID_KP_REFLOW  300     //300
#define PID_KI_REFLOW  0.10    //0.05
#define PID_KD_REFLOW  350    //350

// Sample time:
#define PID_SAMPLE_TIME 1000

// Notes:
#define  note_c     261 //Hz
#define  note_cd    277 //Hz
#define  note_d     294 //Hz
#define  note_e     330 //Hz
#define  note_f     349 //Hz
#define  note_fg    370 //Hz
#define  note_g     392 //Hz
#define  note_ga    415 //Hz
#define  note_a     440 //Hz
#define  note_ab    466 //Hz
#define  note_b     494 //Hz
#define  note_C     523 //Hz

// ***** TYPE DEFINITIONS *****
typedef enum REFLOW_STATE
{
  REFLOW_STATE_IDLE,
  REFLOW_STATE_PRIOR,
  REFLOW_STATE_PREHEAT,
  REFLOW_STATE_SOAK,
  REFLOW_STATE_REFLOW,
  REFLOW_STATE_COOL,
  REFLOW_STATE_COMPLETE,
  REFLOW_STATE_TOO_HOT,
  REFLOW_STATE_ERROR,
  REFLOW_STATE_TUNING_PH
}
reflowState_t;

typedef enum REFLOW_STATUS
{
  REFLOW_STATUS_OFF,
  REFLOW_STATUS_ON
}
reflowStatus_t;

// ***** LCD MESSAGES *****
const char* lcdMessagesReflowStatus[] = {
  "Ready",
  "Prior",
  "Preheat",
  "Soak",
  "Reflow",
  "Cool",
  "Complete",
  "Wait,hot",
  "Error",
  "Tuning"
};

// ***** DEGREE SYMBOL FOR LCD *****
unsigned char degree[8]  = { 140, 146, 146, 140, 128, 128, 128, 128 };

// ***** PIN ASSIGNMENT *****
#ifdef USE_LCD_KEYPAD_SHIELD
  #define btnRIGHT  0
  #define btnUP     1
  #define btnDOWN   2
  #define btnLEFT   3
  #define btnSELECT 4
  #define btnNONE   5
  #define ssrPin             2//6
  #define thermocoupleSOPin  11//5
  #define thermocoupleCSPin  12//4
  #define thermocoupleCLKPin 13//3
  #define lcdRsPin           8//9
  #define lcdEPin            9//10
  #define lcdD4Pin           4//A5
  #define lcdD5Pin           5//A4
  #define lcdD6Pin           6//A3
  #define lcdD7Pin           7//A2
  #define ledRedPin          A1//8
  #define buzzerPin          A2//7
  #define switchPin          A0
  #define lcdBrightnessPin   10
#else
  #define ssrPin             6
  #define thermocoupleSOPin  5
  #define thermocoupleCSPin  4
  #define thermocoupleCLKPin 3
  #define lcdRsPin           9
  #define lcdEPin            10
  #define lcdD4Pin           A5
  #define lcdD5Pin           A4
  #define lcdD6Pin           A3
  #define lcdD7Pin           A2
  #define ledRedPin          8
  #define buzzerPin          7
  #define switchPin          A0
#endif
int data = 0;

// ***** PID CONTROL VARIABLES *****
double setpoint;
double input;
double output;
double kp = PID_KP_PREHEAT;
double ki = PID_KI_PREHEAT;
double kd = PID_KD_PREHEAT;
const int windowSize = 2000;
unsigned long windowStartTime;
unsigned long nextCheck;
unsigned long nextRead;
unsigned long timerSoak;
unsigned long buzzerPeriod;

// ***** PID AUTOTUNE VARIABLES *****
byte ATuneModeRemember=2;
#define aTuneStep        1000
#define aTuneNoise       1
#define aTuneStartValue 1000;
unsigned int aTuneLookBack = 2;

// Reflow oven controller state machine state variable
reflowState_t reflowState;
// Reflow oven controller status
reflowStatus_t reflowStatus;
// Switch debounce timer
long lastDebounceTime;
// Seconds timer
long timerSeconds = 0;
long timeToCheck  = 20;
int checkTemperature = 0;

bool endOfPrevProcess = false;

int TEMPERATURE_PREHEAT_MIN = 40;
int TEMPERATURE_SOAK_MIN = 150;
int TEMPERATURE_SOAK_MAX = 177;
int TEMPERATURE_REFLOW_MAX = 230;

String type = "";

// ***** DEFINING OBJECTS *****
// Specify PID control interface
PID reflowOvenPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);
PID_ATune aTune(&input, &output);

// Specify LCD interface
LiquidCrystal lcd(lcdRsPin, lcdEPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);
Adafruit_MAX31855 thermocouple(thermocoupleCLKPin, thermocoupleCSPin, thermocoupleSOPin);

void setup()
{
  // SSR pin initialization to ensure reflow oven is off
  pinMode(ssrPin, OUTPUT);
  digitalWrite(ssrPin, LOW);

  // Buzzer pin initialization to ensure annoying buzzer is off
  digitalWrite(buzzerPin, LOW);
  pinMode(buzzerPin, OUTPUT);

  // LED pins initialization and turn on upon start-up (active low)
  pinMode(ledRedPin, OUTPUT);
  digitalWrite(ledRedPin, HIGH);

  // Tell the PID to range between 0 and the full window size
  reflowOvenPID.SetOutputLimits(0, windowSize);
  reflowOvenPID.SetSampleTime(PID_SAMPLE_TIME);

  // Start-up splash
  lcd.begin(16, 2);
  lcd.createChar(0, degree);
  lcd.clear();
  lcd.print("GFE Hand Made");
  lcd.setCursor(0, 1);
  lcd.print("Reflow Oven 1.2");

  // Serial communication at 57600 bps
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // Turn off LED (active low)
  digitalWrite(ledRedPin, LOW);

  // Welcome sound
  soundStart();
  lcd.clear();

  // Initialize time keeping variable
  nextCheck = millis();
  // Initialize thermocouple reading variable
  nextRead = millis();
  reflowState = REFLOW_STATE_IDLE;

  #ifdef USE_LCD_KEYPAD_SHIELD
    pinMode(lcdBrightnessPin, OUTPUT);
    analogWrite(lcdBrightnessPin, 125);
  #endif
}

void loop()
{
  // Current time
  unsigned long now;

  // Time to read thermocouple?
  if (millis() > nextRead)
  {
    // Read thermocouple next sampling period
    nextRead += SENSOR_SAMPLING_TIME;
    // Read current temperature
    input = thermocouple.readCelsius();

    // If thermocouple problem detected
    if (isnan(input))
    {
      // Illegal operation
      reflowState = REFLOW_STATE_ERROR;
      reflowStatus = REFLOW_STATUS_OFF;
    }
  }

  if (millis() > nextCheck)
  {
    // Check input in the next seconds
    nextCheck += 1000;
    // If reflow process is on going
    if (reflowStatus == REFLOW_STATUS_ON && reflowState != REFLOW_STATE_PRIOR)
    {
      // Toggle red LED as system heart beat
      digitalWrite(ledRedPin, !(digitalRead(ledRedPin)));
      // Increase seconds timer for reflow curve analysis
      timerSeconds++;
      
      // Send temperature and time stamp to serial as csv standard
      String dataToSend = "(" + (String)timerSeconds + "," + (String)setpoint + "," + (String)input + "," + (String)output + ")";
      char* buf = (char*) malloc(sizeof(char)*dataToSend.length()+1);

      dataToSend.toCharArray(buf, dataToSend.length()+1);
      Serial.println(buf);

      // Freeing the memory;
      free(buf);
    }
    else
    {
      // Turn off red LED
      digitalWrite(ledRedPin, HIGH);
    }

    // Clear LCD
    lcd.clear();
    lcd.setCursor(11, 1);
    if (reflowStatus == REFLOW_STATUS_ON)
    {
      lcd.print(type);
      // Print Stop Button
      lcd.setCursor(0, 1);
      lcd.print("stop");
      lcd.setCursor(6,1);
      lcd.print(timerSeconds);
      lcd.print(" s");
    }
    else
    {
      lcd.print("start");
      lcd.setCursor(0, 1);
      #ifdef USE_LCD_KEYPAD_SHIELD
        lcd.print("menu");
      #else
        lcd.print("prSet");
      #endif
    }

    // Print current system state
    lcd.setCursor(0, 0);
    lcd.print(lcdMessagesReflowStatus[reflowState]);
    // Move the cursor to the 2 line
    lcd.setCursor(9, 0);

    // If currently in error state
    if (reflowState == REFLOW_STATE_ERROR)
    {
      // No thermocouple wire connected
      lcd.print("TC Err");
    }
    else
    {
      // Print current temperature
      lcd.print(input);

      #if ARDUINO >= 100
        // Print degree Celsius symbol
        lcd.write((uint8_t)0);
      #else
        // Print degree Celsius symbol
        lcd.print(0, BYTE);
      #endif
      lcd.print("C ");
    }
  }

  // Reflow oven controller state machine
  switch (reflowState)
  {
    case REFLOW_STATE_IDLE:
      // If oven temperature is still above room temperature
      if (input >= TEMPERATURE_ROOM)
      {
        reflowState = REFLOW_STATE_TOO_HOT;
      }

      else
      {
        // If switch is pressed to start reflow process
        if (Serial.available() > 0)
        {
          //Leggo la stringa in arrivo dalla seriale.
          char dataIn[100] = {""};
          Serial.readBytes(dataIn, 100);
          String receivedString(dataIn);
          data = dataIn[0];
          //Serial.println("1 dataIn[0] = " + data);

          int iniPac = receivedString.indexOf('(');
          int endPac = receivedString.indexOf(')');

          //se ci sono i caratteri di inizio e fine pacchetto è un messaggio,
          //altrimenti ho ricevuto solo un dato da inserire in data.
          if (iniPac != -1 && endPac != -1 && endPac - iniPac > 1)
          {
            String packetIn = receivedString;
            packetIn = packetIn.substring(iniPac+1, endPac); //tolgo le parentesi

            int valoriIn[3] = {-100};
            for (int i=0; packetIn.indexOf(',') != -1; i++)
            {
              int index = packetIn.indexOf(',');
              valoriIn[i] = packetIn.substring(0, index).toInt();
              packetIn = packetIn.substring(index+1, packetIn.length());
            }

            TEMPERATURE_SOAK_MIN =    valoriIn[0];
            TEMPERATURE_SOAK_MAX =    valoriIn[1];
            TEMPERATURE_REFLOW_MAX =  valoriIn[2];

            type = " [C]";
            sendProfile();
          }

          else
          {
            data = dataIn[0];
            //Serial.println("2 dataIn[0] = " + data);
          }
        }
        
        if (analogRead(switchPin) < 30 || data == 10)
        {
          data = 0;
          #ifdef USE_LCD_KEYPAD_SHIELD
            delay(200);
          #endif

          if(TEMPERATURE_SOAK_MIN < 50)
            profileSet();
            
          //Saving data for heating check:
          checkTemperature = thermocouple.readCelsius();
          timeToCheck = 20;
          // Inizio il ciclo, mando i miei dati via seriale per settare il grafico.
          sendProfile();

          //while(input < 40) //PRIOR HEATING
          //{
          //  digitalWrite(ssrPin, HIGH);
          //}
          

          // Ora mando l'header per il file CSV
          Serial.println("Time Setpoint Input Output");
          // Intialize seconds timer for serial debug information
          nextCheck = millis();
          timerSeconds = 0;
          // Initialize PID control window starting time
          windowStartTime = millis();
          // Ramp up to minimum soaking temperature
          setpoint = TEMPERATURE_SOAK_MIN;
          // Turn the PID on
          reflowOvenPID.SetMode(AUTOMATIC);
          // Proceed to preheat stage
          reflowOvenPID.SetTunings(PID_KP_PREHEAT, PID_KI_PREHEAT, PID_KD_PREHEAT);
          reflowState = REFLOW_STATE_PRIOR; //prior heating to heat up the quartz elements
          //reflowState = REFLOW_STATE_PREHEAT;
        }
        else if (data == 35) //tuning preheat code = # -> ASCII: 35
        {
          data = 0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("TUNING PREHEAT");
          // Intialize seconds timer for serial debug information
          timerSeconds = 0;
          // Initialize PID control window starting time
          windowStartTime = millis();
          setpoint = 100;
          reflowOvenPID.SetMode(AUTOMATIC);
          reflowOvenPID.SetTunings(PID_KP_PREHEAT, PID_KI_PREHEAT, PID_KD_PREHEAT);

          //Setting autotuner:
          output=aTuneStartValue;
          aTune.SetNoiseBand(aTuneNoise);
          aTune.SetOutputStep(aTuneStep);
          aTune.SetLookbackSec((int)aTuneLookBack);
          AutoTuneHelper(true);
          type = " [PH]";

          //Going to autotuner mode:
          reflowState = REFLOW_STATE_TUNING_PH;
        }
      }
      break;

    case REFLOW_STATE_PRIOR:
      reflowStatus = REFLOW_STATUS_ON;
      if(input >= TEMPERATURE_PREHEAT_MIN)
      {
        reflowState = REFLOW_STATE_PREHEAT;
      }

    case REFLOW_STATE_PREHEAT:
      reflowStatus = REFLOW_STATUS_ON;
      // If minimum soak temperature is achieve
      if (input >= TEMPERATURE_SOAK_MIN)
      {
        // Chop soaking period into smaller sub-period
        timerSoak = millis() + SOAK_MICRO_PERIOD;
        // Set less agressive PID parameters for soaking ramp
        reflowOvenPID.SetTunings(PID_KP_SOAK, PID_KI_SOAK, PID_KD_SOAK);
        // Ramp up to first section of soaking temperature
        setpoint = TEMPERATURE_SOAK_MIN + SOAK_TEMPERATURE_STEP;
        // Proceed to soaking state
        reflowState = REFLOW_STATE_SOAK;
      }
      break;

    case REFLOW_STATE_SOAK:
      // If micro soak temperature is achieved
      if (millis() > timerSoak)
      {
        timerSoak = millis() + SOAK_MICRO_PERIOD;
        // Increment micro setpoint
        setpoint += SOAK_TEMPERATURE_STEP;
        if (setpoint > TEMPERATURE_SOAK_MAX)
        {
          // Set agressive PID parameters for reflow ramp
          reflowOvenPID.SetTunings(PID_KP_REFLOW, PID_KI_REFLOW, PID_KD_REFLOW);
          // Ramp up to first section of soaking temperature
          setpoint = TEMPERATURE_REFLOW_MAX;
          // Proceed to reflowing state
          reflowState = REFLOW_STATE_REFLOW;
        }
      }
      break;

    case REFLOW_STATE_REFLOW:
      // We need to avoid hovering at peak temperature for too long
      // Crude method that works like a charm and safe for the components
      if (input >= (TEMPERATURE_REFLOW_MAX - 5))
      {
        // Set PID parameters for cooling ramp
        reflowOvenPID.SetTunings(PID_KP_PREHEAT, PID_KI_PREHEAT, PID_KD_PREHEAT);
        // Ramp down to minimum cooling temperature
        setpoint = TEMPERATURE_COOL_MIN;
        // Proceed to cooling state
        reflowState = REFLOW_STATE_COOL;
      }

      break;

    case REFLOW_STATE_COOL:
      // If minimum cool temperature is achieve
      if (input <= TEMPERATURE_COOL_MIN)
      {
        // Turn off reflow process
        reflowStatus = REFLOW_STATUS_OFF;
        // Proceed to reflow Completion state
        reflowState = REFLOW_STATE_COMPLETE;
        endOfPrevProcess = true;
        Serial.print("eof");
      }
      break;

    case REFLOW_STATE_COMPLETE:
      // Process ended. Display time and sound song.
      lcd.clear();
      lcd.print("REFLOW DONE!");
      lcd.setCursor(0,1);
      lcd.print("time: ");
      lcd.setCursor(11,1);
      lcd.print(timerSeconds);
      lcd.print(" s");
      soundComplete();
      
      // Reflow process ended
      reflowState = REFLOW_STATE_IDLE;
    break;

    case REFLOW_STATE_TOO_HOT:
      // If oven temperature drops below room temperature
      if (input < TEMPERATURE_ROOM)
      {
        // Ready to reflow
        reflowState = REFLOW_STATE_IDLE;
      }
      break;

    case REFLOW_STATE_ERROR:
      // If thermocouple problem is still present

      if (isnan(input))
      {
        // Wait until thermocouple wire is connected
        reflowState = REFLOW_STATE_ERROR;
      }
      else
      {
        // Clear to perform reflow process
        reflowState = REFLOW_STATE_IDLE;
      }
      break;

   case REFLOW_STATE_TUNING_PH:
      reflowStatus = REFLOW_STATUS_ON;

      if(timerSeconds > 180)
      {
        reflowStatus = REFLOW_STATUS_OFF;
        reflowState  = REFLOW_STATE_IDLE;
        lcd.clear();
        lcd.print("ERROR, aborting");
        soundError();
      }
      
      byte val = (aTune.Runtime());
      if (val!=0)
      {
        kp = aTune.GetKp();
        ki = aTune.GetKi();
        kd = aTune.GetKd();
        reflowOvenPID.SetTunings(kp,ki,kd);

        //Turn off autotune
        aTune.Cancel();
        AutoTuneHelper(false);

        //Going to IDLE:
        reflowStatus = REFLOW_STATUS_OFF;
        reflowState = REFLOW_STATE_IDLE;

        lcd.clear();
        lcd.print("DONE");
        lcd.print(" ");
        lcd.print(kp);
        lcd.setCursor(0,1);
        lcd.print(ki);
        delay(1000);
      }
   break;
  }

  // If switch 1 is pressed
  if (Serial.available() > 0) data = Serial.read();
  if (analogRead(switchPin) > 400 && analogRead(switchPin) < 800 || data == 100)
  {
    #ifdef USE_LCD_KEYPAD_SHIELD
      delay(200);
    #endif
    data = 0;
    // If currently reflow process is on going
    if (reflowStatus == REFLOW_STATUS_ON)
    {
      // Button press is for cancelling
      // Turn off reflow process
      reflowStatus = REFLOW_STATUS_OFF;
      // Reinitialize state machine
      reflowState = REFLOW_STATE_IDLE;
    }
    else
      #ifdef USE_LCD_KEYPAD_SHIELD
        delay(200);
        menu_page();
     #else
        profileSet();
     #endif
  }
  else if (data == 36)
  {
    // Turn off reflow process
    reflowStatus = REFLOW_STATUS_OFF;
    // Reinitialize state machine
    reflowState = REFLOW_STATE_IDLE;
  }

  // PID computation and SSR control
  if (reflowStatus == REFLOW_STATUS_ON)
  {
    if (reflowState == REFLOW_STATE_PREHEAT && timeToCheck <= timerSeconds)  //check for SSR failures on PREHEAT stage
    {
      if (input < checkTemperature)
      {
        reflowStatus = REFLOW_STATUS_OFF;
        reflowState = REFLOW_STATE_IDLE;
        lcd.clear();
        lcd.print("ERROR  check");
        lcd.setCursor(0,1);
        lcd.print("Power or SSR");
        soundError();
      }
      
      timeToCheck = timerSeconds+20;
      checkTemperature = input + 0.5;
    }
    
    now = millis();

    if(reflowState != REFLOW_STATE_TUNING_PH)
      reflowOvenPID.Compute();

    if ((now - windowStartTime) > windowSize)
    {
      // Time to shift the Relay Window
      windowStartTime += windowSize;
    }
    if (output > (now - windowStartTime))
    {
      //Serial.println("ssrPin, HIGH");
      digitalWrite(ssrPin, HIGH);
    }
    else
    {
      digitalWrite(ssrPin, LOW);
      //Serial.println("ssrPin, LOW");
    }
  }
  // Reflow oven process is off, ensure oven is off
  else
  {
    digitalWrite(ssrPin, LOW);
    lcd.setCursor(7, 1);
    lcd.print(" ");
  }
}
