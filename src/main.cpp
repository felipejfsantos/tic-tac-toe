#include <Arduino.h>

// Definição dos Pinos dos LEDs Verdes (Jogador 1)
#define l001 15
#define l011 4
#define l021 17
#define l101 18
#define l111 21 
#define l121 32
#define l201 25
#define l211 27
#define l221 12 

// Definição dos Pinos dos LEDs Azuis (Jogador 2)
#define l002 2
#define l012 16
#define l022 5
#define l102 19
#define l112 22 
#define l122 33
#define l202 26
#define l212 14
#define l222 13 

// Pinos do Joystick e Botão
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

// --- VARIÁVEIS DO JOGO DA VELHA ---
int pinMtxF[3][3] = { // Armazena as jogadas confirmadas (0: vazio, 1: Jogador 1, 2: Jogador 2)
  {0, 0, 0},
  {0, 0, 0}, 
  {0, 0, 0}
};

int cursorX = 1; // Posição X do cursor (coluna de 0 a 2)
int cursorY = 1; // Posição Y do cursor (linha de 0 a 2)

int rodada = 1;        // Controle de turnos
bool gameOver = false; 
bool joyDedicado = false; // Trava para exigir que o jogador solte o joystick antes de mover de novo
bool botaoPressionado = false; // Trava para clique único do botão

// Funções auxiliares
void atualizarPainelLEDs();
void verificarEstadoJogo();
void reiniciarJogo();

void setup() {
  // Configura os pinos dos LEDs como saída
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
        pinMode(greenLEDs[i][j], OUTPUT);
        pinMode(blueLEDs[i][j], OUTPUT);
    }
  }

  // Configura os inputs do controle
  pinMode(PIN_X, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLDOWN); // Garante nível 0 quando solto
  
  reiniciarJogo();
}

void loop() {
  // Se o jogo acabou, espera um clique no botão para reiniciar
  if (gameOver) {
    if (digitalRead(PIN_BUTTON) == HIGH) {
      delay(200); // Debounce rápido
      reiniciarJogo();
    }
    return; 
  }

  // 1. LEITURA DO JOYSTICK
  int readX = 0;
  int readY = 0;

  if(analogRead(PIN_X) > 4000)      readX = -1; // Esquerda
  else if(analogRead(PIN_X) < 1000) readX = 1;  // Direita

  if(analogRead(PIN_Y) > 4000)      readY = -1; // Cima
  else if(analogRead(PIN_Y) < 1000) readY = 1;  // Baixo

  // Movimentação com trava (só move 1 bloco por vez, precisa voltar o joystick ao centro)
  if (readX == 0 && readY == 0) {
    joyDedicado = false; // Joystick voltou ao centro, liberado para mover novamente
  } 
  else if (!joyDedicado) {
    cursorX += readX;
    cursorY += readY;

    // Limites do tabuleiro (0 a 2)
    if(cursorX < 0) cursorX = 0;
    if(cursorX > 2) cursorX = 2;
    if(cursorY < 0) cursorY = 0;
    if(cursorY > 2) cursorY = 2;

    joyDedicado = true; // Trava o movimento
  }

  // 2. LEITURA DO BOTÃO DE CONFIRMAÇÃO
  if (digitalRead(PIN_BUTTON) == HIGH && !botaoPressionado) {
    botaoPressionado = true;

    // Verifica se a casa atual está livre
    if (pinMtxF[cursorY][cursorX] == 0) {
      // Salva a jogada baseando-se no turno atual
      if (rodada % 2 != 0) {
        pinMtxF[cursorY][cursorX] = 1; // Jogador 1 (Verde)
      } else {
        pinMtxF[cursorY][cursorX] = 2; // Jogador 2 (Azul)
      }
      
      verificarEstadoJogo(); // Processa vitórias ou empate
      
      if (!gameOver) {
        rodada++; // Passa o turno
      }
    } 
    else {
      // Efeito visual de erro: Pisca os leds se tentar jogar em local ocupado
      for(int k=0; k<3; k++) {
        atualizarPainelLEDs(); delay(70);
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], LOW); }
        delay(70);
      }
    }
  }

  if (digitalRead(PIN_BUTTON) == LOW) {
    botaoPressionado = false; // Libera o botão para o próximo clique
  }

  // 3. ATUALIZAÇÃO VISUAL DOS LEDS
  if (!gameOver) {
    atualizarPainelLEDs();
  }
  
  delay(10); // Estabilidade do loop
}

// Atualiza a matriz física de LEDs baseada no estado interno do jogo
void atualizarPainelLEDs() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      // 1. Acende as peças fixas já jogadas
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

      // 2. Sobreponha o cursor piscando do jogador atual
      if (i == cursorY && j == cursorX) {
        // Efeito de piscar controlado pelo tempo interno do processador (millis)
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

// Toda a lógica de análise matemática transposta do seu arquivo C
void verificarEstadoJogo() {
  bool vit1 = false;
  bool vit2 = false;

  // Análise matemática de vitória (linhas, colunas e diagonais)
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

  // Se o Jogador 1 vencer: Pisca a matriz toda em VERDE
  if (vit1) {
    gameOver = true;
    for(int k=0; k<5; k++) {
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], HIGH); digitalWrite(blueLEDs[i][j], LOW); }
      delay(300);
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) digitalWrite(greenLEDs[i][j], LOW);
      delay(300);
    }
  } 
  // Se o Jogador 2 vencer: Pisca a matriz toda em AZUL
  else if (vit2) {
    gameOver = true;
    for(int k=0; k<5; k++) {
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], LOW); digitalWrite(blueLEDs[i][j], HIGH); }
      delay(300);
      for(int i=0; i<3; i++) for(int j=0; j<3; j++) digitalWrite(blueLEDs[i][j], LOW);
      delay(300);
    }
  } 
  // Verificação de Velha (tabuleiro cheio)
  else {
    int emptyBlocks = 0;
    for(int i = 0; i < 3; i++) for(int j = 0; j < 3; j++) if(pinMtxF[i][j] == 0) emptyBlocks++;
    
    if(emptyBlocks == 0) { // Deu Velha! Pisca tudo junto (Verde + Azul)
      gameOver = true;
      for(int k=0; k<5; k++) {
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) { digitalWrite(greenLEDs[i][j], HIGH); digitalWrite(blueLEDs[i][j], HIGH); }
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