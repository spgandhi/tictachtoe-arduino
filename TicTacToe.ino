#include <Button.h>

bool isGameOn = false;

long gameTime = 120000;
int turn = 0;
int firstTurn = 0;
int playerOneScore = 0;
int playerTwoScore = 0;

char mainBoard[9] = {'-', '-', '-', '-', '-', '-', '-', '-', '-'}; 

const int btnPin_0_0 = 2;
const int btnPin_0_1 = 3;
const int btnPin_0_2 = 4;
const int btnPin_1_0 = 5;
const int btnPin_1_1 = 6;
const int btnPin_1_2 = 7;
const int btnPin_2_0 = 8;
const int btnPin_2_1 = 9;
const int btnPin_2_2 = 10;

Button gameButtons[9] = {
  Button (btnPin_0_0),
  Button (btnPin_0_1),
  Button (btnPin_0_2),
  Button (btnPin_1_0),
  Button (btnPin_1_1),
  Button (btnPin_1_2),
  Button (btnPin_2_0),
  Button (btnPin_2_1),
  Button (btnPin_2_2)
};

Button startBtn (12);
Button resetBtn (13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(startBtn.checkButtonAction() == Button::CLICKED){
    Serial.println("Game starts now");
    startGame();
  }
}

void resetGame(){
  playerOneScore = 0;
  playerTwoScore = 0;
  turn = 0;
  firstTurn = 0;
  resetMainBoard();
  Serial.println("Game Reset");
}

void startGame(){
  unsigned long startTime = millis();
  while(millis() - startTime < gameTime){
    if(resetBtn.checkButtonAction() == Button::CLICKED){
      resetGame();
      startTime = millis();
    }
    for(int i=0;i<9;i++){
      int action = gameButtons[i].checkButtonAction();
      if(action == Button::CLICKED){
        Serial.print("Clicked by - ");
        Serial.println(turn);
        playStep(i);
        printMainBoard();      
      }
    }
  }
}

void printMainBoard(){
  Serial.print(mainBoard[0]);
  Serial.print(" | ");
  Serial.print(mainBoard[1]);
  Serial.print(" | ");
  Serial.println(mainBoard[2]);

  Serial.print(mainBoard[3]);
  Serial.print(" | ");
  Serial.print(mainBoard[4]);
  Serial.print(" | ");
  Serial.println(mainBoard[5]);

  Serial.print(mainBoard[6]);
  Serial.print(" | ");
  Serial.print(mainBoard[7]);
  Serial.print(" | ");
  Serial.println(mainBoard[8]);
  
}

void playStep(int index){
  if(mainBoard[index] == '-'){
    mainBoard[index] = turn == 0 ? 'X' : 'O';
    if(!isSomeoneWinner()) turn = (turn + 1)%2;
  } else {
    Serial.println("Wrong input");
  }
}

void nextGame(){
  firstTurn = (firstTurn+1)%2;
  turn = firstTurn;
  resetMainBoard();
  Serial.println("New Game");
  Serial.print("PlayerOne = ");
  Serial.println(playerOneScore);
  Serial.print("PlayerTwo = ");
  Serial.println(playerTwoScore);
}

void resetMainBoard(){
  for(int i=0; i < 9; i++){
    mainBoard[i] = '-';
  }
}

bool isSomeoneWinner(){
  if( mainBoard[0] == mainBoard[1] && mainBoard[1] == mainBoard[2] && mainBoard[0] != '-' ){
    Serial.println(mainBoard[0]);
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
  } else if( mainBoard[3] == mainBoard[4] == mainBoard[5] && mainBoard[3] != '-'){
    Serial.println(mainBoard[3]);
    if(mainBoard[3] == 'X'){
      playerOneScore++;
    } else if (mainBoard[3] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[6] == mainBoard[7] == mainBoard[8] && mainBoard[6] != '-'){
    Serial.println(mainBoard[6]);
    if(mainBoard[6] == 'X'){
      playerOneScore++;
    } else if (mainBoard[6] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[0] == mainBoard[3] == mainBoard[6] && mainBoard[0] != '-'){
    Serial.println(mainBoard[0]);
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[1] == mainBoard[4] == mainBoard[7] && mainBoard[1] != '-'){
    Serial.println(mainBoard[1]);
    if(mainBoard[1] == 'X'){
      playerOneScore++;
    } else if (mainBoard[1] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[2] == mainBoard[5] == mainBoard[8] && mainBoard[2] != '-'){

    Serial.println(mainBoard[2]);
    if(mainBoard[2] == 'X'){
      playerOneScore++;
    } else if (mainBoard[2] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[0] == mainBoard[4] == mainBoard[8] && mainBoard[0] != '-'){

    Serial.println(mainBoard[0]);
    if(mainBoard[0] == 'X'){
      playerOneScore++;
    } else if (mainBoard[0] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  } else if(mainBoard[2] == mainBoard[4] == mainBoard[6] && mainBoard[2] != '-'){

    Serial.println(mainBoard[2]);
    if(mainBoard[2] == 'X'){
      playerOneScore++;
    } else if (mainBoard[2] == 'O'){
      playerTwoScore++;
    }
    nextGame();
    return true;
    
  }
  return false;
}
