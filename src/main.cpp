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

void loop() {

  

}