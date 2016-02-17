#include <LiquidCrystal.h>
#include <Keypad.h>

//------------------------------------------------------------------------------------------
// LCD Configuration and methods
//------------------------------------------------------------------------------------------

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
 
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
void read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
 
    // my buttons when read are centered at these values: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result
 
    if (adc_key_in > 1000) lcd_key = btnNONE; 
 
    // For V1.1 us this threshold
    if (adc_key_in < 50)   lcd_key = btnRIGHT;  
    if (adc_key_in < 250)  lcd_key = btnUP; 
    if (adc_key_in < 450)  lcd_key = btnDOWN; 
    if (adc_key_in < 650)  lcd_key = btnLEFT; 
    if (adc_key_in < 850)  lcd_key = btnSELECT;
    
   // depending on which button was pushed, we perform an action
   switch (lcd_key){
 
       case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            lcd.print("RIGHT ");
            break;
       }
       case btnLEFT:{
            lcd.print("LEFT  "); //  push button "LEFT" and show the word on the screen
            break;
       }    
       case btnUP:{
            lcd.print("UP    ");  //  push button "UP" and show the word on the screen
            break;
       }
       case btnDOWN:{
            lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
            break;
       }
       case btnSELECT:{
            lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
            break;
       }
       case btnNONE:{
            lcd.print("NONE  ");  //  No action  will show "None" on the screen
            break;
       }
   }
}

//------------------------------------------------------------------------------------------
// Bluetooth Configuration and methods
//------------------------------------------------------------------------------------------
String command = "";

//------------------------------------------------------------------------------------------
// Key Configuration and methods
//------------------------------------------------------------------------------------------
// set key pin:
const int KEY_PIN = 15;

// variable for reading the key status
int buttonState;

void readKeyState ()
{
  buttonState = digitalRead(KEY_PIN);
}

//------------------------------------------------------------------------------------------
// Pad Configuration and methods
//------------------------------------------------------------------------------------------
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {13,12,11,3}; //Rows 0 to 3
byte colPins[numCols]= {16,17,18,19}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

void readPad ()
{
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
  {
  Serial.print(keypressed);
  }
}


void setup(){
   Serial.begin(115200);  //initial the Serial (for Bluetooth connection)
   
   pinMode(KEY_PIN, INPUT); // Set the key pin as input
   
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("Push the buttons");  // print a simple message on the LCD
}
  
void loop(){
   // Read the strings that are sent via Bluetooth
   if (Serial.available())  {
    command = Serial.readString();
   }
   read_LCD_buttons();
}
