#include <Arduino.h>

// Verdes
#define l001 15
#define l011 4
#define l021 17
#define l101 18
#define l111 21 
#define l121 32
#define l201 25
#define l211 27
#define l221 12 

// Azuis
#define l002 2
#define l012 16
#define l022 5
#define l102 19
#define l112 22 
#define l122 33
#define l202 26
#define l212 14
#define l222 13 

#define PIN_Y 34
#define PIN_X 35
#define PIN_BUTTON 23

// Matrizes de mapeamento de pinos
const int greenLEDs[3][3] = {
  {l001, l011, l021},
  {l101, l111, l121}, 
  {l201, l211, l221}
};

const int blueLEDs[3][3] = {
  {l002, l012, l022},
  {l102, l112, l122}, 
  {l202, l212, l222}
};


int pinMtxF[3][3] = { // Jogadas confirmadas (0: vazio, 1: Verde 2: Azul)
  {0, 0, 0},
  {0, 0, 0}, 
  {0, 0, 0}
};

int mtxEmpate[3][3] = {
  {1, 0, 1},
  {0, 1, 0},
  {1, 0, 1}
};

int cursorX = 1; // Posição X do cursor 
int cursorY = 1; // Posição Y do cursor 

int rodada = 1;        // Controle de turnos
bool gameOver = false; 
bool joyDedicado = false; 
bool botaoPressionado = false; // Clique único do botão

void atualizarPainelLEDs();
void verificarEstadoJogo();
void reiniciarJogo();

void setup() {
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
        pinMode(greenLEDs[i][j], OUTPUT);
        pinMode(blueLEDs[i][j], OUTPUT);
    }
  }

  pinMode(PIN_X, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLDOWN); 
  
  reiniciarJogo();
}

void loop() {

  if (gameOver) {
    if (digitalRead(PIN_BUTTON) == HIGH) {
      delay(200); 
      reiniciarJogo();
    }
    return; 
  }

  int readX = 0;
  int readY = 0;

  if(analogRead(PIN_X) > 4000)      readX = -1; // Esquerda
  else if(analogRead(PIN_X) < 1000) readX = 1;  // Direita

  if(analogRead(PIN_Y) > 4000)      readY = -1; // Cima
  else if(analogRead(PIN_Y) < 1000) readY = 1;  // Baixo

  if (readX == 0 && readY == 0) {
    joyDedicado = false; 
  } 
  else if (!joyDedicado) {
    cursorX += readX;
    cursorY += readY;

    if(cursorX < 0) cursorX = 0;
    if(cursorX > 2) cursorX = 2;
    if(cursorY < 0) cursorY = 0;
    if(cursorY > 2) cursorY = 2;

    joyDedicado = true; 
  }

  if (digitalRead(PIN_BUTTON) == HIGH && !botaoPressionado) {
    botaoPressionado = true;

    if (pinMtxF[cursorY][cursorX] == 0) {
      if (rodada % 2 != 0) {
        pinMtxF[cursorY][cursorX] = 1; 
      } else {
        pinMtxF[cursorY][cursorX] = 2; 
      }
      
      verificarEstadoJogo(); // Vitórias ou empate
      
      if (!gameOver) {
        rodada++; 
      }
    } 
    else {
      // Pisca os leds se tentar jogar em local ocupado
      for(int k=0; k<3; k++) {
        atualizarPainelLEDs(); delay(70);
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], LOW); }
        delay(70);
      }
    }
  }

  if (digitalRead(PIN_BUTTON) == LOW) {
    botaoPressionado = false; 
  }

  if (!gameOver) {
    atualizarPainelLEDs();
  }
  
  delay(10); 
}

// Atualiza a matriz física de LEDs 
void atualizarPainelLEDs() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (pinMtxF[i][j] == 1) {
        digitalWrite(greenLEDs[i][j], HIGH);
        digitalWrite(blueLEDs[i][j], LOW);
      } 
      else if (pinMtxF[i][j] == 2) {
        digitalWrite(greenLEDs[i][j], LOW);
        digitalWrite(blueLEDs[i][j], HIGH);
      } 
      else {
        digitalWrite(greenLEDs[i][j], LOW);
        digitalWrite(blueLEDs[i][j], LOW);
      }


      if (i == cursorY && j == cursorX) {
        // Piscar o LED selecionado
        if ((millis() % 400) < 200) { 
          if (rodada % 2 != 0) {
            digitalWrite(greenLEDs[i][j], HIGH); // Cursor do Jogador 1
          } else {
            digitalWrite(blueLEDs[i][j], HIGH);  // Cursor do Jogador 2
          }
        }
      }
    }
  }
}

