#include <Button.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Timer and Scoreboard Declaration
Adafruit_7segment timer = Adafruit_7segment();
Adafruit_AlphaNum4 scoreboardDisplay = Adafruit_AlphaNum4();

#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

// initialize the library with the numbers of the interface pins
Adafruit_LiquidCrystal lcd(0);

// Game Related Parameters
int roundNo = 1;
long gameTime = 60000;                // Time for the game in ms
int turn = 0;                         // To determine whose turn is it. 0 = player 1, 1 = player 2
int firstTurn = 0;                    // To determine which player gets first turn in a game. 0 = player 1, 1 = player 2
int playerOneScore = 0;               // Score of player 1
int playerTwoScore = 0;               // Score of player 2
const int playerOneTurnLED = 52;      // LED to determine its player 1's turn
const int playerTwoTurnLED = 53;      // LED to determine its player 2's turn

char mainBoard[9] = {'-', '-', '-', '-', '-', '-', '-', '-', '-'};    // Current sitution of the game board. Possible values are '-', 'X' and 'O'


//LEDs and Button Related Parameters
int startBtn = 13;
int resetBtn = 12;

// The list of digital pins for LEDs
int LEDsArray[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};

// Buzzer Related Parameters
const int buzzerPin = 40;                                             // Digital pin for buzzer
const int songLength = 18;
char notesStart[] = "cdfda ag cdfdg gf ";                             
char notesNextGame[] = "cdfda";                                       
char notesError[] = "c";                       
int beatsStart[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
int beatsNextGame[] = {1,1,1,1,1};
int beatsError[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
int tempo = 113;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  for(int i = 0; i<18; i++){
    pinMode(22+i, OUTPUT);
  }
  manageInitialSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(startBtn) == 0){
    Serial.println("Game Beginning!");
    preGameSetup();
    startGame();
  }
}

void preGameSetup(){
  roundNo = 1;
  playerOneScore = 0;
  playerTwoScore = 0;
  displayPlayerScore(playerOneScore, playerTwoScore);
  resetMainBoard();
  lcd.clear();
  lcdPrint("Game Beginning!", "", false, false);
  sendToTimer(1.00);    
  resetLEDs();
  playBuzzer(beatsStart, notesStart);
  printRoundDetails();
}

void manageInitialSetup(){
    
  //  Test All the LEDs
  for(int i=0; i< sizeof(LEDsArray)/sizeof(int);i++){
    digitalWrite(LEDsArray[i], HIGH);
  }
  delay(2000);
  for(int i=0; i< sizeof(LEDsArray)/sizeof(int);i++){
    digitalWrite(LEDsArray[i], LOW);
  }

  // Set the timer and the scoreboard
  lcd.begin(16, 2);
  #ifndef __AVR_ATtiny85__
  Serial.println("7 Segment Backpack Test");
  #endif
  timer.begin(0x71);
  scoreboardDisplay.begin(0x70);

    
  sendToTimer(1.00);
  lcdPrint("Press 'PLAY' to", "start", false, false);

  resetScoreBoard();
}

void startGame(){
  unsigned long startTime = millis();                                               // Start time of the game

  // Continue the loop until the timer runs out
  while(millis() - startTime < gameTime){
    int elapsedTime = (millis() - startTime)/1000;
    int timeRemaining = gameTime/1000 - elapsedTime;
    turnLED();                                                                      // Turn on the LED to display whose turn is it
    sendToTimer(timeRemaining/60 + float (float (timeRemaining%60)/100));           // Display time remaining in m:ss form

    // Check if the reset button is pressed
    if(digitalRead(resetBtn) == 1){
      resetGame();
      startTime = millis();
    }
    for(int i=0;i<9;i++){
      int action = digitalRead(2+i);
      if(action == 0){
        Serial.print("Clicked by - ");
        Serial.println(turn);
        playStep(i);
        delay(1000);  
      }
    }
  }

  String winMsg = "Game Draw!";
  if(playerOneScore > playerTwoScore) winMsg = "Player 1 won!";
  if(playerOneScore < playerTwoScore) winMsg = "Player 2 won!";
  lcdPrint(winMsg, "Press PLAY to start a new game", false, true);
  playBuzzer(beatsStart, notesStart);
}

// To process when a player plays a turn
void playStep(int index){
  if(mainBoard[index] == '-'){
    mainBoard[index] = turn == 0 ? 'X' : 'O';
    manageLED(index, turn);
      if(!isSomeoneWinner()) {
        turn = (turn + 1)%2;
      }
      printRoundDetails();
    } else {
    Serial.println("Wrong input");
    playErrorBuzzer(beatsError, notesError);
  }
}

