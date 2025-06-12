// NOTE: 'reg51.h' is required for 8051 microcontroller development. If you see linter errors about this file, they can be ignored if you are not compiling for 8051 hardware.
#include <reg51.h>
	//ACCEPTING DATA FROM ADC808 THROUGH PORT P1
#define OUTPUT_PORT P1
//CHANNEL SELECTING PINS OF ADC808
#define ADD_A P2_0
#define ADD_B P2_1
#define ADD_C P2_2
//CHANNEL ENABLING PIN OF ADC808
#define AL P2_3
//READING CONVERTING DATA FROM ANALOG TO DIGITAL OF ADC808
#define RD P2_4
//START CONVERTING DATA OF ADC808
#define WR P2_5
//WAITING FOR CONVERTED DATA TO FINISH PIN
#define INTR_PIN P2_6
//PIN FOR STEPPER MOTOR
#define WINDING1 P3_4
#define WINDING2 P3_5
#define WINDING3 P3_6
#define WINDING4 P3_7
//PIN FOR MOTOR
#define MOTOR P3_2
// UART Communication Constants
#define VOLTAGE_CMD 'V'
#define PRESSURE_CMD 'P'
#define TEMPERATURE_CMD 'T'

// Sensor Conversion Constants
#define ADC_MAX_VALUE 256.0
#define ADC_REFERENCE_VOLTAGE 5.0

// Pressure Sensor Constants
#define PRESSURE_CONV_FACTOR_A (5.0 / ADC_MAX_VALUE)
#define PRESSURE_CONV_FACTOR_B 0.09
#define PRESSURE_CONV_FACTOR_C 0.009

// Voltage Sensor Constant
#define VOLTAGE_CONV_FACTOR 5.02

// Temperature Sensor Constants
#define TEMPERATURE_CONV_FACTOR_A 1.966
#define TEMPERATURE_CONV_FACTOR_B 2.0

// ASCII Offset for numerical conversion
#define ASCII_OFFSET 0x30

// Stepper Motor Control Threshold
#define MOTOR_THRESHOLD 3

//PROTOTYPING OF FUNCTIONS
void condition1(unsigned char);
void condition2(unsigned char);
void condition3(unsigned char);

//PROGRAM FUNCTIONS
void TimerDelay();

// Generic function to read ADC from a specified channel
unsigned char read_adc(unsigned char channel)
{
	unsigned char x;
	// Select the ADC channel based on the input 'channel'
	ADD_A = (channel & 0x01) ? 1 : 0;
	ADD_B = (channel & 0x02) ? 1 : 0;
	ADD_C = (channel & 0x04) ? 1 : 0;
	AL = 1; // Enable the selected channel
	WR = 1; // Start conversion
	AL = 0;
	WR = 0; // Stop conversion
	while (INTR_PIN == 1); // Wait for conversion to stop
	while (INTR_PIN == 0);
	RD = 1; // Read converted data
	x = OUTPUT_PORT; // Copy converted data to variable x
	RD = 0; // Stop reading converted data
	return x; // Return the converted value
}

void rec() interrupt 4
{
	float a, b, c;
	if (RI == 1)
	{
		if (SBUF == VOLTAGE_CMD) // Read Voltage (Channel 0)
		{
			condition1(read_adc(0));
		}
		if (SBUF == PRESSURE_CMD) // Read Pressure (Channel 1)
		{
			c = read_adc(1);
			a = PRESSURE_CONV_FACTOR_A * c;
			b = ((a / ADC_REFERENCE_VOLTAGE) + PRESSURE_CONV_FACTOR_B) / PRESSURE_CONV_FACTOR_C - 1;
			condition2(b);
		}
		if (SBUF == TEMPERATURE_CMD) // Read Temperature (Channel 2)
		{
			condition3(read_adc(2));
		}
		RI = 0;
		TR0 = 1;
	}
}

void TimerDelay()	//Generating 65535 * 1.085us=71.1ms delay
{
	TL0 = 00;	//INITIAL VALUES ARE STARTING FROM ZERO
	TH0 = 00;
	TR0 = 1;	//STARTING TIMER
	while (TF0 == 0);	//WAIT FOR OVER FLOWING TIMER
	TR0 = 0;	//STOPING TIMER
	TF0 = 0;	//FORCING OVER FLOWING
}

