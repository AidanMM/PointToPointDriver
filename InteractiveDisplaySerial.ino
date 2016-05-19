//Pin connected to ST_CP of 74HC595 (SRCLCK)
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
//Output enable(Set to low to turn on output)
int enablePin = 9; 

//Set up pins 4, 5, and 6 to be read switches
int topState = 0;
int bottomState = 1;

String inputString = "";
boolean stringComplete = false;
int stringLength = 0;
bool shiftArray[8] = {false, false, false, false, false, false, false, false};
int counterArray[8] = {0,0,0,0,0,0,0,0};

int countTime = 100;
int state = 1;
int changeCounter = 0;

//Vibrating disc layout is counterclockwise from the top left
//1 - - 8 - - 7
//2 - - - - - 6
//3 - - 4 - - 5

void serialEventRun(void){
  if(Serial.available()) serialEvent();
}

void setup() {
  //set pins to output because they are used in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(enablePin, 0);

  //Initialize serial communication
  Serial.begin(9600);
  //Reserve 200 btes for the input string
  inputString.reserve(200);
}

void loop() {
  //this code utilizes arduino serialEvent() which is called in between loop.
  //serialEvent() does not appear to be an ISR according to arduino documentation.
  if(stringComplete) {
    for(int i = 0; i < stringLength; i++) {
      switch(inputString[i])
      {
        case '1':
          counterArray[0] = countTime;
          break;
        case '2':
          counterArray[1] = countTime;
          break;
        case '3':
          counterArray[2] = countTime;
          break;
        case '4':
          counterArray[3] = countTime;
          break;
        case '5':
          counterArray[4] = countTime;
          break;
        case '6':
          counterArray[5] = countTime;
          break;
        case '7':
          counterArray[6] = countTime;
          break;
        case '8':
          counterArray[7] = countTime;
          break;
          
      }
    }
    
    // clear the string
    inputString = "";
    stringComplete = false;
    stringLength = 0;
  } /*else {
    clearRegisters();
  }*/
  digitalWrite(enablePin, 1);
    for(int i = 7; i >= 0; i--)
    {
      if(counterArray[i] > 0)
      {
        digitalWrite(dataPin, 1);
        counterArray[i]--;
      } else {
        digitalWrite(dataPin, 0);
      }
      //Pulse the clock regardless of value so that each value is in the correct spot.
        pulseClock();
    }
    pulseClock();
    digitalWrite(enablePin, 0);
    delay(1);
    int top = analogRead(topState);
    int bottom = analogRead(bottomState);
    /*Serial.print("Top: ");
    Serial.print(top);
    Serial.print(", Bottom: ");
    Serial.println(bottom);*/
    int newState;
    if(top > 1000)
    {
      newState = 0;
    } else if(bottom > 1000) {
      newState = 2;
    } else {
      newState = 1;
    }
    if(state != newState)
    {
      state = newState;
      changeCounter = 100;
    }
    if(changeCounter > 0)
    {
      changeCounter--;
      if(changeCounter == 0)
      {
        if(state == 0)
        {
          Serial.write('0');
        } else if(state == 2) {
          Serial.write('2');
        } else {
          Serial.write('1');
        }
        Serial.write('\n');
      }
    }
    
    /*if(digitalRead(topState) == 1){
      Serial.write('3');
    }
    if(digitalRead(midState) == 1){
      Serial.write('2');
    }
    if(digitalRead(bottomState) == 1){
      Serial.write('1');
    }*/
}

void serialEvent() {
  while(Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    //If the incoming character is a newline, set a flag for the main loop to process it.
    if(inChar == '\n') {
      stringComplete = true;
    } else {
      //add to the string length so we know to ignore the \n
      stringLength++;
    }
  }
}

void pulseClock()
{
  digitalWrite(latchPin, 0);
  digitalWrite(clockPin, 0);
  digitalWrite(clockPin, 1);
  digitalWrite(latchPin, 1);
}

void pulseClock(int latchPin)
{
  digitalWrite(latchPin, 0);
  digitalWrite(clockPin, 0);
  digitalWrite(clockPin, 1);
  digitalWrite(latchPin, 1);
}

void pulseClock(int latchPin, int otherLatchPin)
{
  digitalWrite(latchPin, 0);
  digitalWrite(otherLatchPin, 0);
  digitalWrite(clockPin, 0);
  digitalWrite(clockPin, 1);
  digitalWrite(latchPin, 0);
  digitalWrite(otherLatchPin, 1);
}

void clearRegisters() 
{
  digitalWrite(enablePin, 1);
  for(int i  = 0; i < 8; i++) {
    digitalWrite(dataPin, 0);
    pulseClock();
  }
  digitalWrite(enablePin, 0);
}
