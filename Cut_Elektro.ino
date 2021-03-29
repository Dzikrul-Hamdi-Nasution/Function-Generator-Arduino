#include <Wire.h> // Library komunikasi I2C 
#include <LiquidCrystal_I2C.h> // Library modul I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <EEPROM.h>
#include <AD9833.h>     // Include the library 
#define FNC_PIN 10       // Can be any digital IO pin 
AD9833 gen(FNC_PIN);       // Defaults to 25MHz internal reference frequency

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

const int tombol_down = 2;
const int tombol_up = 3;
const int relay = 4;
int frekuensi, memori, gelombang;

float voltage_V = 0, shuntVoltage_mV, busVoltage_V;
float current_mA = 0;
float power_mW = 0;
float energy_Wh = 0;
long time_s = 0;


void setup()
{
  Serial.begin(9600);
  lcd.backlight();
  lcd.init();
  lcd.setCursor(4, 0);
  lcd.print("Function");
  lcd.setCursor(3, 1);
  lcd.print("Generator");

  gen.Begin();
  gen.ApplySignal(SQUARE_WAVE, REG0, 350000);
  gen.EnableOutput(true);

  pinMode(relay, OUTPUT);
  pinMode(tombol_up, INPUT);
  pinMode(tombol_down, INPUT);
  pinMode (tombol_down, INPUT_PULLUP);
  pinMode (tombol_up, INPUT_PULLUP);
  digitalWrite(relay, HIGH);

  delay(3000);
  lcd.clear();
  frekuensi = 22000;

  uint32_t currentFrequency;
  ina219.begin();

}
void loop() {


  lcd.setCursor(0, 0);
  lcd.print("F : ");
  lcd.setCursor(4, 0);
  lcd.print(frekuensi);

  if (digitalRead(tombol_up) == LOW) {
    frekuensi = frekuensi + 50;
    EEPROM.write(0, frekuensi);
    if (digitalRead(tombol_down) == LOW) {
      gelombang = 1;
    }
    delay(100);
  }
  if (digitalRead(tombol_down) == LOW) {
    frekuensi = frekuensi - 50;
    EEPROM.write(0, frekuensi);
    if (digitalRead(tombol_up) == LOW) {
      gelombang = 2;
    }
    delay(100);
  }

  //memori = EEPROM.read(0);
  //frekuensi = memori;

  if (gelombang == 1) {
    getData();
    gen.ApplySignal(SQUARE_WAVE, REG0, frekuensi);
    lcd.setCursor(0, 1);
    lcd.print("PULSA");
    digitalWrite(relay, HIGH);
  }
  if (gelombang == 2) {
    getData();
    gen.ApplySignal(SINE_WAVE, REG0, frekuensi);
    lcd.setCursor(0, 1);
    lcd.print("SINUS");
    digitalWrite(relay, LOW);
  }

  

}


void getData() {

  time_s = millis() / (1000); // convert time to sec
  busVoltage_V = ina219.getBusVoltage_V();
  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  voltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
  current_mA = ina219.getCurrent_mA();
  //power_mW = ina219.getPower_mW();
  power_mW = current_mA * voltage_V;
  energy_Wh = (power_mW * time_s) / 3600; //energy in watt hour



  lcd.setCursor(7, 1);
  lcd.print(power_mW);
  lcd.print(" mW");
  
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(voltage_V); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.print("Energy:        "); Serial.print(energy_Wh); Serial.println(" mWh");
  Serial.println("----------------------------------------------------------------------------");
}
