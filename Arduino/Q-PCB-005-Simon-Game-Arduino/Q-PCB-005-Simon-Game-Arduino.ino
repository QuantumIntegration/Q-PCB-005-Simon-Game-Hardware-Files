// TL
// Quantum Integration 2021

#include <stdint.h>
#include "Adafruit_MCP23017.h"

uint8_t button[] = {8, 9, 10, 11}; //red is button[0], green is button[1], blue is button[2], yellow is button[3]
uint8_t led[] = {0, 1, 2, 3};      //red is led[0], green is led[1], blue is led[2], yellow is led[3]

const uint8_t maxRounds = 255;
uint8_t buttonSequence[maxRounds]; //make an array of numbers that will be the sequence that the player needs to remember

uint8_t resetPin = 12; //pin that the reset button is connected to

uint8_t pressedButton = 5; //a variable to remember which button is being pressed. 5 is the value if no button is being pressed.
uint8_t roundCounter = 1;  //keeps track of what round the player is on

uint16_t startTime = 0;    //timer variable for time limit on button press
uint16_t timeLimit = 2000; //time limit to hit a button

boolean gameStarted = false; //variable to tell the game whether or not to play the start sequence

Adafruit_MCP23017 mcp;

void setup()
{
  mcp.begin(7);

  //set all of the button pins to input
  mcp.pinMode(button[0], INPUT);
  mcp.pinMode(button[1], INPUT);
  mcp.pinMode(button[2], INPUT);
  mcp.pinMode(button[3], INPUT);

  //set all of the LED pins to output
  mcp.pinMode(led[0], OUTPUT);
  mcp.pinMode(led[1], OUTPUT);
  mcp.pinMode(led[2], OUTPUT);
  mcp.pinMode(led[3], OUTPUT);

  // set the reset pin to input
  mcp.pinMode(resetPin, INPUT);
}

void loop()
{
  if (gameStarted == false)
  {                     //if the game hasn't started yet
    startSequence();    //flash the start sequence
    roundCounter = 0;   //reset the round counter
    delay(1500);        //wait a second and a half
    gameStarted = true; //set gameStarted to true so that this sequence doesn't start again
  }

  //each round, start by flashing out the sequence to be repeated
  for (int i = 0; i <= roundCounter; i++)
  {                              //go through the array up to the current round number
    flashLED(buttonSequence[i]); //turn on the LED for that array position and play the sound
    delay(200);                  //wait
    allLEDoff();                 //turn all of the LEDs off
    delay(200);
  }

  //then start going through the sequence one at a time and see if the user presses the correct button
  for (int i = 0; i <= roundCounter; i++)
  { //for each button to be pressed in the sequence

    startTime = millis(); //record the start time

    while (gameStarted == true)
    { //loop until the player presses a button or the time limit is up (the time limit check is in an if statement)

      pressedButton = buttonCheck(); //every loop check to see which button is pressed
      if (pressedButton == 4)
      {
        gameStarted = false;
        break;
      }

      if (pressedButton != 5)
      { //if a button is pressed... (4 means that no button is pressed)

        flashLED(pressedButton); //flash the LED for the button that was pressed

        if (pressedButton == buttonSequence[i])
        {              //if the button matches the button in the sequence
          delay(250);  //leave the LED light on for a moment
          allLEDoff(); //then turn off all of the lights and
          break;       //end the while loop (this will go to the next number in the for loop)
        }
        else
        {                 //if the button doesn't match the button in the sequence
          loseSequence(); //play the lose sequence (the loose sequence stops the program)
          break;          //when the program gets back from the lose sequence, break the while loop so that the game can start over
        }
      }
      else
      {              //if no button is pressed
        allLEDoff(); //turn all the LEDs off
      }

      //check to see if the time limit is up
      if (millis() - startTime > timeLimit)
      {                 //if the time limit is up
        loseSequence(); //play the lose sequence
        break;          //when the program gets back from the lose sequence, break the while loop so that the game can start over
      }
    }
  }

  if (gameStarted == true)
  {
    roundCounter = roundCounter + 1; //increase the round number by 1

    if (roundCounter == maxRounds)
    {                //if the player has gotten to the 16th round
      winSequence(); //play the winning song
    }

    delay(500); //wait for half a second between rounds
  }
}

