const uint8_t TX = 1;
const uint8_t RX = 0;
const uint8_t FOut = 11;
const uint8_t Dir = 9;

#define MARKING_PERIOD 9 //>8.33ms for standard marking period
const unsigned long TimeoutStandard = 380; //Standard timeout period for most commands is 380ms 

char ReadArray[25] = {0};

void setup() {
	pinMode(TX, OUTPUT);
	pinMode(FOut, OUTPUT);
	pinMode(Dir, OUTPUT);

	digitalWrite(Dir, HIGH); //Set to transmit 
	Serial1.begin(1200, SERIAL_7E1);
	SerialUSB.begin(115200);
}

void loop() {
	// digitalWrite(TX, LOW);
	// digitalWrite(FOut, LOW);
	// delay(1000);
	// digitalWrite(FOut, LOW);
	// digitalWrite(TX, HIGH);
	// delay(1000);
	// digitalWrite(FOut, HIGH);
	// digitalWrite(TX, LOW);
	// delay(1000);
	// digitalWrite(FOut, HIGH);
	// digitalWrite(TX, HIGH);
	// delay(1000);
	static int ReadLength = 0;
  	String ReadString;
	if(SerialUSB.available() > 0) {
    char Input = SerialUSB.read();

    // Increment counter while waiting for carrage return or newline
    if(Input != 13 || Input != 10) {
//      SerialUSB.println(Input); //DEBUG!
      ReadArray[ReadLength] = Input;
//      SerialUSB.println(ReadArray[ReadLength]); //DEBUG!
      ReadLength++;
    }

    if(Input == 13 || Input == 10) { // carriage or newline
//      SerialUSB.println("CARRAGE OR NEWLINE"); //DEBUG!
      ReadString = String(ReadArray);
//      SerialUSB.println(ReadString);
      ReadString.trim();
      memset(ReadArray, 0, sizeof(ReadArray));
      ReadLength = 0;

	// while(SerialUSB.peek() != '\n'); //Wait for new command
	// String NewCommand = SerialUSB.readStringUntil('\n');
    SerialUSB.print(">");
	SerialUSB.println(ReadString); //Echo back to SerialUSB monitor
	SerialUSB.println(SendCommand(ReadString));
	}
	// GetAddress();
	// delay(5000);

}
}

char GetAddress()
{
	SendBreak(); //Send break to start message
	Mark(MARKING_PERIOD); //Mark for standard period before command
	Serial1.begin(1200, SERIAL_7E1);
	Serial1.print("0I!");
	Serial1.flush(); //Make sure data is transmitted before releasing the bus
	delay(1);
	ReleaseBus(); //Switch bus to recieve 
	// char Var = SerialUSB.re

	unsigned long LocalTime = millis();
	// bool GotData = false;
	while(Serial1.available() < 3 && (millis() - LocalTime) <  TimeoutStandard);
	String Val = Serial1.readStringUntil('\r');
	SerialUSB.println(Val);
	return 0; //DEBUG!
}

void SendBreak()
{
	pinMode(Dir, OUTPUT); //Make sure direction is set to output
	pinMode(FOut, OUTPUT); //Make sure the force out pin is in output mode
	digitalWrite(Dir, HIGH); //Set direction to output
	digitalWrite(FOut, LOW); //Set low to force output high
	delay(13); //Wait for bus to acnowledge action
	digitalWrite(FOut, HIGH); //Stop forcing output
}

void Mark(unsigned long Period)
{
	pinMode(Dir, OUTPUT); //Make sure direction control pin is set as an output
	pinMode(TX, OUTPUT); //Make sure transmit pin is set as output
	digitalWrite(Dir, HIGH); //Set direction to output
	digitalWrite(TX, 1); //Begin marking condition
	delay(Period); //Wait for a given marking period
	digitalWrite(TX, 0); //Stop marking  
}

void Space(unsigned long Period)
{
	pinMode(Dir, OUTPUT); //Make sure direction control pin is set as an output
	pinMode(TX, OUTPUT); //Make sure transmit pin is set as output
	digitalWrite(Dir, HIGH); //Set direction to output
	digitalWrite(TX, 0); //Begin spacing condition
	delay(Period); //Wait for a given marking period
	digitalWrite(TX, 1); //Stop spacing
}

// void SendCommand()

void ReleaseBus() 
{
	pinMode(Dir, OUTPUT); //Make sure direction pin is set as an output
	digitalWrite(Dir, LOW); //Set direction to input
}

String SendCommand(String Command) 
{
	SendBreak(); //Send break to start message
	Mark(MARKING_PERIOD); //Mark for standard period before command
	Serial1.begin(1200, SERIAL_7E1);
	Serial1.print(Command);
	Serial1.flush(); //Make sure data is transmitted before releasing the bus
	delay(1);
	ReleaseBus(); //Switch bus to recieve 
	// char Var = SerialUSB.re

	unsigned long LocalTime = millis();
	char Data[100] = {0}; //Make data array for storage FIX! Change length to not be arbitrary
	bool GotData = false; //Used to keep track of terminating character has been recieved 
	int Pos = 0; //Keep track of position in data array
	while(!GotData && (millis() - LocalTime) <  TimeoutStandard) {
		if(Serial1.available() > 0) {
			Data[Pos] = Serial1.read(); //If byte is available, read it in
			Pos++; //Increment position
		}
		if(Data[Pos] == '\n') GotData = true; //Set flag if end character is read in
	}
	String Val = String(Data); //Convert to String
	Val.trim(); //Get rid of any trailing characters 
	// while(Serial1.available() < 3 && (millis() - LocalTime) <  TimeoutStandard);
	// String Val = Serial1.readStringUntil('\r');
	// SerialUSB.println(Val);
	return Val; 
}