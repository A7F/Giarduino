#include <DHT.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//constants
#define DHTPIN 7		        // DHT22 DAT pin
#define DHTTYPE DHT22       // set sensor type as DHT 22 (AM2302)
#define I2C_ADDR 0x27	      // Add your I2C lcd monitor address here.  Find it from I2C Scanner script
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define LDR_pin A0			    //LDR pin. Must be analog
#define button_1 2			    //button 1 pin. Must be digital
#define MAX_PROG 4			    //maximum programs available
#define led_pin 8			      //led status pin
#define check_delay 1000	  //delay time between every sensors scan

LiquidCrystal_I2C	lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
DHT dht(DHTPIN, DHTTYPE);     // Init DHT sensor for normal 16mhz Arduino

float hum;					    //Stores humidity value
float temp;					    //Stores temperature value
int ldr;					      //Stores LDR value. 0-1023
int buttonState = 0;		//button state
int program = 1;			  //current program value
unsigned long lastTime = 0;
int longPressTime = 300;
unsigned long buttonTimer = 0;
boolean buttonActive = false;
boolean longPressActive = false;
boolean ledState = LOW;

void setup()
{
	Serial.begin(9600);
	dht.begin();
	lcd.begin(16, 2);
	lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
	lcd.setBacklight(HIGH);
	pinMode(button_1, INPUT);
	pinMode(led_pin, OUTPUT);
}

void loop()
{
	//Read data from sensors every second and store it to variables hum, temp and ldr
	checkButton1();
	
	if (millis() - lastTime > check_delay) {
		checkSensors();
		lastTime = millis();
	}

	print();
}

void checkButton1() {
	if (digitalRead(button_1) == HIGH) {	//button pressed
		if (buttonActive == false) {
			buttonActive = true;
			buttonTimer = millis();
		}

		if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
			longPressActive = true;
			ledState = !ledState;
			digitalWrite(led_pin, ledState);
		}
	}
	else {	//button not pressed
		if (buttonActive == true) {	
			if (longPressActive == true) {
				longPressActive = false;
			}
			else {
				if (program < MAX_PROG) {
					program++;
				}
				else if (program == MAX_PROG) {
					program = 1;
				}
			}
			buttonActive = false;
		}
	}
}

void checkSensors() {
	hum = dht.readHumidity();
	temp = dht.readTemperature();
	ldr = analogRead(LDR_pin);
}

void print() {
	lcd.print("H:");
	lcd.setCursor(2, 0);		//print humidity
	lcd.print(hum, 0);
	lcd.print((char)37);
	lcd.setCursor(0, 2);
	lcd.print("T:");
	lcd.setCursor(2, 2);	//print temp
	lcd.print(temp, 1);
	lcd.setCursor(10, 0);
	lcd.print("L:");
	lcd.setCursor(12, 0);	//print light
	lcd.print(ldr);
	lcd.setCursor(12, 2);
	lcd.print("P");
	lcd.setCursor(13, 2);	//print current program
	if (program == MAX_PROG) {
		lcd.print("OFF");
	}
	else {
		lcd.print(program);
		lcd.print("  ");
	}
}
