/*
 Name:		LCD.ino
 Created:	10/17/2020 5:57:44 PM
 Author:	Mikakka
*/

// the setup function runs once when you press reset or power the board

#include <EEPROM.h>
#include <TMRpcm.h>
#include <pcmRF.h>
#include <pcmConfig.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <math.h>

enum state
{
	HALT = 0,
	REACTION = 1,
	ENDREACTION = 2
};

typedef struct QandA
{
	char question[40];
	uint8_t point = 0;
}maindata;

#define Button1Pin 2				//Reaction button
#define Button2Pin 3				//Enter button
#define maxquestion 3			//How much question
#define maxreaction 5				//How much point

#define SD_ChipSelectPin 10			//SD pin config
#define Speaker_output 9			//Speaker pin config
#define Speaker_output_volumn 5		//Speaker volumn config

#define locateXReactionStar 1		//Where is star appear
#define locateYReactionStar 0		//Where is star from

#define xlongLCD 20					//Length of LCD
#define ylongLCD 4					//Width of LCD
#define LCDaddress 0x27				//Address I2C of LCD

bool soundenable = false;			//Set true to enable playsound
bool SDfound = false;

LiquidCrystal_I2C lcd(LCDaddress, xlongLCD, ylongLCD);

maindata record[maxquestion];

TMRpcm soundoutput;

File Recorddata;

uint8_t last_reaction = 0, reaction = 0;
uint8_t questioncount = 0, last_questioncout = -1;

uint8_t Bu1 = 0, Bu2 = 0;

char startscreen[] = "Hello";
char endscreen[] = "Thanks for do survey !!!";

char star = '*';
char reactionout[6];

state statemode = HALT, last_statemode = ENDREACTION;

/*
* @brief	Input question to system
* @note		Copy this line to add more question
*				+ strcpy(record[i].question, "a");
*				+ Change "i" for choose no. question
*				+ Change "a" for change question
*				+ Number of question must smaller than max question number (maxquestion)
*/

void inputquestion()
{
	//strcpy(record[0].question, "1.");		//Copy this
	strcpy(record[0].question, "1.hello how are you mother facker?");
}

void setup() 
{
	inputquestion();

	lcd.init();
	lcd.backlight();

	soundoutput.speakerPin = Speaker_output;
	soundoutput.setVolume(Speaker_output_volumn);

	pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(Button1Pin), EXTI1, CHANGE);
	attachInterrupt(digitalPinToInterrupt(Button2Pin), EXTI2, CHANGE);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	/*
	* @brief	Check SD is plug in
	*/
	if (!SD.begin(SD_ChipSelectPin))
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("SD not found !");
		SDfound = false;
		goto out;
	}
	else
		SDfound = true;
	/*
	* @brief	Change state machine
	* @note		Mode:
	*			- HALT: Not do anything
	*			- REACTION: Show question
	*			- ENDREACTION: End of survey
	*/
	if ((last_statemode != statemode || last_questioncout != questioncount || last_reaction != reaction)&&SDfound)
	{
		switch (statemode)
		{
		case HALT:
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print(startscreen);
			break;
		case REACTION:
			if (last_reaction != reaction || questioncount == 0)
			{
				for (int i = 1; i <= reaction; i++)
					strncat(reactionout, &star, 1);
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print(record[questioncount].question);
				lcd.setCursor(locateYReactionStar, locateXReactionStar);
				lcd.print(reactionout);
				record[questioncount].point = reaction;
				memset(reactionout, 0, 5);
			}
			break;
		case ENDREACTION:
			Recorddata = SD.open("record.txt", FILE_WRITE);
			uint16_t output_ui16 = 0;
			if (soundenable)
				soundoutput.play("4.wav");
			lcd.clear();
			lcd.print(endscreen);
			for (uint8_t i = 0; i < maxquestion; i++)
				output_ui16 += (uint16_t)record[i].point * (uint16_t)pow(10, maxquestion - i - 1);
			Recorddata.println(output_ui16);
			questioncount = 0;
			reaction = 0;
			Recorddata.close();
			break;
		default:
			break;
		}
		last_statemode = statemode;
		last_questioncout = questioncount;
		last_reaction = reaction;
	}
out:
	delay(5);
}

void EXTI1()
{
	delay(200);
	if (digitalRead(Button1Pin) == HIGH && Bu1 == 0)
	{
		Bu1 = 1;
		goto out1;
	}
	else if (digitalRead(Button1Pin) == LOW && Bu1 == 1)
		Bu1 = 0;
	else
		goto out1;
	if (statemode == REACTION)
	{
		if (++reaction > maxreaction)
			reaction = 0;
	}
out1:
	delay(5);
}

void EXTI2()
{
	delay(200);
	if (digitalRead(Button2Pin) == HIGH && Bu2 == 0)
	{
		Bu2 = 1;
		goto out2;
	}
	else if (digitalRead(Button2Pin) == LOW && Bu2 == 1)
		Bu2 = 0;
	else
		goto out2;
	switch (statemode)
	{
	case HALT:
		statemode = REACTION;
		break;
	case REACTION:
		++questioncount;
		if (questioncount < maxquestion)
		{
			reaction = 0;
			last_reaction = 6;
			break;
		}
		else
		{
			last_reaction = 0;
			statemode = ENDREACTION;
		}
		break;
	case ENDREACTION:
		questioncount = 0;
		statemode = HALT;
		break;
	default:
		break;
	}
out2:
	delay(5);
}
