#include <Arduino.h>
#include <SimpleFOC.h>
//Hardware used 
// AS5600
// NEMA 17 Stepper motor
// 2 BTS7960 for each phase (A and B)
// 2 ACS712 for current sensing
// put function declarations here:
StepperMotor motor = StepperMotor(); //Check driver PWM count
StepperDriver4PWM driver = StepperDriver4PWM(TODO,TODO,TODO,TODO); //Check driver PWM count

// encoder instance
Encoder encoder = Encoder(TODO,TODO); //Check encoder pins
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
driver.voltage_limit = TODO;
if(!driver.init()){
  Serial.println("Driver initialization failed");
return;
}
motor.linkDriver(&driver);




motor.voltage_limit = TODO;

//Motion Control method
motor.torque_controller = TorqueControlType::voltage;
motor.controller = MotionControlType::torque;
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