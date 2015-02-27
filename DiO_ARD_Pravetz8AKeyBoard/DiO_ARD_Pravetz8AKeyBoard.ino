//**************************************************************//
//  Name    : shiftIn Example 2.1                               //
//  Author  : Carlyn Maw                                        //
//  Date    : 25 Jan, 2007                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a CD4021B Shift Register    	//
//          :                                                   //
//****************************************************************

//define where your pins are
char DataPin      = 8;
char LatchPin     = 7;
char ClockPin     = 6;
char F2Pin = 5;
char F1Pin = 4;
char AKDPin       = 2;
// It is pin 2, but it is interupt 0;
char StrobePin    = 1;

// Define variables to hold the data 
// for each shift register.
// starting with non-zero numbers can help
// troubleshoot
char KBData = 71;
volatile bool StateStrobe = false;
int StateAKD = 0;
int StateF2  = 0;
int StateF1  = 0;

void setup()
{

  // Set pin modes.
  pinMode(LatchPin,     OUTPUT);
  pinMode(ClockPin,     OUTPUT); 
  pinMode(DataPin,      INPUT);
  pinMode(F1Pin, INPUT);
  pinMode(F2Pin, INPUT);

  // Set interupts.
  attachInterrupt(StrobePin, StrobeCB, RISING);
  
  // Set serial port.
  Serial.begin(9600);
  
  // Tel about yourself. :)
  Serial.println("Hello, I am PRAVETS 8A KB.");
  
  // Initialize control over the keyboard:
  Keyboard.begin();
}

void loop()
{
  StateAKD = digitalRead(AKDPin);
  StateF2 = digitalRead(F2Pin);
  StateF1 = digitalRead(F1Pin);
  
  if(StateAKD == HIGH && StateStrobe == true)
  {
    StateStrobe = false;
    
    //while the shift register is in serial mode
    //collect each shift register into a byte
    //the register attached to the chip comes in first 
    KBData = ShiftIn(LatchPin, DataPin, ClockPin);
  
    //Print out the results.
    //leading 0's at the top of the byte 
    //(7, 6, 5, etc) will be dropped before 
    //the first pin that has a high input
    //reading
    //Serial.println(KBData, BIN);
    Serial.print(KBData, DEC);
    Serial.print(": ");
    Serial.println(KBData);
    Keyboard.print(KBData);
  }
  
  if(StateF2 == HIGH)
  {
    Serial.println("F2Pin");  
  }
  
  if(StateF1 == HIGH)
  {
    Serial.println("F1Pin");  
  }
  
  if((StateAKD == HIGH && StateStrobe == true) || (StateF2 == HIGH) || (StateF1 == HIGH))
  {
	//delay so all these print satements can keep up. 
    delay(40);
  }
}

//////////////////////////////////////////////////////////////////////
// just needs the location of the data pin and the clock pin
// it returns a byte with each bit in the byte corresponding
// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0
//////////////////////////////////////////////////////////////////////
char ShiftIn(int myLatchPin, int myDataPin, int myClockPin)
{
  int bitIndex;
  int temp = 0;
  int pinState;
  char dataIn = 0;

  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(myLatchPin, HIGH);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(myLatchPin, LOW);

  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop
  
  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's DataPin to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
  for (bitIndex = 7; bitIndex >= 0; bitIndex--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(2);
    temp = digitalRead(myDataPin);
    if (temp)
	{
      pinState = 1;
      //set the bit to 0 no matter what
      dataIn = dataIn | (1 << bitIndex);
    }
    else
	{
      //turn it off -- only necessary for debuging
     //print statement since dataIn starts as 0
      pinState = 0;
    }

    digitalWrite(myClockPin, HIGH);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(dataIn, BIN);
  return dataIn;
}

void StrobeCB()
{
  StateStrobe = true;
}
