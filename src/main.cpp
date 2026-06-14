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

#define y 34
#define x 35
#define buttom 23

const int green[3][3] = {
  {l001, l011, l021},
  {l101, l111, l121}, 
  {l201, l211, l221}
};

const int blue[3][3] = {
  {l002, l012, l022},
  {l102, l112, l122}, 
  {l202, l212, l222}
};

void setup() {
  Serial.begin(115200);

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
        pinMode(green[i][j], OUTPUT);
        pinMode(blue[i][j], OUTPUT);
    }
  }

  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(buttom, INPUT_PULLDOWN);
}

int matriz[3][3] = {
  {0, 0, 0},
  {0, 1, 0}, 
  {0, 0, 0}
};

int readx;
int ready;
int posx = 1;
int posy = 1;

void loop() {

  if(analogRead(x) > 4000){
    readx = -1;
  }
  else if(analogRead(x) < 1000){
    readx = 1;
  }
  else{
    readx = 0;
  }

    if(analogRead(y) > 4000){
    ready = -1;
  }
  else if(analogRead(y) < 1000){
    ready = 1;
  }
  else{
    ready = 0;
  }

  Serial.print("X: ");
  Serial.println(readx);
  Serial.print("Y: ");
  Serial.println(ready);


if (readx != 0 || ready != 0){
  matriz[posy][posx] = 0;
  posx += readx;
  posy += ready;
  if(posx < 0){
    posx = 0;
  }
  else if(posx > 2){
    posx = 2;
  }

   if(posy < 0){
    posy = 0;
  }
  else if(posy > 2){
    posy = 2;
  }

  matriz[posy][posx] = 1;
}
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if(matriz[i][j] == 1){
        digitalWrite(blue[i][j], HIGH);
      }else{
        digitalWrite(blue[i][j], LOW);
      }
    }
  }
 delay(100);
}