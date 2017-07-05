
/*
   Revision  Description
   ========  ===========
   1.1       Arduino Nano v3 for GFE Hand Made Reflow Oven Controller board v1.1
             - Compiled for Arduino Nano.
             - Display changed to 16x2 Hitachi HD44780 Compatible.
             - added data output to diplay temperature vs time.
             - added possibility to choose between lead free of leaded solder paste profiles.

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
#include <Adafruit_MAX31855.h>
#include <PID_v1.h>


// ***** CONSTANTS *****
#define TEMPERATURE_ROOM 50

#define TEMPERATURE_COOL_MIN 100
#define SENSOR_SAMPLING_TIME 1000
#define SOAK_TEMPERATURE_STEP 5
#define SOAK_MICRO_PERIOD 9000
#define DEBOUNCE_PERIOD_MIN 50


// ***** PID PARAMETERS *****
// Pre-Heating:
#define PID_KP_PREHEAT 100
#define PID_KI_PREHEAT 0.025
#define PID_KD_PREHEAT 20
// Soaking:
#define PID_KP_SOAK 300
#define PID_KI_SOAK 0.05
#define PID_KD_SOAK 250
// Reflowing:
#define PID_KP_REFLOW 300
#define PID_KI_REFLOW 0.05
#define PID_KD_REFLOW 350

// Sample time:
#define PID_SAMPLE_TIME 1000

// ***** TYPE DEFINITIONS *****
typedef enum REFLOW_STATE
{
  REFLOW_STATE_IDLE,
  REFLOW_STATE_PREHEAT,
  REFLOW_STATE_SOAK,
  REFLOW_STATE_REFLOW,
  REFLOW_STATE_COOL,
  REFLOW_STATE_COMPLETE,
  REFLOW_STATE_TOO_HOT,
  REFLOW_STATE_ERROR
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
  "Preheat",
  "Soak",
  "Reflow",
  "Cool",
  "Complete",
  "Wait,hot",
  "Error"
};


// ***** DEGREE SYMBOL FOR LCD *****
unsigned char degree[8]  = { 140, 146, 146, 140, 128, 128, 128, 128 };


// ***** PIN ASSIGNMENT *****
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
int data = 0;


// ***** PID CONTROL VARIABLES *****
double setpoint;
double input;
double output;
double kp = PID_KP_PREHEAT;
double ki = PID_KI_PREHEAT;
double kd = PID_KD_PREHEAT;
int windowSize;
unsigned long windowStartTime;
unsigned long nextCheck;
unsigned long nextRead;
unsigned long timerSoak;
unsigned long buzzerPeriod;

// Reflow oven controller state machine state variable
reflowState_t reflowState;
// Reflow oven controller status
reflowStatus_t reflowStatus;
// Switch debounce timer
long lastDebounceTime;
// Seconds timer
int timerSeconds = 0;

bool endOfPrevProcess = false;

int TEMPERATURE_SOAK_MIN = 0;
int TEMPERATURE_SOAK_MAX = 0;
int TEMPERATURE_REFLOW_MAX = 0;

String type = "";

// ***** DEFINING OBJECTS *****
// Specify PID control interface
PID reflowOvenPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);
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

  // Start-up splash
  digitalWrite(buzzerPin, HIGH);
  lcd.begin(16, 2);
  lcd.createChar(0, degree);
  lcd.clear();
  lcd.print("GFE Hand Made");
  lcd.setCursor(0, 1);
  lcd.print("Reflow Oven 1.1");

  digitalWrite(buzzerPin, LOW);
  delay(2000);
  lcd.clear();

  // Serial communication at 57600 bps
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // Turn off LED (active low)
  digitalWrite(ledRedPin, LOW);

  profileSet();

  // Set window size
  windowSize = 2000;
  // Initialize time keeping variable
  nextCheck = millis();
  // Initialize thermocouple reading variable
  nextRead = millis();
}

void loop() {
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
    if (reflowStatus == REFLOW_STATUS_ON)
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
    }
    else
    {
      lcd.print("start");
      lcd.setCursor(0, 1);
      lcd.print("prSet");
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
        if (Serial.available() > 0) data = Serial.read();

        if (analogRead(switchPin) < 30 || data == 10)
        {
          data = 0;
          if (endOfPrevProcess == true)
            profileSet();
          else
          {
            // Send header for CSV file
            Serial.println("Time Setpoint Input Output");
            // Intialize seconds timer for serial debug information
            timerSeconds = 0;
            // Initialize PID control window starting time
            windowStartTime = millis();
            // Ramp up to minimum soaking temperature
            setpoint = TEMPERATURE_SOAK_MIN;
            // Tell the PID to range between 0 and the full window size
            reflowOvenPID.SetOutputLimits(0, windowSize);
            reflowOvenPID.SetSampleTime(PID_SAMPLE_TIME);
            // Turn the PID on
            reflowOvenPID.SetMode(AUTOMATIC);
            // Proceed to preheat stage
            reflowState = REFLOW_STATE_PREHEAT;
          }
        }
      }
      break;

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
        reflowOvenPID.SetTunings(PID_KP_REFLOW, PID_KI_REFLOW, PID_KD_REFLOW);
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
        // Retrieve current time for buzzer usage
        buzzerPeriod = millis() + 1000;
        // Turn on buzzer and green LED to indicate completion

        digitalWrite(buzzerPin, HIGH);
        // Turn off reflow process
        reflowStatus = REFLOW_STATUS_OFF;
        // Proceed to reflow Completion state
        reflowState = REFLOW_STATE_COMPLETE;
        endOfPrevProcess = true;
        Serial.print("eof");
      }
      break;

    case REFLOW_STATE_COMPLETE:
      if (millis() > buzzerPeriod)
      {
        // Turn off buzzer and green LED
        digitalWrite(buzzerPin, LOW);
        // Reflow process ended
        reflowState = REFLOW_STATE_IDLE;
      }
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
  }

  // If switch 1 is pressed
  if (Serial.available() > 0) data = Serial.read();
  if (analogRead(switchPin) > 400 && analogRead(switchPin) < 800 || data == 100)
  {
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
      profileSet();
  }

  // PID computation and SSR control
  if (reflowStatus == REFLOW_STATUS_ON)
  {
    now = millis();

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
