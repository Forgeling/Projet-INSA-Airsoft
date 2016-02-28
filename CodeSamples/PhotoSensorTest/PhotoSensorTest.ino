#define PHOTOSENSOR_PIN 1
#define PHOTOSENSOR_THRESHOLD 300

bool isBoxOpen ()
{
  if(analogRead(PHOTOSENSOR_PIN) > PHOTOSENSOR_THRESHOLD) return true;
  else return false;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(PHOTOSENSOR_PIN));
  delay(500);
}
