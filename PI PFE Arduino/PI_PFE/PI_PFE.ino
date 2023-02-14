// Configuration du lcd:

#include <LiquidCrystal.h>
const int rs = 9, e = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, e, d4, d5, d6, d7);


// Configuration des servomoteurs:

#include <Servo.h>
Servo battant1, battant2;

// Configuration du keypad:

#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {18, 19, 20, 21};
byte colPins[COLS] = {17, 16, 15};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configuration du password:

#include <Password.h>
Password password = Password("1920");

// Configuration des pins:

int BP_acces = 11, BP_sortie = 10;
int lampe_rouge = 13, lampe_verte = 12;
int relais = 14;

// Configuration des indices:

int j, k, m = 0; // j : lié à l'état du BP_access , k : lié vérification du code pin et m : lié à la présence d'un personnel dans la zone

void setup() {
  // put your setup code here, to run once:

  lcd.begin(16, 2);

  battant1.attach(3);
  battant2.attach(2);
  battant1.write(0);
  battant2.write(0);

  keypad.addEventListener(keypadEvent);
  keypad.setDebounceTime(50);

  for (int i = 10; i <= 14; i++) {

    if (i < 12) {
      pinMode(i, INPUT);
    } else {
      pinMode(i, OUTPUT);
    }

  }

  digitalWrite(lampe_rouge, LOW);
  digitalWrite(lampe_verte, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("HOPITAL  SOUISSI");
  lcd.setCursor(1, 1);
  lcd.print("LIMITED ACCESS");

  initial(); // exécution de la sous-boucle acces

}



void loop() {
  // put your main code here, to run repeatedly:

  keypad.getKey();
  acces(); // exécution de la sous-boucle acces()
  sortie(); // exécution de la sous-boucle sortie()

}


void initial() {
  // état initial:

  if (m == 0) {

    j = 0;
    k = 0;
    lcd.setCursor(0, 0);
    lcd.print("HOPITAL  SOUISSI");
    lcd.setCursor(1, 1);
    lcd.print("LIMITED ACCESS");

  }

}


void acces() {
  // demande d’accès à la zone:

  int etat_BP_acces;
  etat_BP_acces = digitalRead(BP_acces);

  if (etat_BP_acces == HIGH && m == 0) {

    j++;
    lcd.setCursor(1, 1);
    lcd.print("ENTER YOUR PIN");
    delay(1000);
    lcd.clear();

  }

}


void keypadEvent(KeypadEvent eKey) {
  // entrée du code PIN:

  if (j != 0) {

    switch (keypad.getState()) {

      case PRESSED:
        lcd.print(" *  ");
        switch (eKey) {
          case '#': guessPassword(); break; // # pour valider le code et accéder à la sous-boucle guessPassword()
            lcd.setCursor(0, 1);
          default: password.append(eKey); // considérer la touche saisie si elle est différente de #

        }

    }

  }

  else {
    initial();
  }

}


void guessPassword () {
  // vérification du code PIN:

  if (j != 0) {

    if (password.evaluate()) { //password.evaluate() : fonction validant le code PIN

      k++;
      lcd.setCursor(0, 0);
      lcd.print("HOPITAL  SOUISSI");
      lcd.setCursor(1, 1);
      lcd.print("  CHECK TEMP  ");
      password.reset(); //password.reset() : fonction supprimant le code PIN
      delay(1000);
      j--;
      entree(); // exécution de la sous-boucle entree()

    }

    else {

      lcd.setCursor(0, 0);
      lcd.print("HOPITAL  SOUISSI");
      lcd.setCursor(1, 1);
      lcd.print("  WRONG  PIN  ");
      delay(10000);
      password.reset();
      initial();

    }

  }

}


void entree() {
  // actions après autorisation d'accès à la zone:

  float temp, lect;
  temp = analogRead(0);
  lect = map(temp, 0, 1023, -10, 100);

  if (k != 0) {

    if (lect < 37.5) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    TEMP (C)    ");
      lcd.setCursor(0, 1);
      lcd.setCursor(5, 1);
      lcd.print(lect);
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("HOPITAL  SOUISSI");
      lcd.setCursor(1, 1);
      lcd.print(" DOOR OPENING ");
      battant1.write(90);
      battant2.write(90);
      delay(15000);
      battant1.write(0);
      battant2.write(0);
      digitalWrite(lampe_rouge, HIGH);
      digitalWrite(lampe_verte, LOW);
      occupe(); // exécution de la sous-boucle occupe()

    }

    else {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    TEMP (C)    ");
      lcd.setCursor(0, 1);
      lcd.setCursor(5, 1);
      lcd.print(lect);
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("HOPITAL  SOUISSI");
      lcd.setCursor(1, 1);
      lcd.print("ACCESS  DENIED");
      delay(10000);
      initial();

    }

    k--;

  }

}


void occupe() {
  // instructions s'il y a une personne dans la zone:

  lcd.setCursor(0, 0);
  lcd.print("HOPITAL  SOUISSI");
  lcd.setCursor(1, 1);
  lcd.print("  BUSY  AREA  ");
  m++;

}


void sortie() {
  // demande de sortie:

  if (m != 0) {

    int etat_BP_sortie;
    etat_BP_sortie = digitalRead(BP_sortie);

    if (etat_BP_sortie == HIGH) {

      delay(2000);
      digitalWrite(relais, HIGH);
      delay(20000);
      digitalWrite(relais, LOW);
      battant1.write(90);
      battant2.write(90);
      delay(15000);
      battant1.write(0);
      battant2.write(0);
      digitalWrite(lampe_rouge, LOW);
      digitalWrite(lampe_verte, HIGH);
      m--;
      initial();

    }

  }

}
