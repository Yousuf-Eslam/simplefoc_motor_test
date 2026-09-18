#include <Arduino.h>
#include <SimpleFOC.h>
//Hardware used 
// AS5600
// NEMA 17 Stepper motor
// 2 BTS7960 for each phase (A and B)
// 2 ACS712 for current sensing
// put function declarations here:
//  StepperMotor(int pp, (optional R, KV, Ld, Lq))
//  - pp  - pole pair number
//  - R   - phase resistance value [Ohm] - optional
//  - KV  - motor KV rating [rpm/V] - optional
//  - Ld  - d axis inductance value [H] - optional
//  - Lq  - q axis inductance value [H] - optional
StepperMotor motor = StepperMotor(50); //Check driver PWM count
// StepperDriver4PWM(ph1A, ph1B, ph2A, ph2B, (en1, en2 optional))
StepperDriver4PWM driver = StepperDriver4PWM(18,19,33,25,26,27); 

// encoder instances
Encoder encoder = Encoder(21,22,4096); //Check encoder pins
// channel A and B;
void doA() { encoder.handleA(); }
void doB() { encoder.handleB(); }


//instantiat the commander
Commander command = Commander(Serial);
void doMotor(char* cmd) { command.motor(&motor, cmd); }

void setup() {
Serial.begin(115200);

//Initialize Encoder
encoder.init();
encoder.enableInterrupts(doA, doB);
motor.linkSensor(&encoder);
//Initialize Driver
driver.voltage_power_supply = 27;
driver.voltage_limit = 27;
if(!driver.init()){
  Serial.println("Driver initialization failed");
return;
}
motor.linkDriver(&driver);


motor.voltage_sensor_align = 3; //Alligns motor with sensor 0 position



//Motion Control method
motor.torque_controller = TorqueControlType::voltage;
motor.controller = MotionControlType::velocity;
motor.useMonitoring(Serial);

//Initializing Motor
motor.init();
if (!motor.initFOC()){
  Serial.println("FOC init failed");
  return;
}


motor.target = TODO; //Volts
command.add('M', doMotor, "motor");
Serial.println(F("Motor ready."));
Serial.println(F("Set motor target with command: M <target>"));
}

void loop() {
  motor.loopFOC();
  motor.move();
  command.run();
}