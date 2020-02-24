//Software Version
#define SwVersion "Sw.1.00d"
#define Author "by Riccardo Pittini"
#define UserName false
#define User "XX YYY"

//HARDWARE CONFIGURATION
//LCD TFT pins
#define DisplayCS   4
#define DisplayDC   5
#define DisplayRST  8//(not connected ;) 
//Encoder pins definitions 
#define EncoderA 2
#define EncoderB 3
#define EncoderSW 8
//Heaters pin Definitions
#define Heater1Pin 9
#define Heater2Pin 10
//Temperature Sensor pins Definitions
#define TemperatureSensor1Pin A1
#define TemperatureSensor2Pin A0
//Buttons pins definitions
#define SwitchButton1 1
#define SwitchButton2 0
//Others
#define InputVoltagePin A2
#define InternalTemperaturePin A3
//Hardware limits
#define PresetMaxDutyCycle 125 		//Set the default max duty cycle to limit the power from the power supply
#define DelayMeasurementms 50	//Delay to avoid noise measurement
//Internal NTC configuration
#define NTCResistance 100000
#define NTCRefTemperature 25
//#define NTCBeta 4485
#define NTCBeta 3950
#define NTCSerieResistor 100000

//INPUT VOLTAGE sensing
//OLD HARDWARE v.0.99 (not released only HW testing)
//#define InputVoltageScaling 0.0455728 // 10k and 1.2k 0.0455728, Vref*(Rlow+Rhigh)/Rlow/1024
//NEW HARDWARE v.1.00
#define InputVoltageScaling 0.0537109; // 10k and 1k 0.0537108, Vref*(Rlow+Rhigh)/Rlow/1024


//Vin Under Volatge Lock Out (VinUVLO) for battery operation
#define PresetVinUVLO 80 	//80 = 8V, NOT implemented yet

//SOFTWARE CONFIGURATION
//Buttons delays (short VS long press)
#define ButtonsPressLatch 50 
#define ButtonsLongPress 300 
#define ButtonsVeryLongPress 1000

// Dispaly Pixels OFFSET
#define XOffset 0
#define YOffset 0

//PID parameters
#define OUTPUT_MIN 0
#define OUTPUT_MAX 250
#define KPconst 15 //Original 0.9
#define KIconst 1 //Original 0.03
#define KDconst 0.03 //Original 0.4
#define BangBangHysteresis 10

//ADC to temperature definitions
#define ADCtoTemperatureGain 	0.53 //Original Weller Station
#define ADCtoTemperatureOffset  25.0

//Channel Temperatures Definitions
#define StandbyTemparature	175
#define MinimumTemperature  25
#define MaximumTemperature	455
#define SetTemperatureCh1	275	
#define SetTemperatureCh2	350
#define PresetTemperature1	275
#define PresetTemperature2	350
#define RotaryEncoderTemperatureStep	1
#define TemperatureDisconnectSensing 500
#define LowTemperatureLimit 50

//Save Parameters Address
#define EEPROM_address 0

//General color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

 
