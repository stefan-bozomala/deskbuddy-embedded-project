#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RoboEyes<Adafruit_SSD1306> roboEyes(display); 

// --- CONFIGURARE PINI ---
const int IR_PIN = 2;       // Pinul pentru senzorul IR
const int BUZZER_PIN = 3;   // Pinul pentru Buzzer
const int LDR_PIN = A0;     // Pinul analogic pentru fotorezistenta

// --- VARIABILE DE STARE ---
bool obiectInFata = false;
bool esteFuriosLumina = false;

// --- CALIBRARE LUMINA ---
// Ajusteaza aceasta valoare daca robotul devine furios prea repede sau prea greu.
// Valorile citite sunt intre 0 (intuneric complet) si 1023 (lumina maxima).
const int PRAG_LUMINA_PUTERNICA = 200; 

void setup() {
  Serial.begin(9600);

  pinMode(IR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initializare OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  // Pornire ochi de robot
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100); 

  // Comportament automatizat de baza
  roboEyes.setAutoblinker(ON, 3, 2); 
  roboEyes.setIdleMode(ON, 2, 2); 
  roboEyes.setCuriosity(ON); 
} 

void loop() {
  roboEyes.update(); // Actualizeaza animatiile ochilor in timp real

  // 1. Citim nivelul de lumina de la fotorezistenta
  int valoareLumina = analogRead(LDR_PIN);
  
  // 2. Citim starea senzorului IR
  int stareSenzor = digitalRead(IR_PIN);

  // --- LOGICA PENTRU LUMINA (Prioritate maxima) ---
  if (valoareLumina > PRAG_LUMINA_PUTERNICA) {
    
    if (!esteFuriosLumina) {
      roboEyes.setMood(ANGRY);          // Îl face furios
      esteFuriosLumina = true;
      obiectInFata = false;             // Resetam starea IR ca sa nu se confunde statele
      Serial.print("Prea multa lumina! Valoare: ");
      Serial.println(valoareLumina);
    }
    
  } 
  // --- LOGICA PENTRU SENZORUL IR (Rulare doar daca lumina e normala) ---
  else {
    
    // Daca tocmai a scazut lumina, revenim la normal
    if (esteFuriosLumina) {
      roboEyes.setMood(DEFAULT);
      esteFuriosLumina = false;
      Serial.println("Lumina a revenit la normal.");
    }

    // Verificam obstacolul IR
    if (stareSenzor == LOW) {
      if (!obiectInFata) {
        roboEyes.setMood(HAPPY);        // Ochii zâmbesc
        radeDe3Ori();                   // Sunetul de ras din buzzer
        obiectInFata = true; 
        Serial.println("Obstacol detectat! Robotul rade.");
      }
    } else {
      if (obiectInFata) {
        roboEyes.setMood(DEFAULT);      // Ochii revin la normal cand obiectul pleaca
        obiectInFata = false; 
        Serial.println("Drum liber.");
      }
    }
    
  }
}

// --- FUNCTIA PENTRU RÂSET ---
void radeDe3Ori() {
  tone(BUZZER_PIN, 1400); 
  delay(80);              
  noTone(BUZZER_PIN);     
  delay(60);              

  tone(BUZZER_PIN, 1200); 
  delay(80);
  noTone(BUZZER_PIN);
  delay(60);

  tone(BUZZER_PIN, 1300); 
  delay(80);
  noTone(BUZZER_PIN);
}