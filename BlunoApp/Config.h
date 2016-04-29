#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------------
// Pin mapping
//------------------------------------------------------------------------------
// Photosensor
const int PHOTOSENSOR_PIN = 1; // Analog pin

// Key
const int KEY_PIN = 2;

// LCD Screen
const int LCD_RS_PIN = 8;
const int LCD_ENABLE_PIN = 9;
const int LCD_D4_PIN = 4;
const int LCD_D5_PIN = 5;
const int LCD_D6_PIN = 6;
const int LCD_D7_PIN = 7;

// Keypad
const int ROW_0 = 13;
const int ROW_1 = 12;
const int ROW_2 = 11;
const int ROW_3 = 3;
const int COL_0 = 16; // Analog pin
const int COL_1 = 17; // Analog pin
const int COL_2 = 18; // Analog pin
const int COL_3 = 19; // Analog pin

//------------------------------------------------------------------------------
// Hardware config
//------------------------------------------------------------------------------
// Photosensor
const int PHOTOSENSOR_THRESHOLD = 300;

// LCD Screen
const int LCD_LINE_LENGTH = 16;
const int LCD_NB_LINES = 2;

// Keypad
const byte NB_ROWS= 4;
const byte NB_COLS= 4;
const char keymap[NB_ROWS][NB_COLS]=
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[NB_ROWS] = {ROW_0,ROW_1,ROW_2,ROW_3};
byte colPins[NB_COLS]= {COL_0,COL_1,COL_2,COL_3};

//------------------------------------------------------------------------------
// Game Configuration constantss
//------------------------------------------------------------------------------
// Pad input types
const char CODE = 0;
const char NUMBER = 1;
const char DURATION = 2;

// Secret code to authorize the start of the configuration phase
const String CONFIG_CODE = "1337";

// Acceptable code lengths
const int MIN_CODE_LENGTH = 1;
const int MAX_CODE_LENGTH = LCD_LINE_LENGTH;
const int NB_DIGITS_FOR_CODE_LENGTH = 2;

// Game modes
const int ANTI_TERRO_A = (int) '1';
const int ANTI_TERRO_D = (int) '2';
const int CAPTURE = (int) '3';
const int DEFUSE = (int) '4';

// Game duration
const int MIN_GAME_DURATION = 1;
const int DURATION_DIGITS = 2;

// Key/Code choice
const int KEY_ONLY = (int) '1';
const int CODE_ONLY = (int) '2';
const int KEY_OR_CODE = (int) '3';
const int KEY_AND_CODE = (int) '4';

// Time for defusing
const int MIN_DEFUSE_DURATION = 1;

// Teams
const int MAX_NB_TEAMS = 8;
const int MIN_NB_TEAMS = 2;
const int NB_TEAMS_DIGITS = 1;

// Key deactivation message
const String KEY_DEACTIVATION = "KEY_DEACTIVATION";

// Endgame string
const String ENDGAME_STRING = "ENDGAME_STRING";

//------------------------------------------------------------------------------
// Displays
//------------------------------------------------------------------------------
// Config displays
const int ENTER_CODE_CONFIG_DISPLAY = 0;
const int MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY = 1;
const int WAITING_BLUETOOTH_CONFIG_DISPLAY = 2;
const int GAME_MODE_CONFIG_DISPLAY = 3;
const int GAME_DURATION_CONFIG_DISPLAY = 4;
const int KEY_CODE_CONFIG_DISPLAY = 5;
const int CODE_LENGTH_CONFIG_DISPLAY = 6;
const int CODE_DEFINITION_CONFIG_DISPLAY = 7;
const int TIME_FOR_DEFUSING_CONFIG_DISPLAY = 8;
const int NUMBER_OF_TEAMS_CONFIG_DISPLAY = 9;
const int RANDOM_CODE_GENERATION_DISPLAY = 10;
const int KEY_CONFIRMATION_CONFIG_DISPLAY = 11;

// Begin/end diplays
const int READY_TO_START_DISPLAY = 100;
const int NO_ONES_WIN_END_DISPLAY = 101;
const int TERROS_WIN_END_DISPLAY = 102;
const int ANTIS_WIN_END_DISPLAY = 103;
const int TEAM_NUMBER_WIN_END_DISPLAY = 104;

// Ingame displays
const int WAITING_FOR_CODE_INGAME_DISPLAY = 200;
const int WAITING_FOR_KEY_INGAME_DISPLAY = 201;
const int WAITING_FOR_EITHER_INGAME_DISPLAY = 202;
const int WAITING_FOR_TEAM_NUMBER_INGAME_DISPLAY = 203;
const int WAITING_FOR_CODE_ACTIVATION_INGAME_DISPLAY = 204;
const int WAITING_FOR_KEY_DEACTIVATION_INGAME_DISPLAY = 205;
const int WAITING_FOR_EITHER_ACTIVATION_INGAME_DISPLAY = 206;

// Errors
const int BOX_OPENED_ERROR_DISPLAY = 300;

#endif // CONFIG_H