//To turn on LEDs when a turn is played
void manageLED(int index, int type){
   switch (index){
    case 0:
      digitalWrite(type == 0 ? 34 : 35, HIGH);
      break;
    case 1:
      digitalWrite(type == 0 ? 36 : 37, HIGH);
      break;
    case 2:
      digitalWrite(type == 0 ? 38 : 39, HIGH);
      break;
    case 3:
      digitalWrite(type == 0 ? 28 : 29, HIGH);
      break;
    case 4:
      digitalWrite(type == 0 ? 30 : 31, HIGH);
      break;
    case 5:
      digitalWrite(type == 0 ? 32 : 33, HIGH);
      break;
    case 6:
      digitalWrite(type == 0 ? 22 :23, HIGH);
      break;
    case 7:
      digitalWrite(type == 0 ? 24 : 25, HIGH);
      break;
    case 8:
      digitalWrite(type == 0 ? 26 : 27, HIGH);
      break;      
   }
}


//LED blink pattern when someone wins a game
void blinkWinningLeds(int LEDs[]){
  bool initialState = true;
  for(int j=0; j<5;j++){
    for(int i=0;i<3; i++){
      digitalWrite(LEDs[i], initialState);
    }
    tone(buzzerPin, frequency(notesNextGame[j]), 200);
    delay(200);
    initialState = !initialState;
  }
}


// LED blink patterm when the game draws
void blinkDrawLeds(int LEDs[]){
  bool initialState = true;
  for(int j=0; j<5;j++){
    for(int i=0;i<18; i++){
      digitalWrite(LEDs[i], initialState);
    }
    tone(buzzerPin, frequency(notesNextGame[j]), 200);
    delay(200);
    initialState = !initialState;
  }
}

