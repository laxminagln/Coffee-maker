#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x27,20,4);

Servo cup_servo;
Servo spoon_servo;

const int alarm_hour = 6;
const int alarm_min = 10;
int pump_on_time = 13000;
long coil_on_time =230; //210;
int pos = 0;
bool a;
int hour_check = 0;
tmElements_t tm;
int coil_on_count;
int coil_on_display_count;
int coffee_switch = 50;
int coffee_switch_state =0; 
const int steps_per_revolution = 2048; 
Stepper stepper_coffee (steps_per_revolution, 23, 27, 25, 29);
Stepper stepper_coil (steps_per_revolution, 31, 35, 33, 37);
Stepper stepper_milk (steps_per_revolution, 39, 43, 41, 45);
Stepper stepper_sugar (steps_per_revolution, 47, 51, 49, 53);
const int coil_ir_pin = 34;
const int dispenser1_ir_pin =32;
const int dispenser2_ir_pin =22;
const int dispenser3_ir_pin =24;
const int spoon_ir_pin =30;
const int relay_shock = 36;
const int relay_belt = 38;
const int relay_pump = 40;
const int relay_spoon = 42;
const int relay_coil = 44;
int coil_ir_state =HIGH;
int dispenser1_ir_state =HIGH;
int dispenser2_ir_state =HIGH;
int dispenser3_ir_state =HIGH;
int spoon_ir_state =HIGH;

void setup() {
  cup_servo.attach(3);
  spoon_servo.attach(2);
pinMode(coffee_switch,INPUT);
  stepper_coil.setSpeed(13);
  stepper_coffee.setSpeed(13);
  stepper_sugar.setSpeed(13);
  stepper_milk.setSpeed(13);
pinMode(coil_ir_pin,INPUT);
pinMode(dispenser1_ir_pin,INPUT);
pinMode(dispenser2_ir_pin,INPUT);
pinMode(dispenser3_ir_pin,INPUT);
pinMode(spoon_ir_pin,INPUT);
pinMode(relay_shock,OUTPUT);
pinMode(relay_belt,OUTPUT);
pinMode(relay_pump,OUTPUT);
pinMode(relay_spoon,OUTPUT);
pinMode(relay_coil,OUTPUT);
digitalWrite(relay_shock,HIGH);
digitalWrite(relay_belt,HIGH);
digitalWrite(relay_pump,HIGH);
digitalWrite(relay_spoon,HIGH);
digitalWrite(relay_coil,LOW);
spoon_servo.write(120);
  lcd.begin (16,2);
  lcd.setBacklight(HIGH); 
  lcd.home();
  delay(1000);
  Serial.begin(9600);
}