// Helper function to send a byte with checksum over UART
void send_with_checksum(unsigned char data) {
    unsigned char checksum = data ^ 0xAA;
    SBUF = data;
    while (TI == 0);
    TI = 0;
    SBUF = checksum;
    while (TI == 0);
    TI = 0;
}

// Update condition1, condition2, condition3 to use send_with_checksum
void condition1(unsigned char var) {
    var = VOLTAGE_CONV_FACTOR * var / ADC_MAX_VALUE;
    send_with_checksum(var + ASCII_OFFSET);
}

void condition2(unsigned char var) {
    unsigned char unite, tenth;
    tenth = var / 10;
    tenth = tenth + ASCII_OFFSET;
    send_with_checksum(tenth);
    TimerDelay(); TimerDelay(); TimerDelay();
    unite = var % 10;
    unite = unite + ASCII_OFFSET;
    send_with_checksum(unite);
}

void condition3(unsigned char var) {
    unsigned char unite, tenth;
    var = var * TEMPERATURE_CONV_FACTOR_A;
    tenth = var / 10;
    tenth = tenth + ASCII_OFFSET;
    send_with_checksum(tenth);
    TimerDelay(); TimerDelay(); TimerDelay();
    unite = var % 10;
    unite = unite + ASCII_OFFSET;
    send_with_checksum(unite);
}

void condition3a(unsigned char var)	//CONDITIONS FOR TEMPRATURE SENSOR
{
	unsigned char tenth;
	var = var * TEMPERATURE_CONV_FACTOR_B;	//FORMULA FOR TEMPRATURE SENSOR
	tenth = var / 10;
	if (tenth > MOTOR_THRESHOLD)
	{
		MOTOR = 1;
	}
	else
	{
		MOTOR = 0;
	}
}

void condition2a(unsigned char var)	//CONDITIONS FOR TEMPRATURE SENSOR
{
	float g, h, tenth;
	g = (5.0 / 256) * var;	//PRESSURE FORMULA
	h = ((g / 5.0) + 0.09) / 0.009 - 1;	//FORMULA FOR TEMPRATURE SENSOR
	tenth = h / 10;
	if (tenth >= 4 && tenth < 5)	//IF PRESSURE IS below 50 PASCAL ROTATE STEPPER MOTOR 0 DEGREE CONDITION
	{
		WINDING1 = 1;
		WINDING2 = 0;
		WINDING3 = 0;
		WINDING4 = 1;
	}

	if (tenth >= 5 && tenth < 6)	//IF PRESSURE IS ABOVE 50 PASCAL ROTATE STEPPER MOTOR 45 DEGREE CONDITION
	{
		WINDING1 = 0;
		WINDING2 = 0;
		WINDING3 = 0;
		WINDING4 = 1;
	}

	if (tenth >= 6 && tenth < 7)	//IF PRESSURE IS ABOVE 60 PASCAL ROTATE STEPPER MOTOR 45 DEGREE CONDITION
	{
		WINDING1 = 0;
		WINDING2 = 0;
		WINDING3 = 1;
		WINDING4 = 1;
	}

	if (tenth >= 7 && tenth < 8)	//IF PRESSURE IS ABOVE 70 PASCAL ROTATE STEPPER MOTOR 135 DEGREE CONDITION
	{
		WINDING1 = 0;
		WINDING2 = 0;
		WINDING3 = 1;
		WINDING4 = 0;
	}

	if (tenth >= 8 && tenth < 9)	//IF PRESSURE IS ABOVE 80 PASCAL ROTATE STEPPER MOTOR 180 DEGREE CONDITION
	{
		WINDING1 = 0;
		WINDING2 = 1;
		WINDING3 = 1;
		WINDING4 = 0;
	}
}

void main()	//MAIN FUNCTION
{
	//DECLARING VARIABLE
	TimerDelay();	//TIMER DELAY
	INTR_PIN = 1;	//INITIALIING ALL VALUES
	RD = 0;
	WR = 0;
	AL = 0;
	TMOD = 0x21;	//TIMER1 IN MODE 2(8 BIT AUTORELOAD) AND TIMER 1 IN MODE 1(16BIT)
	TH1 = 0xFD;	//BAUD RATE OF 9600  
	SCON = 0x50;	//SCON REGISTER WITH MODE 2(8 DATA BITS AND 2 STOP AND START BIT)
	IE = 0x90;
	TR1 = 1;	//STARTING TIMER 1

	condition2a(read_adc(1));
	condition3a(read_adc(2));
}
