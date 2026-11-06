#ifndef microstim_h
#define microstim_h
#include <Arduino.h>



int CS_PIN  = 23;
int UD_PIN  = 21;
int INC_PIN = 22;

int OPTO_PIN1 = 18;
int OPTO_PIN2 = 19;

void set_resistance(int percent) {

    percent = constrain(percent, 0, 100);

    // Zerar resistência
    digitalWrite(UD_PIN, LOW);
    for(int i = 0; i < 100; i++) {
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(CS_PIN, HIGH);
        delayMicroseconds(5);
    }

    // Posicionar na resistência desejada
    digitalWrite(UD_PIN, HIGH);
    for(int i = 0; i < percent; i++) {
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(CS_PIN, HIGH);
        delayMicroseconds(5);
    }
}

void init_micro_stim(){
    pinMode(CS_PIN, OUTPUT);
    pinMode(UD_PIN, OUTPUT);
    pinMode(INC_PIN, OUTPUT);
    pinMode(OPTO_PIN1, OUTPUT);
    pinMode(OPTO_PIN2, OUTPUT);

    digitalWrite(CS_PIN, HIGH);
    digitalWrite(INC_PIN, HIGH);

    // valor inicial de intensidade (pode ser ajustado)
    set_resistance(10);
}



#endif // microstim_h