/*  
EQRAT-UNO
Simple Equatorial Mount RA Sidereal Tracker for Arduino UNO/328

By Charles Gershom 
@charlesgershom 

*/

//Debug
const bool debugEnabled = true;

//Mount config
const int Mount_Worm_Gear_Ratio_RA = 130; //RA SKYWATCHER EQ 3-2
const int Mount_Worm_Gear_Ratio_Dec = 65; //DEC SKYWATCHER EQ 3-2
const float Motor_Gear_Ratio_RA = 2.5; //30/12 - Asteron Drive Kit Light
const float Motor_Gear_Ratio_DEC = 3.0; //30/12 - Asteron Drive Kit Light
const int Steps_Per_Rev = 200;
const int Microstep_Setting = 32;

//Pin assignments
const int RAdirPin = 9;   
const int RAstepPin = 10;    
const int DECdirPin = 4;   
const int DECstepPin = 3;    
const int EnablePin = 12;   
const int MicrostepSelect = 11;   

//Sidereal math
const float Seconds_Earth_Rotate = 86164.09053;
const float processorSpeed = 16000000;

float Earth_Seconds_Per_Degree = Seconds_Earth_Rotate / 360.0;
float MicroSteps_Per_Degree = ((float)Mount_Worm_Gear_Ratio_RA * (float)Motor_Gear_Ratio_RA * (float)Steps_Per_Rev * (float)Microstep_Setting) / 360.0;
float Step_Delay_Microseconds = (Earth_Seconds_Per_Degree / MicroSteps_Per_Degree) * 1000000.0;
float Step_Delay_Timer_Half_Phase = Step_Delay_Microseconds / 2.0;

float OCR1A_Calc_Value = (processorSpeed / (((1.0 / Step_Delay_Timer_Half_Phase) * 1000000.0) * 8.0)) - 1.0;
unsigned int OCR1A_Value = round(OCR1A_Calc_Value);

//Stepper state
uint8_t raStepState = LOW;

//Debug timing
unsigned long lastTime = 0;
unsigned long currentTime = 0;
unsigned long stepCount = 0;

//Timer setup - generated with http://www.arduinoslovakia.eu/application/timer-calculator
void setupTimer1() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = OCR1A_Value;
  TCCR1B |= (1 << WGM12); //CTC mode
  TCCR1B |= (1 << CS11); //Prescaler 8
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt Enable
  interrupts();
}

void setup() {  
  Serial.begin(9600);
  Serial.println("Starting EQRAT");
  Serial.println("--------------");
  Serial.println("Timer Calc : " + String(OCR1A_Value));
  
  pinMode(RAstepPin, OUTPUT);   
  pinMode(RAdirPin, OUTPUT);    
  pinMode(DECstepPin, OUTPUT);   
  pinMode(DECdirPin, OUTPUT);    
  pinMode(EnablePin, OUTPUT);  
  pinMode(MicrostepSelect, OUTPUT);  
  
  digitalWrite(RAdirPin, HIGH);   //invert this (HIGH) if wrong direction    
  digitalWrite(EnablePin, LOW);  
  digitalWrite(MicrostepSelect, HIGH);  
  
  setupTimer1();
}   

ISR(TIMER1_COMPA_vect) {
  if(raStepState == LOW){
    raStepState = HIGH;
    digitalWrite(RAstepPin, raStepState); 
  }
  else{
    raStepState = LOW;
    digitalWrite(RAstepPin, raStepState); 
  }
  
  if(debugEnabled) {
    lastTime = currentTime;
    currentTime = micros();
    stepCount++;
  }
}

void loop() {  
  if(debugEnabled) {
    static unsigned long lastPrint = 0;
    if(millis() - lastPrint > 1000) {
      unsigned long timeDiff = (currentTime - lastTime) * 2;
      Serial.println("Step period: " + String(timeDiff) + "μs, Steps: " + String(stepCount));
      lastPrint = millis();
    }
  }
}   
