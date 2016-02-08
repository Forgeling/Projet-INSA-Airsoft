// set pin numbers:
const int buttonPin = 15;     // the number of the pushbutton pin

// variables will change:
int buttonState;         // variable for reading the pushbutton status

void setup()
{
  pinMode(buttonPin, INPUT);
  buttonState = digitalRead(buttonPin);
  Serial.begin(9600);
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  Serial.print(buttonState);
  delay(100);
}
