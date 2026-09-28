#include <SimpleFOC.h>

// Stepper motor instance (pole pairs = 50, phase resistance = 3.5 ohm, KV = 41.6)
StepperMotor motor = StepperMotor(25, 3.5, 41.6);

// Stepper driver instance (4 PWM + 2 Enable pins on ESP32)
StepperDriver4PWM driver = StepperDriver4PWM(18, 19, 33, 25, 26, 27);

// Magnetic sensor instance (AS5600 over I2C)
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

// Commander interface for SimpleFOCStudio
Commander command = Commander(Serial);
void doMotor(char* cmd) { command.motor(&motor, cmd); }

void setup() {
  Serial.begin(115200);

  // Initialize I2C and sensor
  Wire.begin(21, 22);
  sensor.init(&Wire);
  motor.linkSensor(&sensor);

  // Driver configuration
  driver.voltage_power_supply = 12;
  driver.voltage_limit = 12;
  driver.init();
  motor.linkDriver(&driver);

  // Choose FOC modulation
  motor.foc_modulation = FOCModulationType::SinePWM;

  // Safe sensor alignment voltage (prevents overheating 3.5 ohm motor)
  motor.voltage_sensor_align = 3.0;   

  // Control loop setup
  motor.torque_controller = TorqueControlType::voltage;
  
  motor.controller = MotionControlType::velocity;

  // Velocity PID Configuration (adjusted from 100 to a stable baseline)
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 2.0;
  motor.PID_velocity.D = 0.0;
  motor.LPF_velocity.Tf = 0.01;         // Low-pass filter time constant

  // Angle PID Configuration
  motor.P_angle.P = 20;

  // Motor Limits
  motor.voltage_limit = 10;             // [V]
  motor.velocity_limit = 50;            // [rad/s]

  // Enable telemetry monitoring
  motor.useMonitoring(Serial);
  motor.monitor_downsample = 10;        // Send telemetry every 10th loop iteration to prevent serial lag

  // Register motor with Commander under ID 'M'
  command.add('M',doMotor,"motor");
 motor.useMonitoring(Serial);
  motor.monitor_downsample = 0;
  // Initialize motor and execute alignment
  motor.init();
  motor.initFOC();

  // Set initial target velocity
  motor.target = 10;

  Serial.println(F("Motor initialized. Connect SimpleFOCStudio using ID 'M' at 115200 baud."));
  _delay(1000);
}

void loop() {
  // 1. High-frequency FOC core calculation
  motor.loopFOC();

  // 2. Motion controller (computes target velocity/voltage)
  motor.move();

  // 3. Send real-time plot variables to SimpleFOCStudio
  motor.monitor();

  // 4. Parse incoming serial commands from SimpleFOCStudio
  command.run();
}