void loop() {
   if (RTC.read(tm)) {
    lcd.clear();  
    lcd.print("Time  ");
    ampm(tm.Hour);       
    lcd.print(':');
    lcd.print(tm.Minute);
    lcd.print(':');
    lcd.print(tm.Second);
    if(a==true)
    lcd.print(" am");
    if(a==false)
    lcd.print(" pm");
    lcd.setCursor(0,1);
    lcd.print("coffee ");
    lcd.print(alarm_hour);
    lcd.print(':');
    lcd.print(alarm_min);
    lcd.print(" pm");
    delay(250);
    if(hour_check == alarm_hour && tm.Minute == alarm_min){
    lcd.home();  
    makeCoffee();
    }
   }
   coffee_switch_state = digitalRead(coffee_switch);
   Serial.println(coffee_switch_state);
   if(coffee_switch_state == HIGH){
    makeCoffee();
   }
}
void ampm(int Hour){  
if(Hour==0){          
  Hour=12;
  a=true;           
  lcd.print(Hour);
}
else if(Hour>0 && Hour<12){
  a=true;           
  lcd.print(Hour);
}
else if(Hour==12){
  a=false;          
  lcd.print(Hour);
}
else if(Hour>12){
  Hour=Hour-12;     
  a=false;
  lcd.print(Hour);
}
hour_check = Hour;
}
void makeCoffee(){
 coil_ir_state =HIGH;
 dispenser1_ir_state =HIGH;
 dispenser2_ir_state =HIGH;
 dispenser3_ir_state =HIGH;
 spoon_ir_state =HIGH;
  lcd.clear();
  lcd.print("Making coffee....");
  lcd.setCursor(0,1);
  lcd.print("Dispensing cup");
for (pos = 120; pos >= 38; pos -= 1) { 
    cup_servo.write(pos);             
    delay(15);
    }
for (pos = 38; pos <= 120; pos += 1) { 
    // in steps of 1 degree
    cup_servo.write(pos);             
    delay(15);                      
  }
for (pos = 120; pos >= 100; pos -= 1) { 
    cup_servo.write(pos);              
    delay(15);
    }
digitalWrite(relay_belt,LOW);

while (coil_ir_state == HIGH) {
 coil_ir_state = digitalRead(coil_ir_pin);
}
digitalWrite(relay_belt,HIGH);
digitalWrite(relay_pump,LOW);
lcd.setCursor(0,1);
lcd.print("Filling watter  ");
delay(pump_on_time);
digitalWrite(relay_pump,HIGH);
  lcd.setCursor(0,1);
  lcd.print("Coil down      ");
  stepper_coil.step(-4250);
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Coil ON       ");
  digitalWrite(relay_coil,HIGH);
 coffee_switch_state = digitalRead(coffee_switch);
while (coil_on_count <= coil_on_time) {
   coffee_switch_state = digitalRead(coffee_switch);
    if(coffee_switch_state == HIGH){
      coil_on_count = coil_on_time; 
    }
  delay(1000);
coil_on_display_count = coil_on_time - coil_on_count;
  lcd.setCursor(11,1);
  lcd.print(coil_on_display_count);
 coil_on_count++;
}
  digitalWrite(relay_coil,LOW);
  lcd.setCursor(0,1);
  lcd.print("Coil up        ");
  stepper_coil.step(4250);
  delay(500);
digitalWrite(relay_belt,LOW);
while (dispenser1_ir_state == HIGH) {
 dispenser1_ir_state = digitalRead(dispenser1_ir_pin);
}
digitalWrite(relay_belt,HIGH);
lcd.setCursor(0,1);
lcd.print("Disp coffee      ");
stepper_coffee.step(-steps_per_revolution*3);
digitalWrite(relay_belt,LOW);
while (dispenser2_ir_state == HIGH) {
 dispenser2_ir_state = digitalRead(dispenser2_ir_pin);
}
digitalWrite(relay_belt,HIGH);
lcd.setCursor(0,1);
lcd.print("Disp sugar       ");
stepper_sugar.step(-steps_per_revolution*4);
digitalWrite(relay_belt,LOW);
while (dispenser3_ir_state == HIGH) {
 dispenser3_ir_state = digitalRead(dispenser3_ir_pin);
}
digitalWrite(relay_belt,HIGH);
lcd.setCursor(0,1);
lcd.print("Disp milk        ");
stepper_milk.step(-steps_per_revolution*3.5);
  digitalWrite(relay_belt,LOW);
while (spoon_ir_state == HIGH) {
 spoon_ir_state = digitalRead(spoon_ir_pin);
}
digitalWrite(relay_belt,HIGH);
delay(500);
lcd.setCursor(0,1);
lcd.print("Stiring         ");
for (pos = 130; pos >= 48; pos -= 1) { 
    spoon_servo.write(pos);            
    delay(15);                       
  }
digitalWrite(relay_spoon,LOW);
delay(1000);
  for (pos = 48; pos <= 63; pos += 1) { 
    spoon_servo.write(pos);              
    delay(15);
  }
delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) { 
    spoon_servo.write(pos);              
    delay(15);                       
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) { 
    spoon_servo.write(pos);             
    delay(15);
  }
 delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) { 
    spoon_servo.write(pos);              
    delay(15);                       
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) { 
    // in steps of 1 degree
    spoon_servo.write(pos);              
    delay(15);
  }
delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) { 
    spoon_servo.write(pos);              
    delay(15);                       
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) { 
    spoon_servo.write(pos);              
    delay(15);
  }
   delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) {
    spoon_servo.write(pos);              
    delay(15);                       
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) { 
    spoon_servo.write(pos);              
    delay(15);
  }             
delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) { 
    spoon_servo.write(pos);              
    delay(15);                      
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) {
    spoon_servo.write(pos);             
    delay(15);
  }
  delay(1000);
  for (pos = 63; pos >= 48; pos -= 1) {
    spoon_servo.write(pos);              
    delay(15);                       
  }
delay(1000);
for (pos = 48; pos <= 63; pos += 1) { 
    spoon_servo.write(pos);             
    delay(15);
  }
digitalWrite(relay_spoon,HIGH);
delay(1000);

for (pos = 48; pos <= 130; pos += 1) { 
    spoon_servo.write(pos);              
    delay(15);                       
  }
digitalWrite(relay_belt,LOW);
delay(4000);
digitalWrite(relay_belt,HIGH);
digitalWrite(relay_shock,LOW);
 lcd.setCursor(0,1);
 lcd.print("Coffee ready    ");
 delay(4000);
 digitalWrite(relay_shock,HIGH);
}
