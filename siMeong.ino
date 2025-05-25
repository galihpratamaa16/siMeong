// Library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <Servo.h>

// Konfigurasi Pin
#define DT D5     // HX711 
#define SCK D6    // HX711 
#define TRIG_PIN D7 // Ultrasonik
#define ECHO_PIN D8 // Ultrasonik
#define SERVO_PIN D3 // Servo

// Objek
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address bisa 0x3F (jika perlu diganti)
HX711 scale;
Servo servoMotor;

// Konstanta
const float hargaPerKg = 1000.0; // nilai uang per kg
const int jarakTrigger = 5;     // jarak untuk deteksi (dalam cm)

// Setup logika sekali
void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Si Meong");
  delay(2000);
  lcd.clear();

// Kode hasil GPT
  scale.begin(DT, SCK);
  // jika sudah kalibrasi langsung input
  scale.set_scale(); // masukkan faktor kalibrasi jika sudah tahu
  scale.tare();      // set awal berat ke 0

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0); // posisi awal tertutup
}

// === FUNGSI BACA ULTRASONIK ===
float bacaJarakCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durasi = pulseIn(ECHO_PIN, HIGH);
  return durasi * 0.034 / 2;
}

// Logika Berulang
void loop() {
  float jarak = bacaJarakCM();

  if (jarak > 0 && jarak < jarakTrigger) {
    // Buka tutup
    servoMotor.write(90); // buka (derajat)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silakan buang");
    lcd.setCursor(0, 1);
    lcd.print("sampah...");
    delay(3000); // perkiraan menuaruh sampah (3 detik)

    // Baca berat
    float beratGram = scale.get_units(10); // ambil rata-rata 10 pembacaan (Kode hasil GPT)
    if (beratGram < 0) beratGram = 0;
    float beratKg = beratGram / 1000.0;
    float nilaiUang = beratKg * hargaPerKg;

    // Tampilan hasil
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Berat: ");
    lcd.print(beratKg, 2); // 2 angka dibelakang koma
    lcd.print(" kg");

    lcd.setCursor(0, 1);
    lcd.print("Uang: Rp");
    lcd.print((int)nilaiUang);

    delay(5000); // tampilkan selama 5 detik

    // 4. Tutup kembali
    servoMotor.write(0);
    delay(1000);
  }

  delay(200); // debounce delay
}
