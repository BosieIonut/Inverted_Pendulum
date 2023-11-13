#include <Wire.h>
#define ENCA_1 34 // YELLOW
#define ENCB_1 32 // WHITE
#define ENCA_2 35 // YELLOW
#define ENCB_2 17 // WHITE
#define ENM1 33
#define RMA1 25
#define RMB1 26
#define ENM2 12
#define RMA2 14
#define RMB2 27
uint32_t LoopTimer = 0;
volatile int posi_1 = 0,posi_2 = 0;
int pos_1,pos_2;
float RateRoll, RatePitch, RateYaw;
float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;
int RateCalibrationNumber;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
float KalmanAngleRoll = 0, KalmanUncertaintyAngleRoll = 2 * 2;
float KalmanAnglePitch = 0, KalmanUncertaintyAnglePitch = 2 * 2;
float Kalman1DOutput[] = { 0, 0 };
void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput, float KalmanMeasurement) {
  KalmanState = KalmanState + 0.004 * KalmanInput;
  KalmanUncertainty = KalmanUncertainty + 0.004 * 0.004 * 4 * 4;
  float KalmanGain = KalmanUncertainty * 1 / (1 * KalmanUncertainty + 3 * 3);
  KalmanState = KalmanState + KalmanGain * (KalmanMeasurement - KalmanState);
  KalmanUncertainty = (1 - KalmanGain) * KalmanUncertainty;
  Kalman1DOutput[0] = KalmanState;
  Kalman1DOutput[1] = KalmanUncertainty;
}
void gyro_signals(void) {
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  int16_t GyroX = Wire.read() << 8 | Wire.read();
  int16_t GyroY = Wire.read() << 8 | Wire.read();
  int16_t GyroZ = Wire.read() << 8 | Wire.read();
  RateRoll = (float)GyroX / 65.5;
  RatePitch = (float)GyroY / 65.5;
  RateYaw = (float)GyroZ / 65.5;
  AccX = (float)AccXLSB / 4096 + 0.04;
  AccY = (float)AccYLSB / 4096 - 0.01;
  AccZ = (float)AccZLSB / 4096 + 0.04;
  AngleRoll = atan(AccY / sqrt(AccX * AccX + AccZ * AccZ)) * 1 / (3.142 / 180);
  AnglePitch = -atan(AccX / sqrt(AccY * AccY + AccZ * AccZ)) * 1 / (3.142 / 180);
}




void setup() {
  Serial.begin(57600);
  pinMode(ENCA_1,INPUT);
  pinMode(ENCB_1,INPUT);
  pinMode(ENCA_2,INPUT);
  pinMode(ENCB_2,INPUT);
  pinMode(ENM1,OUTPUT);
  pinMode(RMA1,OUTPUT);
  pinMode(RMB1,OUTPUT);
  pinMode(ENM2,OUTPUT);
  pinMode(RMA2,OUTPUT);
  pinMode(RMB2,OUTPUT);
  setMotor(0, 200, ENM1, RMA1, RMB1);
  setMotor(0, 200, ENM2, RMA2, RMB2);
  attachInterrupt(digitalPinToInterrupt(ENCA_1),readEncoder1,RISING);
  attachInterrupt(digitalPinToInterrupt(ENCA_2),readEncoder2,RISING);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Wire.setClock(400000);
  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++) {
    gyro_signals();
    RateCalibrationRoll += RateRoll;
    RateCalibrationPitch += RatePitch;
    RateCalibrationYaw += RateYaw;
    delay(1);
  }
  RateCalibrationRoll /= 2000;
  RateCalibrationPitch /= 2000;
  RateCalibrationYaw /= 2000;
  LoopTimer = micros();
}

#define Ki 180
#define Kp 44
#define Kd 6
#define dt 0.01
float e,r,u,last_e,cum_e=0; // referinta eroare comanda
float y;
float i=0,i1 = 0 ,i2 = 0;
void calculeaza_comanda()
{
    cum_e += e * dt; // integrala erorii
    u  = e * Kp + cum_e * Ki + Kd * (e - last_e) / dt; // comanda
    u = u - 0.6 * pos_1;
    last_e = e;
}



void loop() {

  noInterrupts(); // disable interrupts temporarily while reading
  pos_1 = posi_1;
  pos_2 = posi_2;
  interrupts();
  gyro_signals();
  RateRoll -= RateCalibrationRoll;
  RateYaw -= RateCalibrationYaw;
  kalman_1d(KalmanAngleRoll, KalmanUncertaintyAngleRoll, RateRoll, AngleRoll);
  KalmanAngleRoll = Kalman1DOutput[0];
  KalmanUncertaintyAngleRoll = Kalman1DOutput[1];
  Serial.print(float(pos_1) * 360.0 / 234.3);
  Serial.print("\t");
  Serial.print(float(pos_2) * 360.0 / 234.3);
  Serial.print("\t");
  Serial.print("Roll Angle [°] ");
  Serial.println(KalmanAngleRoll);
  e = KalmanAngleRoll;
  calculeaza_comanda();
  if(u > 0)
  {
    setMotor(-1, int(u),ENM1,RMA1,RMB1);
    setMotor(1, int(u),ENM2,RMA2,RMB2);
  }
  else
  {
    setMotor(1, int(-u),ENM1,RMA1,RMB1);
    setMotor(-1, int(-u),ENM2,RMA2,RMB2);
  }
  while (micros() - LoopTimer < 10000);
  LoopTimer = micros();
}
void readEncoder1(){
  int b1 = digitalRead(ENCB_1);
  if(b1 > 0){
    posi_1++;
  }
  else{
    posi_1--;
  }
}
void readEncoder2(){
  int b2 = digitalRead(ENCB_2);
  if(b2 > 0){
    posi_2++;
  }
  else{
    posi_2--;
  }
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(pwm,pwmVal + 20);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}
