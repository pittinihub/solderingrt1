// Inclusions of Logo and Definitions
#include "SolderingStationLogo.h"
#include "SolderingStationParameters.h"

// Include Libraries
#include <TFT.h>
//#include <Adafruit_ST7735.h>
#include <SPI.h>
//#include <TimerThree.h>
#include <FlexiTimer2.h>
#include <AutoPID.h>
#include "EEPROM.h"


// Create instance for the library
TFT TFTscreen = TFT(DisplayCS, DisplayDC, DisplayRST);

//Varibles for PID
double TempMeas1, setPoint1, OutputVal1=0, OutputVal1Old=0;
double TempMeas2, setPoint2, OutputVal2=0, OutputVal2Old=0;
float KP = KPconst, KI = KIconst, KD = KDconst;
//Create two PID instances
AutoPID PIDch1(&TempMeas1, &setPoint1, &OutputVal1, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPID PIDch2(&TempMeas2, &setPoint2, &OutputVal2, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

//LCD printing offset and colours
const int Colour1[]={255,255,0}; //RGB
const int Colour2[]={0,255,0}; //RGB
const int Colour3[]={255,255,255}; //RGB
const int ChannelXOffset[]={5,85};

//Variables
int RotaryCounter = 0; 
int InterruptCounter = 0; 
int InterruptCounterS1 = 0;
int InterruptCounterS2 = 0;
bool Channel1State = 1;
bool Channel2State = 1;
unsigned int ChannelState[] = {1,1};
int aState;
int aLastState;
int aStateSW;
int aLastStateSW;
int aButtonState;
int Button1State = 1;
int Button2State = 1;
unsigned int SetTemperature[] = {SetTemperatureCh1,SetTemperatureCh2};
unsigned int PresetTemperature[] = {PresetTemperature1,PresetTemperature2};
unsigned int MeasuredTemperature[] = {0,0};
unsigned int MeasuredTemperatureADCvalue[] = {0,0};
unsigned int MaxDutyCycle = PresetMaxDutyCycle;
unsigned int VinUVLO = PresetVinUVLO;
int InSetMode = 0;
int SelectedChannel = 0;
int RefreshScreen = 0;
int RefreshScreenTemperature = 0;
int PreviousChannel = 0;
unsigned int InChannelSettings = 0;
unsigned int FirstBoot = 1;
float InputVoltage = 12.0;
int IncomingChar = 0;
int InternalTemperatureADC = 0;
float InternalTemperature = 0.0;
float Value = 0.0;
//Variables for serial communication
bool DebugMode = 0;
char IncomingCmd[100];
int CmdIndex;

 
void setup() {

	pinMode (EncoderA,INPUT_PULLUP);
	pinMode (EncoderB,INPUT_PULLUP);
	pinMode (EncoderSW,INPUT_PULLUP);
	pinMode (TemperatureSensor1Pin,INPUT);
	pinMode (TemperatureSensor2Pin,INPUT);
	pinMode (InputVoltagePin,INPUT);
	pinMode (InternalTemperaturePin,INPUT);
	pinMode (SwitchButton1,INPUT_PULLUP);
	pinMode (SwitchButton2,INPUT_PULLUP);
	pinMode (Heater1Pin, OUTPUT);
	pinMode (Heater2Pin, OUTPUT);

	Serial.begin (9600);   


	//initialize the library
	TFTscreen.begin();

	// clear the screen with a black background
	TFTscreen.background(0, 0, 0);
	TFTscreen.setTextSize(1);
	TFTscreen.stroke(255,255,0);//red,green,blue
	TFTscreen.setCursor(10+XOffset,20+YOffset);
	TFTscreen.println("Soldering RT1");
	TFTscreen.println(" ");
	TFTscreen.setTextSize(3);
	TFTscreen.setCursor(10+XOffset,35+YOffset);
	TFTscreen.println(SwVersion);
	TFTscreen.setTextSize(1);
	TFTscreen.setCursor(30+XOffset,80+YOffset);
	TFTscreen.println(Author);
  if (UserName){TFTscreen.setCursor(30+XOffset,95+YOffset); TFTscreen.print("Property of"); TFTscreen.setCursor(30+XOffset,105+YOffset);TFTscreen.println(User);}
	//delay(1500);
	//TFTscreen.background(0, 0, 0);
	//TFTscreen.drawBitmap(5,5,stationLOGO1,124,47,23432);
	delay(500);
	TFTscreen.background(0, 0, 0);

	FirstBoot = EEPROM.read(EEPROM_address);
	if (FirstBoot==0){
		//It's not the first boot
		//Load default values
		LoadParameters();
		//Check that data are not corrupted
		if ((PresetTemperature[0]<MinimumTemperature)|(PresetTemperature[0]>MaximumTemperature)){
			InitializeEEPROM();
		}
		if ((PresetTemperature[1]<MinimumTemperature)|(PresetTemperature[1]>MaximumTemperature)){
			InitializeEEPROM();
		}
    if (MaxDutyCycle>255){
      InitializeEEPROM();
    }   
		//Now everything good, we can set the set-points
		//Load the channels to de default parameters
		SetTemperature[0] = PresetTemperature[0];
		SetTemperature[1] = PresetTemperature[1];
	} else {
		//Write the memory cells, as content is unknown
		InitializeEEPROM();
	}

	//Print Soldering Iron Channels T1 and T2
	PrintChannel(Colour1,ChannelXOffset[0],1);
	PrintChannel(Colour2,ChannelXOffset[1],2);
	PrintPresetTemperatures();

	// Reads the initial state of the EncoderA
	aLastState = digitalRead(EncoderA);
	//PWM frequency
	/*  Setting   Divisor   Frequency
	0x01    1     31372.55 //MOSFET gets TOO HOT
	0x02    8     3921.16 //TOO high noise pitch
	0x03      64    490.20   <--DEFAULT
	0x04      256     122.55
	0x05    1024    30.64
	TCCR1B = (TCCR1B & 0b11111000) | <setting>; */
	TCCR1B = (TCCR1B & 0b11111000) | 0x04;
	//Setup interrupt
	FlexiTimer2::set(2, InterruptMain); // Good was : 2 (period in ms)
	FlexiTimer2::start();

	//Ensure Heaters are OFF
	digitalWrite(Heater1Pin,0);
	digitalWrite(Heater2Pin,0);

	//Configure PIDs
	//If temperature is more than 50 degrees below or above setpoint, OUTPUT will be set to min or max respectively
	PIDch1.setBangBang(BangBangHysteresis);
	PIDch2.setBangBang(BangBangHysteresis);
	//set PID update interval to XXX ms
	PIDch1.setTimeStep(10);
	PIDch2.setTimeStep(10);
	
	delay(100);
	//Read input voltage and calculate max duty
	//InputVoltage = analogRead(InputVoltagePin)*InputVoltageScaling;
}

void loop() {
	delay(100); 
	UpdatePWM();
	ExecuteControl();
	ReadInternalTemperature();
	ExecuteStateMachine();
	UpdateScreen();
	ExecuteSerialCommunication();
	if (DebugMode) { SerialDebug(); }
}


void ExecuteControl(){
	//Keep track of previous duty cycles for display update
	OutputVal1Old = OutputVal1;
	OutputVal2Old = OutputVal2;
	StopPWM();
	delay(DelayMeasurementms);
	//Read input voltage and calculate max duty
	InputVoltage = analogRead(InputVoltagePin)*InputVoltageScaling;
	//Update PIDs setponts and limits
	PIDch1.setOutputRange(0,int(MaxDutyCycle));
	PIDch2.setOutputRange(0,int(MaxDutyCycle));
	//Read Temperatures
	ReadTemperatures();
	//Update PID setpoints
	setPoint1 = SetTemperature[0];
	setPoint2 = SetTemperature[1];
	//Compute both PIDs and Write Outputs
	UpdatePWM();
}

void ReadInternalTemperature(){
	InternalTemperature = 0.0;
	InternalTemperatureADC = analogRead(InternalTemperaturePin);
	Value = InternalTemperatureADC;
	Value = 1023 / Value - 1;
	Value = NTCSerieResistor / Value;
	InternalTemperature = Value / NTCResistance;     // (R/Ro)
	InternalTemperature = log(InternalTemperature); // ln(R/Ro)
	InternalTemperature /= NTCBeta;                   // 1/B * ln(R/Ro)
	InternalTemperature += 1.0 / (NTCRefTemperature + 273.15); // + (1/To)
	InternalTemperature = 1.0 / InternalTemperature;                 // Invert the value
	InternalTemperature -= 273.15;                         // Convert it to Celsius
}

void StopPWM(){
	analogWrite(Heater1Pin, 0);
	analogWrite(Heater2Pin, 0);
}

void UpdatePWM(){
	//Enable the PWM only if the channel is active
	if (ChannelState[0]==1)  //Checks that a solering probe is conencted to the station, if not T>500 (ADC saturated)
	{	if (MeasuredTemperature[0]<TemperatureDisconnectSensing) {
			PIDch1.run();
			analogWrite(Heater1Pin, OutputVal1);
		} else {
			ToggleChannelONOFF(1);
			PIDch1.reset();
		}
	} else if ((ChannelState[0]==2)&(MeasuredTemperature[0]<TemperatureDisconnectSensing)) {
			//Channel was disconnected but now is connected, change state to OFF
			SwitchChannelOFF(1);
		} else if((ChannelState[0]==0)&(MeasuredTemperature[0]>=TemperatureDisconnectSensing)) 
			{ //The heater has been disconnected
				SwitchChannelNC(1);
			} 
			else {//Disable PID - Reset
				PIDch1.reset();
			}


	//Enable the PWM only if the channel is active
	if (ChannelState[1]==1)  //Checks that a solering probe is conencted to the station, if not T>500 (ADC saturated)
	{	if (MeasuredTemperature[1]<TemperatureDisconnectSensing) {
			PIDch2.run();
			analogWrite(Heater2Pin, OutputVal2);
		} else {
			ToggleChannelONOFF(2);
			PIDch2.reset();
		}
	} else if ((ChannelState[1]==2)&(MeasuredTemperature[1]<TemperatureDisconnectSensing)) {
			//Channel was disconnected but now is connected, change state to OFF
			SwitchChannelOFF(2);
		} else if((ChannelState[1]==0)&(MeasuredTemperature[1]>=TemperatureDisconnectSensing))
			{ //The heater has been disconnected
				SwitchChannelNC(2);
			} 
			else {//Disable PID - Reset
				PIDch2.reset();
			}
	
}

void ReadTemperatures(){
	MeasuredTemperatureADCvalue[0] = analogRead(TemperatureSensor1Pin);
	MeasuredTemperatureADCvalue[1] = analogRead(TemperatureSensor2Pin);
	MeasuredTemperature[0] = MeasuredTemperatureADCvalue[0]*ADCtoTemperatureGain+ADCtoTemperatureOffset;
	MeasuredTemperature[1] = MeasuredTemperatureADCvalue[1]*ADCtoTemperatureGain+ADCtoTemperatureOffset;
	if ((TempMeas1!=MeasuredTemperature[0])||(TempMeas2!=MeasuredTemperature[1])) {
		//Update measured temperature Ch.1
		RefreshScreenTemperature = 1;
	}
	TempMeas1 = MeasuredTemperature[0];
	TempMeas2 = MeasuredTemperature[1];
}

void SetChannelTemperature(){
    if (SelectedChannel>0){
		if (SelectedChannel==1){
			SetTemperature[0] = SetTemperature[0] + RotaryCounter;
			//Check temperature ranges
			if (SetTemperature[0] > MaximumTemperature){
				SetTemperature[0] = MinimumTemperature;
			}
			else if (SetTemperature[0] < MinimumTemperature){
				SetTemperature[0] = MaximumTemperature;
			}
		}
		else {
			SetTemperature[1] = SetTemperature[1] + RotaryCounter;
			//Check temperature ranges
			if (SetTemperature[1] > MaximumTemperature){
				SetTemperature[1] = MinimumTemperature;
			}
			else if (SetTemperature[1] < MinimumTemperature){
				SetTemperature[1] = MaximumTemperature;
			}
		}
		if (RotaryCounter!=0) {
			// Update screen only if knob is rotated
			PrintSetTemperature(Colour1,ChannelXOffset[0],1);
			PrintSetTemperature(Colour2,ChannelXOffset[1],2);
			RotaryCounter = 0;
		}
		//PrintSetTemperature(Colour1,ChannelXOffset[0],1);
		//PrintSetTemperature(Colour2,ChannelXOffset[1],2);
    }
}

void SwitchChannelOFF(int Ch){
  //Serial.println("Channel OFF");
  if (Ch==1){
    ChannelState[0] = 0;
	PrintChannelState(Colour1,ChannelXOffset[0],1);
  } else if (Ch==2){
    ChannelState[1] = 0;
	PrintChannelState(Colour2,ChannelXOffset[1],2);
  }
}

void SwitchChannelNC(int Ch){
  //Serial.println("Channel OFF");
  if (Ch==1){
    ChannelState[0] = 2;
	PrintChannelState(Colour1,ChannelXOffset[0],1);
  } else if (Ch==2){
    ChannelState[1] = 2;
	PrintChannelState(Colour2,ChannelXOffset[1],2);
  }
}

void SwitchChannelON(int Ch){
  //Serial.println("Channel ON");
  if (Ch==1){
    ChannelState[0] = 1;
	PrintChannelState(Colour1,ChannelXOffset[0],1);
  } else if (Ch==2){
    ChannelState[1] = 1;
	PrintChannelState(Colour2,ChannelXOffset[1],2);
  }
}

void ToggleChannelONOFF(int Ch){
  //Serial.println("Channel ON/OFF Toggle");
  if (Ch==1){
	  if (ChannelState[0]==1){
		  ChannelState[0] = 0;
	  } else if (ChannelState[0]==0) {
		  ChannelState[0] = 1;
	  }
	PrintChannelState(Colour1,ChannelXOffset[0],1);
  } else if (Ch==2){
	  if (ChannelState[1]==1){
		  ChannelState[1] = 0;
	  } else if (ChannelState[1]==0) {
		  ChannelState[1] = 1;
	  }
	PrintChannelState(Colour2,ChannelXOffset[1],2);
  }
}

void ExecuteStateMachine(){
	if (SelectedChannel>0){
		//Set temperature via the rotary encoder
		SetChannelTemperature();
		//If rotary knob is pushed and channel is selected, toggle channel state (ON/OFF)
		if((SelectedChannel==1)&&(InterruptCounter>ButtonsPressLatch)){
		  ToggleChannelONOFF(1);
		  InterruptCounter = 0;
		}
		if((SelectedChannel==2)&&(InterruptCounter>ButtonsPressLatch)){
		  ToggleChannelONOFF(2);
		  InterruptCounter = 0;
		}
	}
  
  //Button 1 (LEFT)
  if((Button1State)&&(InterruptCounterS1>0)){
    if(SelectedChannel==1){
      //VeryLongPress /LongPress /ShortPress
      if (InterruptCounterS1>ButtonsVeryLongPress){
			//VeryLongPress: Program the Preset Temperature
			PresetTemperature[0] = SetTemperature[0];
			//Serial.print("Programmed1!!!");
			SaveParameters();
			PrintPresetTemperatures();
        } else if (InterruptCounterS1>ButtonsLongPress){
			//LongPress: Load Preset1 on Channel 1
			SetTemperature[0] = PresetTemperature[0];
			//Serial.print("Loaded1");
			RefreshScreen = 1;
        } else {
			//ShortPress
			//Exit channel 1
			SelectedChannel = 0;
			RefreshScreen = 1;
        }
    } else if(SelectedChannel==2) { //No Channel 1 selected
      //VeryLongPress /LongPress /ShortPress
      if (InterruptCounterS1>ButtonsVeryLongPress){
			//VeryLongPress: Program the Preset Temperature
			PresetTemperature[0] = SetTemperature[1];
			//Serial.print("Programmed1!!!");
			SaveParameters();
			PrintPresetTemperatures();
        } else if (InterruptCounterS1>ButtonsLongPress){
			//LongPress: Load Preset1 on Channel 1
			SetTemperature[1] = PresetTemperature[0];
			//Serial.print("Loaded1");
			RefreshScreen = 1;
        } else {
			//ShortPress
			//Select channel 1
			SelectedChannel = 1;
			RefreshScreen = 1;
        }
    } else {
		SelectedChannel = 1;
		RefreshScreen = 1;
    }
    InterruptCounterS1 = 0;
   }
  
  //Button 2 (RIGHT)
  if((Button2State)&&(InterruptCounterS2>0)){
    if(SelectedChannel==2){
      //VeryLongPress /LongPress /ShortPress
      if (InterruptCounterS2>ButtonsVeryLongPress){
			//VeryLongPress: Program the Preset Temperature
			PresetTemperature[1] = SetTemperature[1];
			//Serial.print("Programmed2!!!");
			SaveParameters();
			PrintPresetTemperatures();
        } else if (InterruptCounterS2>ButtonsLongPress){
			//LongPress: Load Preset2 on Channel 2
			SetTemperature[1] = PresetTemperature[1];
			RefreshScreen = 1;
			//Serial.print("Loaded2");
        } else {
			//ShortPress
			//Exit channel 2
			SelectedChannel = 0;
			RefreshScreen = 1;
        }
    } else if(SelectedChannel==1) { //No Channel 2 selected
      //VeryLongPress /LongPress /ShortPress
      if (InterruptCounterS2>ButtonsVeryLongPress){
			//VeryLongPress: Program the Preset Temperature
			PresetTemperature[1] = SetTemperature[0];
			//Serial.print("Programmed2!!!");
			SaveParameters();
			PrintPresetTemperatures();
        } else if (InterruptCounterS2>ButtonsLongPress){
			//LongPress: Load Preset2 on Channel 1
			SetTemperature[0] = PresetTemperature[1];
			RefreshScreen = 1;
			//Serial.print("Loaded2");
        } else {
			//ShortPress
			//Select channel 1
			SelectedChannel = 2;
			RefreshScreen = 1;
        }
    } else {
		SelectedChannel = 2;
		RefreshScreen = 1;
    }
    InterruptCounterS2 = 0;
   }
}

void InterruptMain(void){
	aState = digitalRead(EncoderA); // Reads the "current" state of the EncoderA
	// If the previous and the current state of the EncoderA are different, that means a Pulse has occured
	if (aState != aLastState){     
	// If the EncoderB state is different to the EncoderA state, that means the encoder is rotating clockwise
	if (digitalRead(EncoderB) != aState) { 
		//RotaryCounter --;
		RotaryCounter = RotaryCounter -RotaryEncoderTemperatureStep;
	} else {
		//RotaryCounter ++;
		RotaryCounter = RotaryCounter +RotaryEncoderTemperatureStep;
		}
	} 
	aLastState = aState; // Updates the previous state of the EncoderA with the current state  
	aButtonState = digitalRead(EncoderSW);
	if (!aButtonState){
		//ButtonPushed
		InterruptCounter++;
	}
	//Read buttons 
	Button1State = digitalRead(SwitchButton1);
	Button2State = digitalRead(SwitchButton2);
	if(!Button1State){
		InterruptCounterS1++;
	}
	if(!Button2State){
		InterruptCounterS2++;
	}
}
