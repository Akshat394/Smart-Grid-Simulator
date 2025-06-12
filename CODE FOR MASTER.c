#ifdef __C51__
#include <reg51.h>
#else
// Minimal stubs for non-embedded environments
unsigned char P1, P2, P3;
unsigned char SBUF, TI, RI, TR0, TF0, TMOD, TH1, SCON, TR1, TL0, TH0;
unsigned char rs, e, col1, col2, col3, row1, row2, row3, row4, LAMP;
#define lcd P1
#endif
//PIN FOR LCD            
//PIN FOR REGISTER
//PIN FOR ENABLE
//PIN FOR KEYPAD
//PIN FOR STREET LIGHTS
//PROGRAM FUNCTIONS PROTOTYPING
void delay(int);
void cmd(char);
void display(char);
void string(char*);
void init(void);
void TimerDelay();	//TIMER DELAY
void TimerDelay()	//Generating 65535 * 1.085us=71.1ms delay
{
	TL0 = 00;
	TH0 = 00;
	TR0 = 1;
	while (TF0 == 0);
	TR0 = 0;
	TF0 = 0;
}

void delay(int d)	//DELAY
{
	unsigned char i;
	for (; d > 0; d--)
	{
		for (i = 250; i > 0; i--);
		for (i = 248; i > 0; i--);
	}
}

void cmd(char c)	//COMMAND FUNCTION FOR LCD
{
	lcd = c;
	rs = 0;
	e = 1;
	delay(5);
	e = 0;
}

void display(char c)	//DISPLAY FUNCTION FOR LCD
{
	lcd = c;
	rs = 1;
	e = 1;
	delay(1);
	e = 0;
}

void string(char *p)	//STRING FUNCTION FOR LCD
{
	while (*p)
	{
		display(*p++);
	}
}

void init(void)	//LCD INITIALIZATION
{
	unsigned char x;
	cmd(0x0c);	//display on
	cmd(0x38);	//use two lines
	cmd(0x01);	// Clearing of screen
	cmd(0x80);	//BEGINNING OF THE FIRST LINE
	string("Starting...");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}

	cmd(0x01);
	string("REQUESTING");
	cmd(0xc0);
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}

	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1S delay
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1s delay
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1s delay
	cmd(0x01);	// beginning of first line
}

// Helper function to receive a byte with checksum over UART
unsigned char receive_with_checksum() {
    unsigned char data, checksum;
    while (RI == 0); // Wait for data
    data = SBUF;
    RI = 0;
    while (RI == 0); // Wait for checksum
    checksum = SBUF;
    RI = 0;
    if ((data ^ 0xAA) == checksum) {
        return data;
    } else {
        // Checksum error, return 0xFF as error code
        return 0xFF;
    }
}

// Update sensor1, sensor2, sensor3 to use receive_with_checksum
void sensor1() {
    unsigned char a;
    string("VOLTAGE     ");
    cmd(0xc0);
    a = receive_with_checksum();
    if (a != 0xFF) {
        display(a);
        string(" V  ");
    } else {
        string("ERR");
    }
    cmd(0x80);
}

void sensor2() {
    unsigned char a, b;
    string("PRESSURE     ");
    cmd(0xc0);
    a = receive_with_checksum();
    b = receive_with_checksum();
    if (a != 0xFF && b != 0xFF) {
        display(a);
        display(b);
        string("KP");
    } else {
        string("ERR");
    }
    cmd(0x80);
}

void sensor3() {
    unsigned char a, b;
    string("TEMPERATURE   ");
    cmd(0xc0);
    a = receive_with_checksum();
    b = receive_with_checksum();
    if (a != 0xFF && b != 0xFF) {
        display(a);
        display(b);
        string("C  ");
    } else {
        string("ERR");
    }
    cmd(0x80);
}

unsigned char keypad()	//KEYPAD FUNCTION
{
	unsigned char a;
	col1 = 0, col2 = 1, col3 = 1;	//ENABLING COLOUME 1
	if (row1 == 0)	//IF KEY 1 IS PRESSED SEND 'V' TO SLAVE
	{
		SBUF = 'V';
		while (TI == 0);
		TI = 0;
		a = 1;
	}

	if (row2 == 0)	//IF KEY4 IS PRESSED SEND 'P' TO SLAVE
	{
		SBUF = 'P';
		while (TI == 0);
		TI = 0;
		a = 2;
	}

	if (row3 == 0)	//IF KEY7 IS PRESSED SEND 'T' TO SLAVE
	{
		SBUF = 'T';
		while (TI == 0);
		TI = 0;
		a = 3;
	}

	if (row4 == 0)	//IF KEY 2 IS PRESSED ON STREET LIGHTS
	{
		SBUF = '*';
		while (TI == 0);
		TI = 0;
		a = 15;
	}

	col1 = 1, col2 = 0, col3 = 1;	//ENABLING COLOUME 2
	if (row1 == 0)	//IF KEY 2 IS PRESSED OFF STREET LIGHTS
	{
		a = 16;
	}

	return a;	//RETURN a
}

void main()	//MAIN FUNCTION
{
	LAMP = 0;	//INITIALING LAMP
	TMOD = 0x21;	//TIMER1 IN MODE 2(8 BIT AUTORELOAD) AND TIMER 1 IN MODE 1(16BIT)
	TH1 = 0xFD;	//BAUD RATE OF 9600
	SCON = 0x50;	//SCON REGISTER WITH MODE 2(8 DATA BITS AND 2 STOP AND START BIT)
	TR1 = 1;	//STARTING TIMER 1

	init();	//INITIALIZING LCD

	while (1)	//FOREVER EHILE LOOP
	{
		if (keypad() == 1)	//IF 1 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF VOLTAGE SENSOR
		{
			sensor1();
		}

		if (keypad() == 2)	//IF 4 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF PRESSURE SENSOR
		{
			sensor2();
		}

		if (keypad() == 3)	//IF 7 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF TEMPRATURE SENSOR
		{
			sensor3();
		}

		if (keypad() == 15)	//IF *IS PRESSED ON STREET LIGHTS
		{
			LAMP = 1;
		}

		if (keypad() == 16)	//IF 2 IS PRESSED OFF STREET LIGHTS
		{
			LAMP = 0;
		}
	}
}