void flashLED(int ledNumber)
{
  mcp.digitalWrite(led[ledNumber], HIGH);
}

//TURN ALL LEDS OFF
void allLEDoff()
{
  //turn all the LEDs off
  mcp.digitalWrite(led[0], LOW);
  mcp.digitalWrite(led[1], LOW);
  mcp.digitalWrite(led[2], LOW);
  mcp.digitalWrite(led[3], LOW);
}

//CHECK WHICH BUTTON IS PRESSED
int buttonCheck()
{
  //check if any buttons are being pressed
  if (mcp.digitalRead(button[0]) == HIGH)
  {
    return 0;
  }
  else if (mcp.digitalRead(button[1]) == HIGH)
  {
    return 1;
  }
  else if (mcp.digitalRead(button[2]) == HIGH)
  {
    return 2;
  }
  else if (mcp.digitalRead(button[3]) == HIGH)
  {
    return 3;
  }
  else if (mcp.digitalRead(resetPin) == HIGH)
  {
    return 4;
  }
  else
  {
    return 5; //this will be the value for no button being pressed
  }
}

//START SEQUENCE
void startSequence()
{

  randomSeed(analogRead(A0)); //make sure the random numbers are really random

  //populate the buttonSequence array with random numbers from 0 to 3
  for (uint8_t i = 0; i < 0xff; i++)
  {

    buttonSequence[i] = round(random(0, 4));
  }

  //flash all of the LEDs when the game starts
  for (uint8_t i = 0; i <= 3; i++)
  {

    //turn all of the leds on
    mcp.digitalWrite(led[0], HIGH);
    mcp.digitalWrite(led[1], HIGH);
    mcp.digitalWrite(led[2], HIGH);
    mcp.digitalWrite(led[3], HIGH);

    delay(100); //wait for a moment

    //turn all of the leds off
    mcp.digitalWrite(led[0], LOW);
    mcp.digitalWrite(led[1], LOW);
    mcp.digitalWrite(led[2], LOW);
    mcp.digitalWrite(led[3], LOW);

    delay(100); //wait for a moment

  } //this will repeat 3 times
}

//WIN SEQUENCE
void winSequence()
{

  //turn all the LEDs on
  for (int j = 0; j <= 3; j++)
  {
    digitalWrite(led[j], HIGH);
  }

  //wait until a button is pressed
  do
  {
    pressedButton = buttonCheck();
  } while (pressedButton > 3);
  delay(100);

  gameStarted = false; //reset the game so that the start sequence will play again.
}

//LOSE SEQUENCE
void loseSequence()
{

  for (uint8_t i = 0; i < 2; i++)
  {

    //turn all of the leds on
    mcp.digitalWrite(led[0], HIGH);
    mcp.digitalWrite(led[1], HIGH);
    mcp.digitalWrite(led[2], HIGH);
    mcp.digitalWrite(led[3], HIGH);

    delay(100); //wait for a moment

    //turn all of the leds off
    mcp.digitalWrite(led[0], LOW);
    mcp.digitalWrite(led[1], LOW);
    mcp.digitalWrite(led[2], LOW);
    mcp.digitalWrite(led[3], LOW);

    delay(100); //wait for a moment

  } //this will repeat 2 times

  //turn all the LEDs on
  for (int j = 0; j <= 3; j++)
  {
    mcp.digitalWrite(led[j], HIGH);
  }

  //wait until a button is pressed
  do
  {
    pressedButton = buttonCheck();

  } while (pressedButton != 4);
  delay(200);

  gameStarted = false; //reset the game so that the start sequence will play again.
}