/*
 Name:		LCD.ino
 Created:	10/17/2020 5:57:44 PM
 Author:	Mikakka
*/

// the setup function runs once when you press reset or power the board

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include<string.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define Button1Pin 2	//Reaction button
#define Button2Pin 3	//Enter button
#define maxquestion 3	//How much question
#define maxreaction 6

#define locateXReactionStar 1		//Where is star appear
#define locateYReactionStar 0		//Where is star from

enum state
{
	HALT = 0,
	REACTION = 1,
	ENDREACTION = 2
};

uint8_t current_reaction = 0, reaction = 0;
uint8_t question = 1, lastquestion = 0;

uint8_t point[maxquestion];

char reactionout[6];
char star = '*';

state statemode = HALT, laststatemode = ENDREACTION;

void setup() 
{
	lcd.init();
	lcd.backlight();

	pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(Button1Pin), EXTI1, FALLING);
	attachInterrupt(digitalPinToInterrupt(Button2Pin), EXTI2, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	if (laststatemode != statemode || lastquestion != question || current_reaction != reaction)
	{
		switch (statemode)
		{
		case HALT:
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print("Arduinokit.vn");
			lcd.setCursor(0, 1);
			lcd.print("Mother facker");
			break;
		case REACTION:
			if (current_reaction != reaction || question == 1)
			{
				for (int i = 1; i <= reaction; i++)
					strncat(reactionout, &star, 1);
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print(question);
				lcd.setCursor(locateYReactionStar, locateXReactionStar);
				lcd.print(reactionout);
				point[question] = reaction;
				current_reaction = reaction;
				memset(reactionout, 0, 5);
			}
			break;
		case ENDREACTION:
			lcd.clear();
			lcd.print("Chot");
			break;
		default:
			break;
		}
		laststatemode = statemode;
		lastquestion = question;
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
		++question;
		if (question <= maxquestion)
		{
			reaction = 0;
			current_reaction = 7;
			break;
		}
		statemode = ENDREACTION;
		break;
	case ENDREACTION:
		question = 1;
		statemode = HALT;
		break;
	default:
		break;
	}
}
