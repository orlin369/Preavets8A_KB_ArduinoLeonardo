//**************************************************************//
//  Name    : Pravets 8A Keyboard Controller                    //
//  Author  : Orlin Dimitrov                                    //
//  Date    : 27 Feb, 2015                                      //
//  Version : 1.0                                               //
//  Notes   : Reading Pravets 8A keyboard                       //
//          :                                                   //
//****************************************************************

//define where your pins are
char PinData      = 8;
char PinLatch     = 7;
char ClockPin     = 6;
char PinF2        = 5;
char PinF1        = 4;
char PinAKD       = 2;
// It is pin 2, but it is interupt 0;
char PinStrobe    = 1;

// Define variables to hold the data 
// for each shift register.
// starting with non-zero numbers can help
// troubleshoot
char KBData = 42;
volatile bool StateStrobe = false;
int StateAKD = 0;
int StateF2  = 0;
int StateF1  = 0;

void setup()
{

  // Set pin modes.
  pinMode(PinLatch, OUTPUT);
  pinMode(ClockPin, OUTPUT); 
  pinMode(PinData,  INPUT_PULLUP);
  pinMode(PinF1,    INPUT_PULLUP);
  pinMode(PinF2,    INPUT_PULLUP);

  // Set interupts.
  attachInterrupt(PinStrobe, StrobeCB, RISING);
  
  // Set serial port.
  Serial.begin(9600);
  
  // Tel about yourself. :)
  Serial.println("Hello, I am PRAVETS 8A KB.");
  
  // Initialize control over the keyboard:
  Keyboard.begin();
}

void loop()
{
  StateAKD = digitalRead(PinAKD);
  StateF2 = digitalRead(PinF2);
  StateF1 = digitalRead(PinF1);
  
  if(StateAKD == HIGH && StateStrobe == true)
  {
    StateStrobe = false;
    
    //while the shift register is in serial mode
    //collect each shift register into a byte
    //the register attached to the chip comes in first 
    KBData = ShiftIn(PinLatch, PinData, ClockPin);
  
    //Print out the results.
    //leading 0's at the top of the byte 
    //(7, 6, 5, etc) will be dropped before 
    //the first pin that has a high input
    //reading
    //Serial.println(KBData, BIN);
    Serial.print(KBData, DEC);
    Serial.print(": ");
    Serial.println(KBData);

    switch(KBData)
    {
      // Left
      case 8:
        Keyboard.press(KEY_LEFT_ARROW);
        Keyboard.release(KEY_LEFT_ARROW);
        break;
        
      // Right
      case 21:
        Keyboard.press(KEY_RIGHT_ARROW);
        Keyboard.release(KEY_RIGHT_ARROW);
        break;
      
      // Up
      case 11:
        Keyboard.press(KEY_UP_ARROW);
        Keyboard.release(KEY_UP_ARROW);
        break;
      
      // Down
      case 10:
        Keyboard.press(KEY_DOWN_ARROW);
        Keyboard.release(KEY_DOWN_ARROW);
        break;

      // Delete
      case 127:
        Keyboard.press(KEY_DELETE);
        Keyboard.release(KEY_DELETE);
        break;
  
      // Tab
      case 9:
        Keyboard.press(KEY_TAB);
        Keyboard.release(KEY_TAB);
        break;

      // Escape
      case 27:
        Keyboard.press(KEY_ESC);
        Keyboard.release(KEY_ESC);
        break;

      default:
        //
        Keyboard.print(KBData);
        break;
    }

  }
  
  if(StateF2 == HIGH)
  {
    Serial.println("PinF2"); 

    //Press F2.
    Keyboard.press(KEY_F2);
    Keyboard.release(KEY_F2);
  }
  
  if(StateF1 == HIGH)
  {
    Serial.println("PinF1");  

    // Press F1.
    Keyboard.press(KEY_F1);
    Keyboard.release(KEY_F1);
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
char ShiftIn(char pinLatch, char pinData, char pinClock)
{
  char bitIndex;
  int temp = 0;
  int pinState;
  char dataIn = 0;

  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(pinLatch, HIGH);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(pinLatch, LOW);

  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop
  
  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's PinData to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
  for (bitIndex = 7; bitIndex >= 0; bitIndex--)
  {
    digitalWrite(pinClock, 0);
    delayMicroseconds(2);
    temp = digitalRead(pinData);
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

    digitalWrite(pinClock, HIGH);
  }

  return dataIn;
}

void StrobeCB()
{
  StateStrobe = true;
}
