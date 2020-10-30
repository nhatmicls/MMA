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

enum state
{
	HALT = 0,
	REACTION = 1,
	ENDREACTION = 2
};

typedef struct QandA
{
	char question[20];
	uint8_t point;
}maindata;

#define Button1Pin 2				//Reaction button
#define Button2Pin 3				//Enter button
#define maxquestion 3				//How much question
#define maxreaction 6				//How much point

#define SD_ChipSelectPin 10			//SD pin config
#define Speaker_output 9			//Speaker pin config
#define Speaker_output_volumn 5		//Speaker volumn config

#define locateXReactionStar 1		//Where is star appear
#define locateYReactionStar 0		//Where is star from

#define xlongLCD 16					//Length of LCD
#define ylongLCD 2					//Width of LCD
#define LCDaddress 0x3F				//Address I2C of LCD

bool soundenable = false;			//Set true to enable playsound
bool SDfound = false;

LiquidCrystal_I2C lcd(LCDaddress, xlongLCD, ylongLCD);

maindata record[maxquestion];

TMRpcm soundoutput;

File Recorddata;

uint8_t current_reaction = 0, reaction = 0;
uint8_t questioncount = 0, lastquestioncout = -1;

char startscreen[] = "Hello";
char endscreen[] = "Thanks for do survey";

char star = '*';
char reactionout[6];

state statemode = HALT, laststatemode = ENDREACTION;

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
	strcpy(record[0].question, "a");		//Copy this
}

void setup() 
{
	inputquestion();

	Recorddata = SD.open("record.txt", FILE_WRITE);

	lcd.init();
	lcd.backlight();

	soundoutput.speakerPin = Speaker_output;
	soundoutput.setVolume(Speaker_output_volumn);

	pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(Button1Pin), EXTI1, FALLING);
	attachInterrupt(digitalPinToInterrupt(Button2Pin), EXTI2, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	/*
	* @brief	Check SD is plug in
	*/
	if (!SD.begin(SD_ChipSelectPin))
	{
		lcd.setCursor(0, 0);
		lcd.print("SD not found !");
		SDfound = true;
	}
	/*
	* @brief	Change state machine
	* @note		Mode:
	*			- HALT: Not do anything
	*			- REACTION: Show question
	*			- ENDREACTION: 
	*/
	if ((laststatemode != statemode || lastquestioncout != questioncount || current_reaction != reaction)&&SDfound)
	{
		switch (statemode)
		{
		case HALT:
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print(startscreen);
			break;
		case REACTION:
			if (current_reaction != reaction || questioncount == 1)
			{
				for (int i = 1; i <= reaction; i++)
					strncat(reactionout, &star, 1);
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print(record[questioncount].question);
				lcd.setCursor(locateYReactionStar, locateXReactionStar);
				lcd.print(reactionout);
				record[questioncount].point = reaction;
				current_reaction = reaction;
				memset(reactionout, 0, 5);
			}
			break;
		case ENDREACTION:
			if (soundenable)
				soundoutput.play("4.wav");
			lcd.clear();
			lcd.print(endscreen);
			break;
		default:
			break;
		}
		laststatemode = statemode;
		lastquestioncout = questioncount;
	}
}

void EXTI1()
{
	if (statemode == REACTION)
	{
		reaction++;
		if (reaction >= maxreaction)
			reaction = 0;
	}
}

void EXTI2()
{
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
			current_reaction = 7;
			break;
		}
		statemode = ENDREACTION;
		break;
	case ENDREACTION:
		questioncount = 1;
		statemode = HALT;
		break;
	default:
		break;
	}
}