void verificarEstadoJogo() {
  bool vit1 = false;
  bool vit2 = false;

  // Condições de vitória
  if((pinMtxF[0][0] == 1 && pinMtxF[0][1] == 1 && pinMtxF[0][2] == 1) ||
     (pinMtxF[1][0] == 1 && pinMtxF[1][1] == 1 && pinMtxF[1][2] == 1) ||
     (pinMtxF[2][0] == 1 && pinMtxF[2][1] == 1 && pinMtxF[2][2] == 1) ||
     (pinMtxF[0][0] == 1 && pinMtxF[1][0] == 1 && pinMtxF[2][0] == 1) ||
     (pinMtxF[0][1] == 1 && pinMtxF[1][1] == 1 && pinMtxF[2][1] == 1) ||
     (pinMtxF[0][2] == 1 && pinMtxF[1][2] == 1 && pinMtxF[2][2] == 1) ||
     (pinMtxF[0][0] == 1 && pinMtxF[1][1] == 1 && pinMtxF[2][2] == 1) ||
     (pinMtxF[0][2] == 1 && pinMtxF[1][1] == 1 && pinMtxF[2][0] == 1)) {
      vit1 = true;
  } 
  else if((pinMtxF[0][0] == 2 && pinMtxF[0][1] == 2 && pinMtxF[0][2] == 2) ||
          (pinMtxF[1][0] == 2 && pinMtxF[1][1] == 2 && pinMtxF[1][2] == 2) ||
          (pinMtxF[2][0] == 2 && pinMtxF[2][1] == 2 && pinMtxF[2][2] == 2) ||
          (pinMtxF[0][0] == 2 && pinMtxF[1][0] == 2 && pinMtxF[2][0] == 2) ||
          (pinMtxF[0][1] == 2 && pinMtxF[1][1] == 2 && pinMtxF[2][1] == 2) ||
          (pinMtxF[0][2] == 2 && pinMtxF[1][2] == 2 && pinMtxF[2][2] == 2) ||
          (pinMtxF[0][0] == 2 && pinMtxF[1][1] == 2 && pinMtxF[2][2] == 2) ||
          (pinMtxF[0][2] == 2 && pinMtxF[1][1] == 2 && pinMtxF[2][0] == 2)) {
      vit2 = true;
  }

  if (vit1) {
    gameOver = true;
    for(int k=0; k<5; k++) {
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], HIGH); digitalWrite(blueLEDs[i][j], LOW); }
      delay(300);
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) digitalWrite(greenLEDs[i][j], LOW);
      delay(300);
    }
  } 
  else if (vit2) {
    gameOver = true;
    for(int k=0; k<5; k++) {
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], HIGH); }
      delay(300);
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) digitalWrite(blueLEDs[i][j], LOW);
      delay(300);
    }
  } 
  // Velha
  else {
    int emptyBlocks = 0;
    for(int i = 0; i < 3; i++) for(int j = 0; j < 3; j++) if(pinMtxF[i][j] == 0) emptyBlocks++;
    
    if(emptyBlocks == 0) { 
      gameOver = true;
      for(int k=0; k<5; k++) {
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) { if(mtxEmpate[i][j] == 1){
          digitalWrite(greenLEDs[i][j], HIGH); digitalWrite(blueLEDs[i][j], HIGH); 
        }else{digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], LOW);}
        }
        delay(300);
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], LOW); }
        delay(300);
      }
    }
  }
}

void reiniciarJogo() {
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      pinMtxF[i][j] = 0;
      digitalWrite(greenLEDs[i][j], LOW);
      digitalWrite(blueLEDs[i][j], LOW);
    }
  }
  rodada = 1;
  cursorX = 1;
  cursorY = 1;
  gameOver = false;
}