// Conditions to check if someone won the game
bool isSomeoneWinner(){
  if( mainBoard[0] == mainBoard[1] && mainBoard[1] == mainBoard[2] && mainBoard[0] != '-' ){
    int playerOneWinningLeds[] = {34,36,38};
    int playerTwoWinningLeds[] = {35,37,39};
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
  } else if( mainBoard[3] == mainBoard[4] && mainBoard[4] == mainBoard[5] && mainBoard[3] != '-'){
    int playerOneWinningLeds[] = {28,30,32};
    int playerTwoWinningLeds[] = {29,31,33};
    if(mainBoard[3] == 'X'){
      playerOneScore++;
    } else if (mainBoard[3] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[6] == mainBoard[7] && mainBoard[7] == mainBoard[8] && mainBoard[6] != '-'){
    int playerOneWinningLeds[] = {22,24,26};
    int playerTwoWinningLeds[] = {23,35,27};
    if(mainBoard[6] == 'X'){
      playerOneScore++;
    } else if (mainBoard[6] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[0] == mainBoard[3] && mainBoard[3] ==  mainBoard[6] && mainBoard[0] != '-'){
    int playerOneWinningLeds[] = {34,28,22};
    int playerTwoWinningLeds[] = {35,29,23};
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[1] == mainBoard[4] && mainBoard[4] == mainBoard[7] && mainBoard[1] != '-'){
    int playerOneWinningLeds[] = {36,30,24};
    int playerTwoWinningLeds[] = {37,31,25};
    if(mainBoard[1] == 'X'){
      playerOneScore++;
    } else if (mainBoard[1] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[1] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[2] == mainBoard[5] && mainBoard[5] == mainBoard[8] && mainBoard[2] != '-'){
    int playerOneWinningLeds[] = {26,32,38};
    int playerTwoWinningLeds[] = {27,33,39};
    if(mainBoard[2] == 'X'){
      playerOneScore++;
    } else if (mainBoard[2] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[0] == mainBoard[4] && mainBoard[4] == mainBoard[8] && mainBoard[0] != '-'){
    int playerOneWinningLeds[] = {34,30,26};
    int playerTwoWinningLeds[] = {35,31,27};
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if(mainBoard[2] == mainBoard[4] && mainBoard[4] == mainBoard[6] && mainBoard[2] != '-'){
    int playerOneWinningLeds[] = {22,30,38};
    int playerTwoWinningLeds[] = {23,31,39};
    if(mainBoard[2] == 'X'){
      playerOneScore++;
    } else if (mainBoard[2] == 'O'){
      playerTwoScore++;
    }
    blinkWinningLeds(mainBoard[0] == 'X' ? playerOneWinningLeds : playerTwoWinningLeds);
    nextGame();
    return true;
    
  } else if (mainBoard[0] != '-' && mainBoard[1] != '-' && mainBoard[2] != '-' && mainBoard[3] != '-' && mainBoard[4] != '-' && mainBoard[5] != '-' && mainBoard[6] != '-' && mainBoard[7] != '-' && mainBoard[8] != '-'){
    int playerOneWinningLeds[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
    Serial.println("Game Draw");
    blinkDrawLeds(playerOneWinningLeds);
    resetMainBoard();
    displayPlayerScore(playerOneScore, playerTwoScore);
    roundNo++;
    resetLEDs();
  }
  return false;
}

void nextGame(){
  firstTurn = (firstTurn+1)%2;
  turn = firstTurn;
  resetMainBoard();
  displayPlayerScore(playerOneScore, playerTwoScore);
  roundNo++;
  resetLEDs();
}

// ===== Display/LED Related Functions Begin ===== //

void turnLED(){
  digitalWrite(turn == 0 ? 52 : 53, HIGH);
  digitalWrite(turn == 0 ? 53 : 52, LOW);
}

void displayPlayerScore(int scoreOne, int ScoreTwo){
  scoreboardDisplay.writeDigitAscii(0, 32);
  scoreboardDisplay.writeDigitAscii(1, asciiTableValue(scoreOne));
  scoreboardDisplay.writeDigitAscii(2, 32);
  scoreboardDisplay.writeDigitAscii(3, asciiTableValue(ScoreTwo));
  scoreboardDisplay.writeDisplay();
}

void printRoundDetails(){
  lcd.clear();
  lcd.setCursor(0,0);
  String msg = "Round ";
  msg.concat(roundNo);
  lcdPrint(msg, turn == 0 ? "Player 1's turn" : "Player 2's turn", false, false);
  Serial.println("Round Printd");
}

void lcdPrint(String line1, String line2, bool isMsgOneScroll, bool isMsgTwoScroll){
   lcd.setCursor(0,0);
   lcd.print(line1);
   if(isMsgOneScroll) lcd.scrollDisplayLeft();
   lcd.setCursor(0,1);
   lcd.print(line2);
   if(isMsgTwoScroll) lcd.scrollDisplayLeft();
}

void sendToTimer(float num){
  timer.print(num);    
  timer.writeDisplay();
}

// ===== Display/LED Related Functions End   ===== //

// ===== Reset Related Functions Begin ===== //

void resetGame(){
  roundNo = 1;
  playerOneScore = 0;
  playerTwoScore = 0;
  displayPlayerScore(playerOneScore, playerTwoScore);
  resetMainBoard();
  lcd.clear();
  lcdPrint("Game Resetting", "", false, false);
  sendToTimer(1.00);    
  resetLEDs();
  playBuzzer(beatsStart, notesStart);
  printRoundDetails();
}

void resetMainBoard(){
  for(int i=0; i < 9; i++){
    mainBoard[i] = '-';
  }
}

void resetLCD(){
  lcd.clear();
}

void resetLEDs(){
  for(int i =0; i<sizeof(LEDsArray)/sizeof(int); i++){
    digitalWrite(LEDsArray[i], LOW);  
  }
}


void resetScoreBoard(){
  scoreboardDisplay.writeDigitAscii(0, 32);
  scoreboardDisplay.writeDigitAscii(1, 48);
  scoreboardDisplay.writeDigitAscii(2, 32);
  scoreboardDisplay.writeDigitAscii(3, 48);
  scoreboardDisplay.writeDisplay(); 
}

// ===== Reset Related Functions End  ===== //

// ===== Buzzer Related Functions Begin =====  (Buzzer related code credits - https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v32/experiment-11-using-a-piezo-buzzer)//

void playBuzzer(int beats[], char notes[]){
  int i, duration;

  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // is this a rest? 
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }
}

void playErrorBuzzer(int beats[], char notes[]){
  tone(buzzerPin, frequency(notes[0]), 100);
}

void playNextGameBuzzer(int beats[], char notes[]){
  tone(buzzerPin, frequency(notes[0]), 200);
  delay(200);
  tone(buzzerPin, frequency(notes[1]), 200);
  delay(200);
  tone(buzzerPin, frequency(notes[2]), 200);
  delay(200);
  tone(buzzerPin, frequency(notes[3]), 200);
  delay(200);
  tone(buzzerPin, frequency(notes[4]), 200);
  delay(200);
}

int frequency(char note) 
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;
  const int numNotes = 8;  // number of notes we're storing

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // We looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
}

// ===== Buzzer Related Functions End   ===== //

// ===== Helper Related Functions Start   ===== //

int asciiTableValue(int num){
  switch (num){
    case 0:
      return 48;
    case 1:
      return 49; 
    case 2:
      return 50; 
    case 3:
      return 51; 
    case 4:
      return 52; 
    case 5:
      return 53; 
    case 6:
      return 54; 
    case 7:
      return 55; 
    case 8:
      return 56; 
    case 9:
      return 57; 
  }
}

// ===== Helper Related Functions End   ===== //
