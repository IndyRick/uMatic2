/*
 * The purpose of the sketch is to replicate the features of the HeathKit µMatic Memory Keyer SA-5010.
 * The "fully working" prototype version was completed on 8 AUG 2019 by WW9JD using code of his own design,
 * along with the open source code of others.  It is hoped that a QST article will result from this effort,
 * along with upload to GitHub where the code will be extended by others.
 * 
 * The QST article was published in the April, 2020, edition of QST. 
 * 
 * The "<F>" symbol is used to suggest where new Features might be developed/implemented in the future
 * 
 * Fritzing file "completed" 20 AUG 2019 
 * https://github.com/IndyRick/uMatic2
 * Current Sketch "D" - 5 DEC 2020 WW9JD 
 * On 25 OCT 2020 WW9JD added transmitting the Current Sketch "letter code" for am audible version verification.
 *   This only sounds when the keyer is reset, to avoid it being annoying.
 * First PCB design (Ver 1) completed 24 AUG 2019
 * Second PCB design (Ver 1a) completed 11 SEP 2019
 * Third PCB design (Ver 1b) completed 30 SEP 2019 & first fabricated 23 MAR 2020
 * Current PCB design (Ver 1c) completed 4 APR 2020
 * 
 * If you make modifications to this file, please include the date, your call, and collaboration notes in 1, or more, line comments
 *   e.g. case 'Q' : sendCW("33132"); break; //19 MAR 2020 WW9JD corrected error discovered by N4TVC
*/

#include "Nextion.h"   //Nextion TFT LCD screen support
#include "uMatic2.h"    //Nextion interface settings for µMatic Memory Keyer 
#include <EEPROM.h>    //Library to use (or simulate the use of) EEPROM memory for non-volitile storage of values
#include <SD.h>        //Library to make use of microSD card reader/writer
#include <toneAC.h>    //Library to enable speaker volume control
#include <avr/sleep.h> //Library that will permit the keyer to go to sleep and reduce power consumption from about 140mA to about 45mA
//<F>#include <LowPower.h>  //Library to enable low-power operation

// PINS
const int pinKeyStraight = 2; //TRS plug TIP
int pinKeyDit = 3; //TRS plug TIP
int pinKeyDah = 4; //TRS plug RING
const int pinPTT = 6; //TRS plug RING //201114 WW9JD Discovered XMIT & PTT were swapped
const int pinXMIT = 5; //TRS plug TIP
const int pinOnboardLed = LED_BUILTIN;
const int testSD = 35; //This permits SD card breakout board test (SDtest) without hooking up Nextion display
const int SDcrd = 53;

File myFile;

// KEYER MODE
const int Iambic = 0;
const int Iambic_Rev = 1;
const int Bug = 2;
const int Single = 3; //19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
const int Single_Rev = 4;//


// DEFAULT STATE with EEPROM Override <<<<<<<<<<<<<<<
int toneFreq = 600;  //Sidetone Hz
int toneVol = 2;  //Sidetone volume - scale 1-10
float k_WPM = 20; //Manual keyer speed - 1-99 WPM
float f_WPM = 20; //Farnsworth speed when sending files
bool Weighting = false; //Change the standard 1:3 dit:dah ratio
int wt = 5; //Weight - "ratio" of dit to dah 0-9
float rpt = 0; //Repeat - Number of times to repeat the sending of a file
float T_Const = 100; //Timing Constant - To account for timing changes between different hardware configurations
int Mode = Iambic;
int PTTwait = 5; //Delay in ms between closing the PTT and Sending lines
bool vMode = 0;//Contest Mode                                 //5 DEC 2020 WW9JD Added for Contest Mode
int test1 = 1; //A Serial Number that can be used in contests //
int test2 = 1; //A Serial Number that can be used in contests //
int test3 = 1; //A Serial Number that can be used in contests //
int test4 = 1; //A Serial Number that can be used in contests //
int test5 = 1; //A Serial Number that can be used in contests //
int test6 = 1; //A Serial Number that can be used in contests //
int testN = 1; //Serial Number in use                         //
int Zero = 0;  //Serial number leading zeros                  //
int qMode = 0; //Calliing CQ or Hunt & Pounce                 //
bool sTest = 0; //Sent current serial number status           //
bool unMute = 1; // 24 APR 2020 WW9JD Sidetone is ON (MUTE saves state through resets & power cycles)
bool BacklightOnly = 0;  // 30 MAY 2020 WW9JD - Settings saves/sets Backlight checkbox (false means 2560 sleeps, too)
int abMode = 0; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
int CL = 6; //Call Length - Length of EEPROM memory when using call signs of fixed length
char call[] = "KK9ABC"; //Default call sign (not allocated to any Ham as of Aug 2019)


// Setup EEPROM storage
#define E_INIT 1023  
int EEloc = 0;

/*
 * Object structure used to save/recall values to/from EEPROM
 */
struct myObject{
int e_toneFreq;
int e_toneVol;
float e_k_WPM;
float e_f_WPM;
bool e_Weighting;
float e_wt;
float e_rpt;
float e_T_Const;
int e_Mode;
int e_PTTwait;
bool e_vMode;//5 DEC 2020 WW9JD Contest Mode
int e_test1; //5 DEC 2020 WW9JD Contest Mode serial number storage
int e_test2; //
int e_test3; //
int e_test4; //
int e_test5; //
int e_test6; //
int e_testN; //5 DEC 2020 WW9JD Contest Mode active serial number bank
int e_Zero; //5 DEC 2020 WW9JD Contest Mode leading zeros for serial number
int e_qMode; //5 DEC 2020 WW9JD Contest Mose Calling CQ or Hunt & Pounce 
bool e_sTest; //1JAN 2021 WW9JD Contest Mode Sent Contest Serial #
bool e_unMute;  // 24 APR 2020 WW9JD - MUTE saves state through resets & power cycles
bool e_BacklightOnly;  // 30 MAY 2020 WW9JD - Settings saves/sets Backlight checkbox
int e_abMode; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
};


/*
 * Initial variable assignments
 */
int addr = 0;
bool setUp = false;
bool Stop = false;
bool Pause = false;
bool ps = false; // The switch to send a combination of letters as a prosign
bool cmnt = false; // The switch to indicate that the following text is a comment in a .TXT file 
bool pracState = false; //Practice State is OFF (keyer will key the transmitter)
bool PRACon = false;
bool LOADon = false;
bool PAUSEon = false;
bool SENDon = false;
bool NORMon = false;
int numi=0;
String numb;
String keyBuff = "";
String pauseBuff = "";
bool delaySend = true;
bool tuneState = false;
bool firstPush = true;
bool endFile = false;
bool WPMSliderTap = false;
String SendHold; 
float dit;
float k_const=T_Const/100; //Adjustment needed for timing variance introduced by keyer hardware
float f_element; //(Farnsworth) Character delay time
float f_wt; //Decimal weight factor from a lookup table
float ratio;
float holdTouchtime;
bool touched = false;
bool dispTxt = true; // 1 NOV 2020 WW9JD - Saves state of transmitted text display (hide text so that Farnsworth text speed is accurate)
bool dispTxtHold; //
int abModeHold; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
bool squeeze=false; //
char call2[20]; //5 DEC 2020 WW9JD 2nd call sign for Contest Mode
uint32_t holdI = 1; //5 DEC 2020 WW9JD Added for Contest Mode serial numbers
int testI = 1; //


/*
 * ===========================================================================================
 * Sketch Setup function
 * ===========================================================================================
 */

void setup() {
  setUp = true;
  String hold;
  // You will need to change NexConfig.h file in your ITEADLIB_Arduino_Nextion folder
  // Set the baud rate which is for debug (dbSerial) and communication with Nextion screen (NexSerial or Serial1)
  nexInit();  //Startup the Nextion display
  loadSettings(); //Loads keyer settings from EEPROM/default
  hold = "[N/A]";
  hold.toCharArray(call2, 30); //30 characters are used to store the call sign
  

//PIN setup
  pinMode(pinKeyStraight, INPUT_PULLUP); //PIN 2
  pinMode(pinKeyDit, INPUT_PULLUP); //PIN 3
  pinMode(pinKeyDah, INPUT_PULLUP); //PIN 4
  pinMode(pinOnboardLed, OUTPUT); //PIN LED_BUILTIN
  pinMode(pinPTT, OUTPUT); //PIN 5
  pinMode(pinXMIT, OUTPUT); //PIN 6
  pinMode(testSD, INPUT_PULLUP); //PIN 35
  pinMode(SDcrd, OUTPUT); //PIN 53
  digitalWrite(50, HIGH); // These are the PINS for the µSD card breakout board
  digitalWrite(51, HIGH); // I was having trouble with the LED on that board being lit when I put
  digitalWrite(52, HIGH); // the keyer to sleep.  In the Arduino Forum, cpinamtz suggested setting
  digitalWrite(53, HIGH); // these PINS high to help with power consumption, and it worked here, too.

  
  // Register the Nextion push/pop event callback function of the components
  AutoSleep.attachPush(AutoSleepPushCallback, &AutoSleep);
  TUNE.attachPush(TUNEPushCallback, &TUNE);
  MUTE.attachPush(MUTEPushCallback, &MUTE);
  RANGE.attachPush(RANGEPushCallback, &RANGE);
  RANGE.attachPop(RANGEPopCallback, &RANGE);
  WPM.attachPush(WPMPushCallback, &WPM);
  PRAC.attachPush(PRACPushCallback, &PRAC);
  RPT.attachPush(RPTPushCallback, &RPT);
  WT.attachPush(WTPushCallback, &WT);
  PC.attachPush(PCPushCallback, &PC);
  LOAD.attachPush(LOADPushCallback, &LOAD);
  SPACING.attachPush(SPACINGPushCallback, &SPACING);
  SEND.attachPush(SENDPushCallback, &SEND);
  STOP.attachPush(STOPPushCallback, &STOP);
  Backlight.attachPush(BacklightPushCallback, &Backlight);
  BLText.attachPush(BLTextPushCallback, &BLText);
  SLEEP0.attachPush(SLEEP0PushCallback, &SLEEP0);
  SLEEP1.attachPush(SLEEP1PushCallback, &SLEEP1);
  SLEEP30.attachPush(SLEEP30PushCallback, &SLEEP30);
  SLEEP2.attachPush(SLEEP2PushCallback, &SLEEP2);
  SLEEP5.attachPush(SLEEP5PushCallback, &SLEEP5);
  SLEEP10.attachPush(SLEEP10PushCallback, &SLEEP10);
  RPT2.attachPush(RPT2PushCallback, &RPT2);
  PRAC2.attachPush(PRAC2PushCallback, &PRAC2);
  PCb.attachPush(PCbPushCallback, &PCb);
  LOAD2.attachPush(LOAD2PushCallback, &LOAD2);
  SEND2.attachPush(SEND2PushCallback, &SEND2);
  STOP2.attachPush(STOP2PushCallback, &STOP2);
  bSerial.attachPush(bSerialPushCallback, &bSerial);
  EnterText.attachPush(EnterTextPushCallback, &EnterText);
  EnterText4.attachPush(EnterText4PushCallback, &EnterText4);
  WPMSlider.attachPop(WPMSliderPopCallback, &WPMSlider);
  Splash.attachPush(SplashPushCallback, &Splash);
  SplashPic.attachPush(SplashPicPushCallback, &SplashPic);
  mode_slider.attachPop(mode_sliderPopCallback, &mode_slider);
  PitchSlider.attachPop(PitchSliderPopCallback, &PitchSlider);
  VolSlider.attachPop(VolSliderPopCallback, &VolSlider);
  T_Slider.attachPop(T_SliderPopCallback, &T_Slider);
  PTTSlider.attachPop(PTTSliderPopCallback, &PTTSlider);
  resetKeyer.attachPush(resetKeyerPushCallback, &resetKeyer);
  txtDsp.attachPush(txtDspPushCallback, &txtDsp);
  iMode.attachPush(iModePushCallback, &iMode);
  resetI.attachPush(resetIPushCallback, &resetI);
  bZero.attachPush(bZeroPushCallback, &bZero);
  bqMode.attachPush(bqModePushCallback, &bqMode);
  r0.attachPush(r0PushCallback, &r0);
  r1.attachPush(r1PushCallback, &r1);
  r2.attachPush(r2PushCallback, &r2);
  r3.attachPush(r3PushCallback, &r3);
  r4.attachPush(r4PushCallback, &r4);
  r5.attachPush(r5PushCallback, &r5);
  First_1.attachPush(First_1PushCallback, &First_1);
  First_2.attachPush(First_2PushCallback, &First_2);
  First_3.attachPush(First_3PushCallback, &First_3);
  First_4.attachPush(First_4PushCallback, &First_4);
  First_5.attachPush(First_5PushCallback, &First_5);
  Home_1.attachPush(Home_1PushCallback, &Home_1);
  Home_2.attachPush(Home_2PushCallback, &Home_2);
  Home_3.attachPush(Home_3PushCallback, &Home_3);
  Home_4.attachPush(Home_4PushCallback, &Home_4);
  Home_5.attachPush(Home_5PushCallback, &Home_5);
  qwerty_1.attachPush(qwerty_1PushCallback, &qwerty_1);
  qwerty_2.attachPush(qwerty_2PushCallback, &qwerty_2);
  qwerty_3.attachPush(qwerty_3PushCallback, &qwerty_3);
  qwerty_4.attachPush(qwerty_4PushCallback, &qwerty_4);
  qwerty_5.attachPush(qwerty_5PushCallback, &qwerty_5);
  Keyer_1.attachPush(Keyer_1PushCallback, &Keyer_1);
  Keyer_2.attachPush(Keyer_2PushCallback, &Keyer_2);
  Keyer_3.attachPush(Keyer_3PushCallback, &Keyer_3);
  Keyer_4.attachPush(Keyer_4PushCallback, &Keyer_4);
  Keyer_5.attachPush(Keyer_5PushCallback, &Keyer_5);
  Settings_1.attachPush(Settings_1PushCallback, &Settings_1);
  Settings_2.attachPush(Settings_2PushCallback, &Settings_2);
  Settings_3.attachPush(Settings_3PushCallback, &Settings_3);
  Settings_4.attachPush(Settings_4PushCallback, &Settings_4);
  Settings_5.attachPush(Settings_5PushCallback, &Settings_5);
  Refs_1.attachPush(Refs_1PushCallback, &Refs_1);
  Refs_2.attachPush(Refs_2PushCallback, &Refs_2);
  Refs_3.attachPush(Refs_3PushCallback, &Refs_3);
  Refs_4.attachPush(Refs_4PushCallback, &Refs_4);
  Refs_5.attachPush(Refs_5PushCallback, &Refs_5);
  bsplash.attachPush(bsplashPushCallback, &bsplash);
  callText.attachPush(callTextPushCallback, &callText);
  callText4.attachPush(callText4PushCallback, &callText4);
  bref.attachPush(brefPushCallback, &bref);
  bendFile.attachPush(bendFilePushCallback, &bendFile);
  nMode.attachPush(nModePushCallback, &nMode);
  _0.attachPush(_0PushCallback, &_0);
  _1.attachPush(_1PushCallback, &_1);
  _2.attachPush(_2PushCallback, &_2);
  _3.attachPush(_3PushCallback, &_3);
  _4.attachPush(_4PushCallback, &_4);
  _5.attachPush(_5PushCallback, &_5);
  _6.attachPush(_6PushCallback, &_6);
  _7.attachPush(_7PushCallback, &_7);
  _8.attachPush(_8PushCallback, &_8);
  _9.attachPush(_9PushCallback, &_9);
  _Z.attachPush(_ZPushCallback, &_Z);
  BX.attachPush(BXPushCallback, &BX);
  BC.attachPush(BCPushCallback, &BC);
  _V.attachPush(_VPushCallback, &_V);
  BB.attachPush(BBPushCallback, &BB);
  BN.attachPush(BNPushCallback, &BN);
  _M.attachPush(_MPushCallback, &_M);
  COM.attachPush(COMPushCallback, &COM);
  PER.attachPush(PERPushCallback, &PER);
  BAR.attachPush(BARPushCallback, &BAR);
  _A.attachPush(_APushCallback, &_A);
  BS.attachPush(BSPushCallback, &BS);
  _D.attachPush(_DPushCallback, &_D);
  _F.attachPush(_FPushCallback, &_F);
  _G.attachPush(_GPushCallback, &_G);
  _H.attachPush(_HPushCallback, &_H);
  _J.attachPush(_JPushCallback, &_J);
  _K.attachPush(_KPushCallback, &_K);
  BL.attachPush(BLPushCallback, &BL);
  _QM.attachPush(_QMPushCallback, &_QM);
  _Q.attachPush(_QPushCallback, &_Q);
  _W.attachPush(_WPushCallback, &_W);
  _E.attachPush(_EPushCallback, &_E);
  _R.attachPush(_RPushCallback, &_R);
  _T.attachPush(_TPushCallback, &_T);
  _Y.attachPush(_YPushCallback, &_Y);
  BU.attachPush(BUPushCallback, &BU);
  _I.attachPush(_IPushCallback, &_I);
  _O.attachPush(_OPushCallback, &_O);
  BP.attachPush(BPPushCallback, &BP);
  b_1.attachPush(b_1PushCallback, &b_1);
  b_2.attachPush(b_2PushCallback, &b_2);
  b_3.attachPush(b_3PushCallback, &b_3);
  b_4.attachPush(b_4PushCallback, &b_4);
  b_5.attachPush(b_5PushCallback, &b_5);
  b_6.attachPush(b_6PushCallback, &b_6);
  b_7.attachPush(b_7PushCallback, &b_7);
  b_8.attachPush(b_8PushCallback, &b_8);
  b_9.attachPush(b_9PushCallback, &b_9);
  b_0.attachPush(b_0PushCallback, &b_0);
  error.attachPush(errorPushCallback, &error);
  WORD.attachPush(WORDPushCallback, &WORD);
  BT.attachPush(BTPushCallback, &BT);
  b0.attachPush(b0PushCallback, &b0);
  b1.attachPush(b1PushCallback, &b1);
  b2.attachPush(b2PushCallback, &b2);
  b3.attachPush(b3PushCallback, &b3);
  b4.attachPush(b4PushCallback, &b4);
  b5.attachPush(b5PushCallback, &b5);
  b6.attachPush(b6PushCallback, &b6);
  b7.attachPush(b7PushCallback, &b7);
  b8.attachPush(b8PushCallback, &b8);
  b9.attachPush(b9PushCallback, &b9);
  b10.attachPush(b10PushCallback, &b10);
  b11.attachPush(b11PushCallback, &b11);
  b12.attachPush(b12PushCallback, &b12);
  b13.attachPush(b13PushCallback, &b13);
  b14.attachPush(b14PushCallback, &b14);
  b15.attachPush(b15PushCallback, &b15);
  b16.attachPush(b16PushCallback, &b16);
  b17.attachPush(b17PushCallback, &b17);
  b18.attachPush(b18PushCallback, &b18);
  b19.attachPush(b19PushCallback, &b19);
  b20.attachPush(b20PushCallback, &b20);
  b21.attachPush(b21PushCallback, &b21);
  b22.attachPush(b22PushCallback, &b22);
  b23.attachPush(b23PushCallback, &b23);
  b24.attachPush(b24PushCallback, &b24);
  b25.attachPush(b25PushCallback, &b25);
  b26.attachPush(b26PushCallback, &b26);
  b27.attachPush(b27PushCallback, &b27);
  b28.attachPush(b28PushCallback, &b28);
  b29.attachPush(b29PushCallback, &b29);

  //Display the Main screen automatically after 5 sec
  myDelay(4000);
  sendCommand("page Main");
  Serial1.flush();
  SplashPushCallback(&Splash);

  //Set dit length
  k_const=T_Const/100;
  dit = (1200/k_WPM)*k_const; 
  /*
   * Special thanks to SCPhillips.com for his Farnsworth formula.  
   * 
   * The formula provided by Jon Bloom, KE3Z: f_element = (((60000*k_WPM)-(37200*f_WPM)*k_const)/(k_WPM*f_WPM))
   * didn't work for me.  I plan to look into why it didn't work sometime in the future.
   */
  k_const=T_Const/100;
  f_element = (((300*k_WPM)-(186*f_WPM))*k_const/(95*k_WPM*f_WPM))*1000; //(Farnsworth) Character delay time

  //Setup is done - send an invitation to transmit ("K") after setting PRACTICE mode 
  PRACPushCallback(&PRAC);
  hold = String(call);
  keyBuff = "";
  keyBuff = "K[Ready]  ";
  if (hold == "KK9ABC"){
    keyBuff = keyBuff + "D[Sketch version 5 DEC 2020 WW9JD]   "; //25 OCT 2020 WW9JD Transmits sketch version info on RESET
    keyBuff = keyBuff + "[To set your callsign - 1.Tap QWERTY key 2.Tap LOAD 3.Enter your call with the green keys 4.Tap END FILE]";
  }
  while (keyBuff != ""){
    loop(); //Gives the sketch the chance to send text
  }
  STOPPushCallback(&STOP);
  setUp = false;
}

/*
 * ===========================================================================================
 * Sketch Loop function
 * ===========================================================================================
 */

void loop() {
  
  /*
   * When a pop or push event occurred every time,
   * the corresponding component[right page id and component id] in touch event list will be asked.
   */
  nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
  
  if (!tuneState) listenKey();
  if (tuneState) tx_and_sidetone_key(1);
  sendTouch(); //23 OCT 2020 WW9JD

  if (keyBuff != "") {
    ledNORMoff();
    ledSENDon();
    nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
    ReadText();
  }
  if ((keyBuff = "")&&(SENDon)) {
    ledSENDoff();
    ledNORMon();
  }

  if (digitalRead(testSD)==LOW) {
    SDtest();
  }
  // <F>LowPower.powerDown(SLEEP_4S,ADC_OFF,BOD_OFF);
}

/*
 * ===========================================================================================
 * EEPROM functions
 * ===========================================================================================
 */

/*
 * Get variable settings from EEPROM on program startup. 
 * If this is the very first startup, or first after reset, then load the default values.
 */
void loadSettings() {
  myObject keyerSettings;
  if (EEPROM.read(E_INIT) == 'T'){
  }else{
    saveSettings();
    EEPROM.put(E_INIT, 'T'); //If the letter "T" is stored at EEPROM location 1023, then this is NOT the first start
  } 
  EEPROM.get(1, keyerSettings); 
  toneFreq = keyerSettings.e_toneFreq;
  toneVol = keyerSettings.e_toneVol;
  k_WPM = keyerSettings.e_k_WPM;
  f_WPM = keyerSettings.e_f_WPM;
  Weighting = keyerSettings.e_Weighting;
  wt = keyerSettings.e_wt;
  rpt = keyerSettings.e_rpt;
  T_Const = keyerSettings.e_T_Const;
  Mode = keyerSettings.e_Mode;
  PTTwait = keyerSettings.e_PTTwait;
  vMode = keyerSettings.e_vMode;
  test1 = keyerSettings.e_test1;
  test2 = keyerSettings.e_test2;
  test3 = keyerSettings.e_test3;
  test4 = keyerSettings.e_test4;
  test5 = keyerSettings.e_test5;
  test6 = keyerSettings.e_test6;
  testN = keyerSettings.e_testN;
  Zero = keyerSettings.e_Zero;
  qMode = keyerSettings.e_qMode;
  sTest = keyerSettings.e_sTest;
  unMute = keyerSettings.e_unMute;  // 24 APR 2020 WW9JD - MUTE saves state through resets & power cycles
  BacklightOnly = keyerSettings.e_BacklightOnly; // 30 MAY 2020 WW9JD - Settings sets Backlight state 
  abMode = keyerSettings.e_abMode; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
  EEPROM.get(200, call);
  switch (testN){
    case 1: testI = test1; break;                     
    case 2: testI = test2; break;                     
    case 3: testI = test3; break;                     
    case 4: testI = test4; break;                     
    case 5: testI = test5; break;                     
    case 6: testI = test6; break;                     
  }
}

/*
 * Save variable settings to EEPROM
 */
void saveSettings() {
  switch (testN){
    case 1: test1 = testI; break;                     
    case 2: test2 = testI; break;                     
    case 3: test3 = testI; break;                     
    case 4: test4 = testI; break;                     
    case 5: test5 = testI; break;                     
    case 6: test6 = testI; break;                     
  }
  myObject keyerSettings;
  keyerSettings.e_toneFreq = toneFreq;
  keyerSettings.e_toneVol = toneVol;
  keyerSettings.e_k_WPM = k_WPM;
  keyerSettings.e_f_WPM = f_WPM;
  keyerSettings.e_Weighting = Weighting;
  keyerSettings.e_wt = wt;
  keyerSettings.e_rpt = rpt;
  keyerSettings.e_T_Const = T_Const;
  keyerSettings.e_Mode = Mode;
  keyerSettings.e_PTTwait = PTTwait;
  keyerSettings.e_vMode = vMode;
  keyerSettings.e_test1 = test1;
  keyerSettings.e_test2 = test2;
  keyerSettings.e_test3 = test3;
  keyerSettings.e_test4 = test4;
  keyerSettings.e_test5 = test5;
  keyerSettings.e_test6 = test6;
  keyerSettings.e_testN = testN;
  keyerSettings.e_Zero = Zero;
  keyerSettings.e_qMode = qMode;
  keyerSettings.e_sTest = sTest;
  keyerSettings.e_unMute = unMute;  // 24 APR 2020 WW9JD - MUTE saves state through resets & power cycles
  keyerSettings.e_BacklightOnly = BacklightOnly; // 30 MAY 2020 WW9JD - Settings saves Backlight state 
  keyerSettings.e_abMode = abMode; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
  EEPROM.put(1, keyerSettings); 
  EEPROM.put(200, call);
}


/*
 * ===========================================================================================
 * Core sketch functions
 * ===========================================================================================
 */

/*
 * This function causes a dit or dah to be sent, depending on the keyer mode.
 */
void listenKey(){
  if (digitalRead(pinKeyStraight)==LOW) {
    //sendCommand("repo thsp,1");//Just a Nextion command that can be executed quickly for Nextion serial wake (commented out 23 OCT 2020 WW9JD)
    //Serial1.flush();//Adds a serial communication with the Nextion display to wake the display, if asleep (commented out 23 OCT 2020 WW9JD)
  touched = true; //24 OCT 2020 WW9JD
  if ((SENDon) && (!PAUSEon)) {
    keyBuff = "|" + keyBuff ;
  }
    add_straight();
  }
  if (digitalRead(pinKeyDit)==LOW){
    //sendCommand("repo thsp,1");//Just a Nextion command that can be executed quickly for Nextion serial wake (commented out 23 OCT 2020 WW9JD)
    //Serial1.flush();//Adds a serial communication with the Nextion display to wake the display, if asleep (commented out 23 OCT 2020 WW9JD)
  touched = true; //24 OCT 2020 WW9JD
  if ((SENDon) && (!PAUSEon)) {
    keyBuff = "|" + keyBuff ;
  }
    switch(Mode){
      case 0 :  add_dit(); break;
      case 1 :  add_dah(); break;
      case 2 :  if (!digitalRead(pinKeyDah)==LOW){
                add_dit();
                break;
                }else{
                  break;
                }//19 NOV 2020 WW9JD - Fixed Bug behavior error (would add dits with dah key pressed)
      case 3 :  if (!digitalRead(pinKeyDah)==LOW){
                add_dit(); 
                break;
                }else{
                  break; 
                }//19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
      case 4 :  if (!digitalRead(pinKeyDah)==LOW){
                add_dah(); 
                break;
                }else{ 
                  break;
                }//19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
    }
  }
  if (digitalRead(pinKeyDah)==LOW){
    //sendCommand("repo thsp,1");//Just a Nextion command that can be executed quickly for Nextion serial wake (commented out 23 OCT 2020 WW9JD)
    //Serial1.flush();//Adds a serial communication with the Nextion display to wake the display, if asleep (commented out 23 OCT 2020 WW9JD)
  touched = true; //24 OCT 2020 WW9JD
  if ((SENDon) && (!PAUSEon)) {
    keyBuff = "|" + keyBuff ;
  }
    switch(Mode){
      case 0 :  add_dah(); break;
      case 1 :  add_dit(); break;
      case 2 :  add_straight(); break;
      case 3 :  if (!digitalRead(pinKeyDit)==LOW){
                add_dah(); 
                break;
                }else{ 
                  break;
                }//19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
      case 4 :  if (!digitalRead(pinKeyDit)==LOW){
                add_dit(); 
                break;
                }else{
                  break; 
                }//19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
    }
  }
}

/*
 * 24 OCT 2020 WW9JD
 * This function sends a "touch" to the Nextion display in order to keep it out of timed sleep. 
 */
void sendTouch(){
    if ((touched) && (millis() - holdTouchtime > 25)) {
    sendCommand("repo thsp,1");//Just a Nextion command that can be executed quickly for Nextion serial wake (commented out 23 OCT 2020 WW9JD)
    Serial1.flush();//Adds a serial communication with the Nextion display to wake the display, if asleep (commented out 23 OCT 2020 WW9JD)
    holdTouchtime = millis();
    touched = false;
    }
}
  

/*
 * The functions for adding character & word spaces.
 */
void add_word(){
    delay(f_element*6); //mute 2020 WW9JD changed factor from 7 to 6 because each element already has a closing dit space
}
void add_char(){
    delay(f_element*2); //24 APR 2020 WW9JD changed factor from 3 to 2 because each element already has a closing dit space
}

/*
 * This is the function where a dah is added to the sending stream.
 * The choice of where to place dit sensing has an effect on the feel of the keyer.
 * Dit sensing can be placed in any or all of the 4 locations.  At one point during development,
 * dit sensing was present in locations 1, 2, & 3, at the same time.  That made it a little
 * too easy to insert a dit.
 * <F>It would be relatively straight forward to add checkboxes to enable/disable sensing locations.
 */
void add_dah(){
  //Serial.println("dah"); //leave this line and commented out to remind what debug println looks like
  bool dit_pressed = false;
  float w_dit;
  float w_dah;
  tx_and_sidetone_key(1);
  //dit sensing location 1
  if (Weighting){
    w_dah = 3+f_wt;
    delay(dit*w_dah);
  }else{
    delay(dit*3);
  }
  //dit sensing location 2
  if (((Mode == 0) && (digitalRead(pinKeyDit)==LOW)) || ((Mode == 1) && (digitalRead(pinKeyDah)==LOW))){
    dit_pressed = true;
  }
  tx_and_sidetone_key(0);
  //dit sensing location 3
  if (Weighting){
    w_dit = 1+f_wt;
    delay((2*dit)-(dit*w_dit));
  }else{
    delay(dit);
  }
  if (dit_pressed){
     squeeze = true; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
     add_dit();
    }
  if ((abMode)&&(squeeze)){
     squeeze = false;
     xtra_dit(); 
    }// 19 NOV 2020 WW9JD - Added to support Iambic Mode B
}

// 19 NOV 2020 WW9JD - Added to support Iambic Mode B
void xtra_dah(){
  float w_dit;
  float w_dah;
  tx_and_sidetone_key(1);
  if (Weighting){
    w_dah = 3+f_wt;
    delay(dit*w_dah);
  }else{
    delay(dit*3);
  }
  tx_and_sidetone_key(0);
  if (Weighting){
    w_dit = 1+f_wt;
    delay((2*dit)-(dit*w_dit));
  }else{
    delay(dit);
  }
}

/*
 * This is where a dit is added to the sending stream.
 * While the location for dah sensing has never seemed to be a problem, it could, of course, be adjusted.
 */
void add_dit(){
  float w_dit;
  tx_and_sidetone_key(1);
  if (Weighting){
    w_dit = 1+f_wt;
    delay(dit*w_dit);
  }else{
    delay(dit);
  }
  tx_and_sidetone_key(0);
  if (Weighting){
    delay((2*dit)-(dit*w_dit));
  }else{
    delay(dit);
  }
  if (((Mode == 0) && (digitalRead(pinKeyDah)==LOW)) || ((Mode == 1) && (digitalRead(pinKeyDit)==LOW))){
     squeeze=true; // 19 NOV 2020 WW9JD - Added to support Iambic Mode B
     add_dah();
    }
  if ((abMode)&&(squeeze)){
     squeeze=false;
     xtra_dah();
    }// 19 NOV 2020 WW9JD - Added to support Iambic Mode B
}


// 19 NOV 2020 WW9JD - Added to support Iambic Mode B
void xtra_dit(){
  float w_dit;
  tx_and_sidetone_key(1);
  if (Weighting){
    w_dit = 1+f_wt;
    delay(dit*w_dit);
  }else{
    delay(dit);
  }
  tx_and_sidetone_key(0);
  if (Weighting){
    delay((2*dit)-(dit*w_dit));
  }else{
    delay(dit);
  }
  if (((Mode == 0) && (digitalRead(pinKeyDah)==LOW)) || ((Mode == 1) && (digitalRead(pinKeyDit)==LOW))){
    if (abMode){
     xtra_dah();
    }else{
     add_dah();
    }
  }
}


/*
 * For the straight key, the keyer just closes the PTT and key lines as long that the key is depressed
 */
void add_straight(){
    while ((digitalRead(pinKeyStraight)==LOW) || ((digitalRead(pinKeyDah)==LOW) && (!digitalRead(pinKeyDit)==LOW))){
      tx_and_sidetone_key(1);
    }
    tx_and_sidetone_key(0);
}


/*
 * Function to call the Transmit function and turn on the sidetone, depending of the Mute state of the keyer
 */
void tx_and_sidetone_key(int ti){
    if(ti==1){
      if ((unMute)||(setUp)) toneAC(toneFreq, toneVol); // 24 APR 2020 WW9JD - (||(setUp)) MUTE saves state through resets & power cycles
      XMIT(1);
      }
    if(ti==0){
      toneAC(0);
      XMIT(0);
      tuneState = false;
      }
}


/*
 * This function works through 2 optoisolators to key the PTT and XMTR KEY lines.  The PTTwait delay is
 * user-settable in case the keyer is used with equipment that needs the PTT line closed
 * for some period of time before the KEY line.
 */
void XMIT(int tx){
  if (!pracState){
  if(tx==1){
    digitalWrite(pinPTT,HIGH);
    delay(PTTwait);
    digitalWrite(pinXMIT,HIGH);
    digitalWrite(pinOnboardLed,HIGH);
    }else{
    digitalWrite(pinXMIT,LOW);
    delay(PTTwait);
    digitalWrite(pinPTT,LOW);
    digitalWrite(pinOnboardLed,LOW);
    }
  }
}

/*
 * Many folks think that the use of the Arduino Delay() function is poor programming.  I saw the issue in this
 * sketch a few times, so I created this function to permit screen taps to interrupt myDelay().
 */
void myDelay(long DelayTime){
  long StartTime = millis();
  while((millis()-StartTime) < DelayTime) {
    nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
  }
}


/*
 * When ReadText is called, the value of 'txt' is parsed and each character is sent as CW or a command
 */
void ReadText(){
  String txt;
  String testNum;
  String holdTest;
  char cw[8];
  char MsgHold[1000];
  while ((keyBuff != "") && (!LOADon)){
    nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
    listenKey();
    while (PAUSEon){
          nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
          listenKey();
    }
    //for each character of the string, find the corresponding Morse Code sequence and call sendCW() to send it
    qwertyDisp(keyBuff); //Send the rest of the text buffer to the screen for display
    txt = keyBuff[0]; //Grab the next character
    keyBuff.remove(0,1); //Remove that character from the start of the buffer
    txt.toCharArray(cw,8);
    switch(cw[0]) {
      case ' ' : sendCW("4"); break;
      case 'a' : sendCW("132"); break;
      case 'b' : sendCW("31112"); break;
      case 'c' : sendCW("31312"); break;
      case 'd' : sendCW("3112"); break;
      case 'e' : sendCW("12"); break;
      case 'f' : sendCW("11312"); break;
      case 'g' : sendCW("3312"); break;
      case 'h' : sendCW("11112"); break;
      case 'i' : sendCW("112"); break;
      case 'j' : sendCW("13332"); break;
      case 'k' : sendCW("3132"); break;
      case 'l' : sendCW("13112"); break;
      case 'm' : sendCW("332"); break;
      case 'n' : sendCW("312"); break;
      case 'o' : sendCW("3332"); break;
      case 'p' : sendCW("13312"); break;
      case 'q' : sendCW("33132"); break;
      case 'r' : sendCW("1312"); break;
      case 's' : sendCW("1112"); break;
      case 't' : sendCW("32"); break;
      case 'u' : sendCW("1132"); break;
      case 'v' : sendCW("11132"); break;
      case 'w' : sendCW("1332"); break;
      case 'x' : sendCW("31132"); break;
      case 'y' : sendCW("31332"); break;
      case 'z' : sendCW("33112"); break;
      case 'A' : sendCW("132"); break;
      case 'B' : sendCW("31112"); break;
      case 'C' : sendCW("31312"); break;
      case 'D' : sendCW("3112"); break;
      case 'E' : sendCW("12"); break;
      case 'F' : sendCW("11312"); break;
      case 'G' : sendCW("3312"); break;
      case 'H' : sendCW("11112"); break;
      case 'I' : sendCW("112"); break;
      case 'J' : sendCW("13332"); break;
      case 'K' : sendCW("3132"); break;
      case 'L' : sendCW("13112"); break;
      case 'M' : sendCW("332"); break;
      case 'N' : sendCW("312"); break;
      case 'O' : sendCW("3332"); break;
      case 'P' : sendCW("13312"); break;
      case 'Q' : sendCW("33132"); break; //19 MAR 2020 WW9JD corrected error discovered by N4TVC
      case 'R' : sendCW("1312"); break;
      case 'S' : sendCW("1112"); break;
      case 'T' : sendCW("32"); break;
      case 'U' : sendCW("1132"); break;
      case 'V' : sendCW("11132"); break;
      case 'W' : sendCW("1332"); break;
      case 'X' : sendCW("31132"); break;
      case 'Y' : sendCW("31332"); break;
      case 'Z' : sendCW("33112"); break;
      case '0' : sendCW("333332"); break;
      case '1' : sendCW("133332"); break;
      case '2' : sendCW("113332"); break;
      case '3' : sendCW("111332"); break;
      case '4' : sendCW("111132"); break;
      case '5' : sendCW("111112"); break;
      case '6' : sendCW("311112"); break;
      case '7' : sendCW("331112"); break;
      case '8' : sendCW("333112"); break;
      case '9' : sendCW("333312"); break;
      case '.' : sendCW("1313132"); break;
      case ',' : sendCW("3311332"); break;
      case '/' : sendCW("311312"); break;
      case '?' : sendCW("1133112"); break;
      case '-' : sendCW("3111132"); break;
      case '=' : sendCW("311132"); break; //BT
      case ':' : sendCW("3331112"); break;
      case ';' : sendCW("3131312"); break;
      case '\'' : sendCW("1333312"); break; //apostrophe - the "\" is needed so the sketch won't consider it a program command
      case '"' : sendCW("1311312"); break;
      case '!' : sendCW("3131332"); break;
      case '(' : sendCW("313312"); break;
      case ')' : sendCW("3133132"); break;
      case '@' : sendCW("1331312"); break;
      case '%' : sendCW("111111112"); break; //error
      case '*' : keyBuff = call + keyBuff; break; //station call sign
      case '^' : keyBuff = call2 + keyBuff; break; //5 DEC 2020 WW9JD Contest Mode contact call sign
      case '<' : ps = true; break; //Turn on PROSIGN mode (eliminates character/word spaces)
      case '>' : ps = false; break; //Turn off PROSIGN mode
      case '[' : cmnt = true; break; //Turn on COMMENT mode (permits comments in the .TXT files)
      case ']' : cmnt = false; break; //Turn off COMMENT mode
      case '|' : PCPushCallback(&PC); break; // The "|" character does not have an English CW sequence, so it is used to insert a PAUSE
      case '#' : {switch (Zero){
                    case 0: holdTest = String(testI); break;
                    case 1: {
                      if (testI < 10){
                        holdTest = "0" + String(testI);
                        break; 
                      }
                      holdTest = String(testI); 
                      break;
                    }
                    case 2: {
                      if (testI < 10){
                        holdTest = "00" + String(testI); 
                        break; 
                      }
                      if (testI < 100){
                        holdTest = "0" + String(testI); 
                        break; 
                      }
                      holdTest = String(testI); 
                      break;
                    }
                    case 3: {
                      if (testI < 10){
                        holdTest = "000" + String(testI); 
                        break; 
                      }
                      if (testI < 100){
                        holdTest = "00" + String(testI); 
                        break; 
                      }
                      if (testI < 1000){
                        holdTest = "0" + String(testI); 
                        break; 
                      }
                      holdTest = String(testI); 
                      break;
                    }
                    case 4: {
                      if (testI < 10){
                        holdTest = "0000" + String(testI); 
                        break; 
                      }
                      if (testI < 100){
                        holdTest = "000" + String(testI); 
                        break; 
                      }
                      if (testI < 1000){
                        holdTest = "00" + String(testI); 
                        break; 
                      }
                      if (testI < 10000){
                        holdTest = "0" + String(testI); 
                        break; 
                      }
                      holdTest = String(testI); 
                      break;
                    }
                    case 5: {
                      if (testI < 10){
                        holdTest = "00000" + String(testI); 
                        break; 
                      }
                      if (testI < 100){
                        holdTest = "0000" + String(testI); 
                        break; 
                      }
                      if (testI < 1000){
                        holdTest = "000" + String(testI); 
                        break; 
                      }
                      if (testI < 10000){
                        holdTest = "00" + String(testI); 
                        break; 
                      }
                      if (testI < 100000){
                        holdTest = "0" + String(testI); 
                        break; 
                      }
                      holdTest = String(testI); 
                      break;
                    }
                  }
                  keyBuff = holdTest + keyBuff; 
                  sTest = true;
                  SerialFont();
                  break;
                  saveSettings();
                  loadSettings();
                  }
    }
  }
      SendHold = "";
      qwertyDisp("");
}

/*
 * This function parses the character codes generated above and calls the appropriate sending function
 */
void sendCW(String holdCW){
  int ll;
  ll = holdCW.length()-1;
  if (ps) ll = ll-1; //If in PROSIGN mode, the length variable (ll) is shortened so the character space is not included.
  for (int iii = 0; iii <= ll; iii++) {
    nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
    listenKey();
    if (!cmnt){
    switch(holdCW[iii]){
      case '1' : add_dit(); break;
      case '2' : add_char(); break;
      case '3' : add_dah(); break;
      case '4' : add_word(); break;
      }
    }
  }
}


/*
 * ===========================================================================================
 * Main page functions
 * ===========================================================================================
 */

/*
 * Button TUNE component push callback function. 
 * When the TUNE button is touched, the transmitter is keyed and the sidetone sounds. 
 * Tap any other Main page button to stop TUNE.
 */
void TUNEPushCallback(void *ptr) {
    tuneState = true;
    tx_and_sidetone_key(1);
    ledNORMdim();
    numi=0;
    numb="";
    //<F>Start Tune timer (settable delay - 5 sec initial default)
    //<F>Send high speed callsign at end of Tune time by default (may be defeated)
}

/*
 * MUTE button push callback function. 
 * When the MUTE button is touched, the mute features toggles between on & off. 
 * The chosen state is saved in EEPROM, and restored on the next startup //24 APR 2020 WW9JD
 * MUTE silences only the sidetone.
 * The keyer Notification Sounds BEEPs will still be heard.
 */
void MUTEPushCallback(void *ptr) {
    tx_and_sidetone_key(0);
  if (unMute == false){
    unMute = true;
  }else{
    unMute = false;
    }
    if (!unMute){
      MUTE.setText("UNMUTE");
    }else{
      MUTE.setText("MUTE");
    }
    saveSettings();
    beep();
  }

/*
 * PRAC button push callback function. 
 * On the original µMatic Memory Keyer, the Practice mode sent repeatable, random text in CW. 
 * This version does not (currently) have a random text generator.  Instead, any number of practice files can be
 * created and played for practice.  The original, published text files are available in the same
 * Repository as the other files for this project.  Any text can be copied to a .TXT file and used for practice.
 * 
 * What the PRAC key does on the Arduino µMatic2 Memory Keyer is merely to toggle a "Practice State".  
 * If the PRAC LED is on, then the sidetone sounds, but the transmitter is not keyed.  You can use the paddles, straight
 * key, or QWERTY keyboard, and you will not transmit anything.  Tap PRAC or STOP to exit the Practice State.  
 * If Practice State is initiated before SENDing a file, the conclusion of SENDing a file will also exit the Practice State.
 */
void PRACPushCallback(void *ptr) {
    if(tuneState){
      STOPPushCallback(&STOP);
      return;
    }
    if (NORMon){
      ledNORMoff();
      ledPRACon();
      pracState = true;
    }else{
      pracState = false;
      STOPPushCallback(&STOP);
    }
    saveSettings();
}
 
/*
 * SPACING button push callback function. 
 * This sets the Farnsworth speed at which files are sent. It has no effect on hand-sent code
 */
void SPACINGPushCallback(void *ptr) {
    if (!tuneState){
        f_WPM = numb.toInt(); //24 APR 2020 WW9JD Permitted Farnsworth WPM to be set faster that Keyer WPM because some ops prefer shorter spacing
      if (numi == 0){
        f_WPM=k_WPM;
      }
    }
    /*
     * Special thanks to SCPhillips.com for his Farnsworth formula.  
     * 
     * The formula provided by Jon Bloom, KE3Z: f_element = (((60000*k_WPM)-(37200*f_WPM)*k_const)/(k_WPM*f_WPM))
     * didn't work during sketch design.  The reason why it didn't work is a topic for future research.
     */
    k_const=T_Const/100;
    f_element = (((300*k_WPM)-(186*f_WPM))*k_const/(95*k_WPM*f_WPM))*1000; //(Farnsworth) Character delay time
    if (f_element >= 0){
       saveSettings();
       n4.setValue(f_WPM);
    }else{
       warble(); //24 APR 2020 WW9JD Added check to make sure that f_element is greater than 0 before saving f_WPM and f_element
    }
    STOPPushCallback(&STOP);
}

/*
 * RPT button push callback function. 
 * This button will take the keyer out of TUNE mode, and set the number of times a message file is sent.
 * The purpose of this button in the original keyer was to repeat CW phrases to save memory space.
 * Since the original PCD5101P CMOS memory ICs only stored a total of 240 characters/commands, the 
 * RPT key helped to expand the amount of CW you could store/send.  The Arduino µMatic2 uses a microSD card
 * for storage with the capability of storing up to 6,400,000 pages of text on an 8GB card, so the RPT key
 * isn't needed for just phrases.  In concert with P/C, it will allow you to repeat the sending of a file
 * for a given number of times.  For example, you could easily load your contest CQ and set it to repeat 99
 * times. then, for the next 99 CQs you'd only have to tap the P/C key to send a CQ.
 */
void RPTPushCallback(void *ptr) {
  if (PAUSEon) {
    keyBuff = ""; //If PAUSEd, tapping RPT causes keyBuff to be emptied, which in turn causes the
    MsgDisp("");  // current Repeat loop to end (if myFile < 100 characters), and then next one to start.
    qwertyDisp("");
  }else
  if (!tuneState){
                  beep();
    switch(numi){
      case 0 :  {
                rpt = 0; 
                numb="";
                numi=0;
                n5.setValue(rpt);
                n0q.setValue(rpt);
                saveSettings();
                DisplayText("off");
                break;
                }
      case 1 : { 
                rpt = numb.toInt();
                numb="";
                numi=0;
                n5.setValue(rpt);
                n0q.setValue(rpt);
                saveSettings();
                DisplayText("off");
                break;
                }
      case 2 : {
                rpt = numb.toInt();
                numb="";
                numi=0;
                n5.setValue(rpt);
                n0q.setValue(rpt);
                saveSettings();
                DisplayText("off");
                break;
                }
     default :  {
                warble();    
                DisplayText("off");
                }
    }
  }else{
    STOPPushCallback(&STOP);
  }
}
 
/*
 * WT button push callback function. 
 * 
 * SETTING CHARACTER WEIGHT  (From the original Heathkit manual...)
 * The normal ratio of dot/dash/element space is 1:3:1. You can alter this ratio by using the WT
 * key. When you press a single digit key n , followed by the WT key, it sets the timing as follows:
 * 1. If you press WT with no preceding digit, the normal timing ratio shown above is used. This
 * means that the length of a dash is three times as long as that of a dot.
 * 2. If n (your specified digit) is between 0 and 4, inclusive, the interelement space is increased in
 * length, and the dot and dash lengths are correspondingly decreased. This results in a "lighter"
 * weighting with digit "0" being the lightest. With "0", the length of a dash is 4.4 times that of a dot.
 * 3. When you select a digit between 5 and 9, inclusive, the interelement space is decreased in 
 * length and the dot and the dash length are correspondingly increased. This results in a "heavier"
 * weighting with digit 9 being the heaviest. With 9, the length of a dash is 2.4 times that of a dot.
 * 
 * Denis Bolshoiden's Android app, Sound Oscilloscope, helped a great deal in "reverse engineering" the 
 * method the Heathkit engineers used to set weighting in the SA-5010.  It is unlikely that the method
 * used here is the same, but it does result in ratios that are very close to what they should be.
 * Also, the waveforms from the two keyers look very much alike.
 * 
 * NOTE: Please understand that weighting is included in this keyer first for historical accuracy, but
 * also in the belief that the 1st Amendment guarantee of free speech also protects poorly weighted CW.
 * Please don't use this feature without cause.
 */
void WTPushCallback(void *ptr) {
  if (!tuneState){
    if(numi == 1) {
      Weighting = true;
      wt = numb.toInt();
      n3.setValue(wt);
      n3.Set_font_color_pco(0); //WT font color = black
      switch (wt){
        case 0 : f_wt = -0.4; break;
        case 1 : f_wt = -0.3; break;
        case 2 : f_wt = -0.2; break;
        case 3 : f_wt = -0.1; break;
        case 4 : f_wt = -0.05; break;
        case 5 : f_wt = 0.05; break;
        case 6 : f_wt = 0.1; break;
        case 7 : f_wt = 0.2; break;
        case 8 : f_wt = 0.3; break;
        case 9 : f_wt = 0.4; break;
      }
    }
    if(numi == 0) {
      Weighting = false;
      n3.Set_font_color_pco(65504); //WT font color = yellow (same as background)
    }
    if(numi > 1) warble();
    saveSettings();
  }
    STOPPushCallback(&STOP);
}
  
/*
 * P/C button push callback function. 
 * When the P/C (Pause/Continue) button is touched, the file currently being sent is paused. 
 * Tap it again to resume sending.  You can also invoke this feature by including the vertical bar "|"
 * in the text of a file.
 */
void PCPushCallback(void *ptr) {
    if (!PAUSEon){
      if (NORMon){
        warble();
        //STOPPushCallback(&STOP);
      }
      if (((PRACon) || (SENDon)) && (keyBuff != "")){
      ledPAUSEon();
      }
    }else{
      ledPAUSEoff();
      beep();
    }
}
  
/*
 * LOAD button push callback function. 
 * 
 * The LOAD button has two uses - to store the operators call sign, and to store the text of a message. 
 * 1. To store a call sign (up to 30 characters in length), 
 *    tap LOAD without first tapping any number keys and enter the call sign using the QWERTY keys
 * 2. To store a message (no practical length limit), tap 1 or 2 number keys followed by the LOAD key
 * 
 * Tap END FILE when done with either.
 */
void LOAD2PushCallback(void *ptr) {
    dispTxtHold = dispTxt; // 1 NOV 2020 WW9JD
    dispTxt = true; // 1 NOV 2020 WW9JD - Display text if hidden so that entered text is displayed
    if (tuneState){
      warble();
      STOPPushCallback(&STOP);
      return;
    }
    if(numi == 0){
      if (vMode == 0){
      SendHold = "Enter Callsign";
      }else{  
      SendHold = "Enter Contact Callsign";
      }
      qwertyDisp(SendHold);
      //myDelay(1000);
      beep();
    }
    File myFile;
    SendHold="";
    String buffHold;
    int startI = 0;
    int endI = 0;
    char MsgHold[30];
    tx_and_sidetone_key(0);
    ledNORMoff();
    ledLOADon();
    if(numi > 0){
       numb = numb+".txt";
       if (!SD.begin(SDcrd)) {
          // if the card didn't initialize, print an error:
          warble();
          SendHold = "Error initializing SD card. ";
          MsgDisp(SendHold);
          qwertyDisp(SendHold);
          myDelay(5000);
        }
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        if (SD.exists(numb)) SD.remove(numb);
        myFile = SD.open(numb, FILE_WRITE);
       
        // if the file opened okay, write to it:
        if (myFile) {
          while (!endFile){
            if (keyBuff != ""){
              nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
              myFile.print(keyBuff);
              buffHold = buffHold + keyBuff;
              SendHold = buffHold;
              endI = buffHold.length();
              startI = endI - 18;
              if (startI < 0) startI = 0;
              SendHold = buffHold.substring(startI, endI);
              qwertyDisp(SendHold);
              if (keyBuff == "sd qsl k") endFile = true; //Used to test if SD card breakout board is properly installed without using the Nextion display
              keyBuff = "";
            }
            nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
        }
          myFile.println();
        // close the file:
          myFile.close();
          qwertyDisp("");
          
        } else {
          // if the file didn't open, print an error:
          warble();
          SendHold = "Error writing to " + numb;
          MsgDisp(SendHold);
          qwertyDisp(SendHold);
          myDelay(5000);
        }
      }else if (numi == 0){
          while (!endFile){
            if (keyBuff != ""){
              nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
              buffHold = buffHold + keyBuff;
              SendHold = buffHold;
              endI = buffHold.length();
              startI = endI - 18;
              if (startI < 0) startI = 0;
              SendHold = buffHold.substring(startI, endI);
              qwertyDisp(SendHold);
              keyBuff = "";
            }
            nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
          }
          SendHold.toUpperCase();
          if (vMode == 0){
            SendHold.toCharArray(call, 30); //30 characters are used to store the call sign
            saveSettings();
            qwertyDisp("");
            callText4.setText(call); //Updates the call display on the QWERTY page
          }else{
            SendHold.toCharArray(call2, 30); //30 characters are used to store the call sign
            qwertyDisp("");
            callText4.setText(call2); //Updates the call display on the QWERTY page (5 DEC 2020 WW9JD with contact call sign)
          }
          }          
    dispTxt = dispTxtHold; // 1 NOV 2020 WW9JD
    STOPPushCallback(&STOP);
    return;
}
  
/*
 * SEND button push callback function. 
 * To SEND a file, tap 1 or 2 black number keys (the file name) followed by the SEND key 
 */
void SENDPushCallback(void *ptr) {
  if (tuneState){
    warble();
    STOPPushCallback(&STOP);  
    return;
  }
  if (!tuneState){
    if(numi == 0){
      /*warble();
      STOPPushCallback(&STOP);
      */
      //b16PushCallback(&b16); //22 MAR 2020 WW9JD feature suggested by N8DUY (unknowingly - during a phone conversation)
                             /* Tim (N8DUY) commented to me that he had nearly worn out the SEND key on his SA-5010 
                              *  from sending his call so often.  I never realized that the SA-5010 sent memory 0 by
                              *  default if no number keys were pressed before SEND.  As the commented lines show, 
                              *  I had originally programmed that to make the ERROR sound and STOP.
                              */
      keyBuff = keyBuff + "*";//5 DEC 2020 WW9JD Modified above so that "call" would always be sent, even in Contest Mode
      return;
    }
    String numbHold;
    String SendHold1;
    numbHold = numb;
    int numiHold;
    numiHold = numi;
    rpt++;
    while (rpt > 0){
      File myFile;
      String sdFile;
      SendHold="";
      keyBuff = " ";
      char ReadHold;
      char MsgHold[1000];
      ledNORMoff();
      if (!pracState){
        tx_and_sidetone_key(0);
        ledSENDon();
        }
        if(numiHold > 0){
           sdFile = numbHold+".txt";
           /*
            * For SD.begin to run repeatedly, the line:
            * if (root.isOpen()) root.close();      // allows repeated calls
            * MUST be added just before 
            * return card.init(SPI_HALF_SPEED, csPin) &&
            * in the library file SD.cpp
            */
           if (!SD.begin(SDcrd)) { 
              warble();
              SendHold = "Error initializing SD card.";
              MsgDisp(SendHold);
              qwertyDisp(SendHold);
              myDelay(5000);
              return;
            }
            // open the file. note that only one file can be open at a time,
            // so you have to close this one before opening another.
            myFile = SD.open(sdFile);
            // if the file opened okay, read from it:
              if (myFile) {
                  SendHold1 = "Sending file " + sdFile;
                  MsgDisp(SendHold1);
                  dispTxtHold = dispTxt; // 1 NOV 2020 WW9JD
                  dispTxt = true; // 1 NOV 2020 WW9JD - Display text if hidden so that file contents is displayed
                  qwertyDisp(SendHold1);
                  DisplayText(" ");
                    dispTxt = dispTxtHold; // 1 NOV 2020 WW9JD
                while (myFile.available()) {
                    nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
                    ReadHold = myFile.read();
                    keyBuff = keyBuff + String(ReadHold);
                    //This WHILE section runs when keyBuff is 100 characters or more so that a long file will not swamp the memory
                    //while ((keyBuff.length() > 100) || (myFile.read() == 0)){
                    while (keyBuff.length() > 100){
                      ReadText();
                      nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
                      if (NORMon) return; //If you tap STOP during the loop, this will exit out of reading the file
                      }
                    if (!myFile.available()){
                        while (keyBuff != ""){
                              ReadText();
                              nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
                              if (NORMon) return; //If you tap STOP during the loop, this will exit out of reading the file
                      }
                    }
                    
                    }
                }else {
                  // if the file didn't open, print an error:
                  warble();
                  SendHold = "Error opening " + sdFile;
                  MsgDisp(SendHold);
                  qwertyDisp(SendHold);
                  myDelay(5000); //Displays the error message for 5 sec
                  STOPPushCallback(&STOP);
                  return;
                }
              // close the file:
              myFile.close();
              nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
              if (rpt == 99){
                rpt++;
              }
              rpt--; //Decrement the variable rpt (the repeat count)
              n5.setValue(rpt);
              n0q.setValue(rpt);
              saveSettings();
              nexLoop(nex_listen_list); //Gives the Nextion display a chance to submit a push event (i.e. STOP something)
            }
      }
  STOPPushCallback(&STOP);
  }
}
  
/*
 * SDtest is designed to permit the testing of an SD card breakout board without
 * having to connect a Nextion display.  You could use a full-sized MEGA board to 
 * test to see if the SD board will work, or if you have the jumpers wired correctly
 * before you solder the card.
 * It creates the file "sdtest.txt", stores "sd qsl k", and then sends the file.  What you should
 * then hear is:
 * boop_beep
 * beep
 * sd qsl k (in CW)
 * If anything fails, you won't hear "sd qsl k".  You might hear a warble, or you mmight hear nothing.
 * Anything other than "sd qsl k" will require troubleshooting.
 */
void SDtest() {
  boop_beep();
  numi = 1;
  numb = "sdtest";
  keyBuff = "sd qsl k";
  LOAD2PushCallback(&LOAD2);
  myDelay(3000);
  numi = 1;
  numb = "sdtest";
  keyBuff = "";
  SENDPushCallback(&SEND);
}

/*
 * STOP button push callback function. 
 * This may be the most specifically called function in the sketch.
 * It is designed to stop the keyer from whatever it is currently doing
 * and return the keyer to the NORM (normal) state.
 */
void STOPPushCallback(void *ptr) {
    if (!setUp) beep(); //Extra beeps won't sound during Setup
    tx_and_sidetone_key(0);
    numb="";
    numi=0; //setting numi=0 & numb="" effectively clears any digits that may be in the "number buffer"
    rpt=0;
    if (PRACon) ledPRACoff(); 
    if (LOADon) ledLOADoff(); 
    if (PAUSEon) ledPAUSEoff(); 
    if (SENDon) ledSENDoff(); 
    ledNORMon(); //Normal LED configuration
    DisplayText("off");
    MsgDisp(" ");
    dispTxtHold = dispTxt; // 1 NOV 2020 WW9JD - Permits clearing of text display (if hidden so that Farnsworth text speed is accurate)
    dispTxt = true;        //above
    qwertyDisp(" ");
    dispTxt = dispTxtHold; //above
    keyBuff="";
    n5.setValue(rpt);
    n0q.setValue(rpt);
    saveSettings();
    loadSettings();
    pracState=false;
    endFile=false;
}

/*
 * OFF display function
 * Tap the DisplayText area (showing "off" when not showing anything else) to put both the
 * keyer and the display to sleep.  It doesn't stop all power use, but it reduces it.
 * One of the acknowledged problems with this sleep procedure is the need for a physical button
 * to call the Arduino out of sleep.
 * To exit the sleep mode, tap the dit paddle or the straight key.
 * 
 * The Sleep/Wake functions were taken from a tutorial by Ab Kurk.
 * WW9JD modified it, but Ab's tutorial got the process started.
 * 
 * Author:Ab Kurk
 * version: 1.0
 * date: 24/01/2018
 * Description: 
 * This sketch is part of the beginners guide to putting your Arduino to sleep
 * tutorial. It is to demonstrate how to put your arduino into deep sleep and
 * how to wake it up.
 * Link To Tutorial http://www.thearduinomakerman.info/blog/2018/1/24/guide-to-arduino-sleep-mode
 */
void EnterTextPushCallback(void *ptr) {
    myDelay(100);
    sendCommand("thup=0");
    sendCommand("usup=1");
    sendCommand("sleep=1");
    Serial1.flush();
    sleep_enable();//Enabling sleep mode
    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    attachInterrupt(1, wakeUp, LOW);//attaching a interrupt to pin d3
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    sleep_cpu();//activating sleep mode
}

/*
 * The next 2 functions are both parts of the Sleep/Wake function
 */
void EnterText4PushCallback(void *ptr) {
    EnterTextPushCallback(&EnterText);
}

void wakeUp(){
    sleep_disable();//Disable sleep mode
    detachInterrupt(0); //Removes the interrupt from pin 2;
    detachInterrupt(1); //Removes the interrupt from pin 3;
    sendCommand("thup=1");
    sendCommand("sleep=0");
    Serial1.flush();
}

/*
 * 30 MAY 2020 WW9JD
 * Nextion AutoSleep calls these functions to also put the Mega 2560 to sleep/unSleep if !BacklightOnly
 * This feature requires a change to NexHardware.cpp and the addition of a hidden button on the Splash Page,
 * along with the needed code to make use of that button's Push event (just like any other button responding
 * to a physical touch/push)
 */
void AutoSleepPushCallback(void *ptr) {
    sendCommand("sleep=1");
    if(!BacklightOnly){
      myDelay(100);
      Serial1.flush();
      sleep_enable();//Enabling sleep mode
      attachInterrupt(0, unSleep, LOW);//attaching a interrupt to pin d2
      attachInterrupt(1, unSleep, LOW);//attaching a interrupt to pin d3
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
      sleep_cpu();//activating sleep mode
    }
}

void unSleep(){
    sleep_disable();//Disable sleep mode
    detachInterrupt(0); //Removes the interrupt from pin 2;
    detachInterrupt(1); //Removes the interrupt from pin 3;
    sendCommand("sleep=0");
    Serial1.flush();
}

/*
 * The next 6 functions use parts of the Sleep/Wake function from the buttons on the Setup Page //21 MAY 2020 WW9JD
 */
void SLEEP0PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(65520);
  SLEEP30.Set_background_color_bco(34815);
  SLEEP2.Set_background_color_bco(34815);
  SLEEP5.Set_background_color_bco(34815);
  SLEEP10.Set_background_color_bco(34815);
  beep();
}

void SLEEP1PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(65520);
  SLEEP30.Set_background_color_bco(34815);
  SLEEP2.Set_background_color_bco(34815);
  SLEEP5.Set_background_color_bco(34815);
  SLEEP10.Set_background_color_bco(34815);
  beep();
  AutoSleepPushCallback(&AutoSleep); //29 MAY 2020 WW9JD Settings Sleep NOW button puts Nextion & Mega 2560 to sleep
}

void SLEEP30PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(34815);
  SLEEP30.Set_background_color_bco(65520);
  SLEEP2.Set_background_color_bco(34815);
  SLEEP5.Set_background_color_bco(34815);
  SLEEP10.Set_background_color_bco(34815);
  beep();
}

void SLEEP2PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(34815);
  SLEEP30.Set_background_color_bco(34815);
  SLEEP2.Set_background_color_bco(65520);
  SLEEP5.Set_background_color_bco(34815);
  SLEEP10.Set_background_color_bco(34815);
  beep();
}

void SLEEP5PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(34815);
  SLEEP30.Set_background_color_bco(34815);
  SLEEP2.Set_background_color_bco(34815);
  SLEEP5.Set_background_color_bco(65520);
  SLEEP10.Set_background_color_bco(34815);
  beep();
}

void SLEEP10PushCallback(void *ptr) {
  SLEEP0.Set_background_color_bco(34815);
  SLEEP30.Set_background_color_bco(34815);
  SLEEP2.Set_background_color_bco(34815);
  SLEEP5.Set_background_color_bco(34815);
  SLEEP10.Set_background_color_bco(65520);
  beep();
}


/*
 * WPM button push callback function. 
 * If 1 or 2 black digit keys have already been touched, when the WPM button is touched,
 * keyer speed is set, the length of a dit is calculated, the values are stored & displayed.
 * When you make a speed change, Farnsworth speed is always set to equal the keyer speed.
 */
void WPMPushCallback(void *ptr) {
    if((numi>0)&&(numi<3)){
      k_WPM = numb.toFloat();
      f_WPM = k_WPM; //Farnsworth speed = keyer speed
      k_const=T_Const/100;
      dit = (1200/k_WPM)*k_const; // Dit length calculation
      saveSettings();
    }
    if((numi==0) && (!tuneState))warble();
    WPMSlider.setValue(k_WPM); //Speed display on the slider
    n0.setValue(k_WPM); //Speed display on the top of the page
    n4.setValue(f_WPM); //Farnsworth speed display on the top of the page
    n1q.setValue(k_WPM);//5 DEC 2020 WW9JD QWERTY Page WPM display added for Contest Mode
    STOPPushCallback(&STOP);
   }

/*
 * WPMSlider component pop function. 
 * When the WPMSlider is released, the keyer speed is set, saved, and the keyer sends "CQ". 
 * CQ was chosen to give you a good basis to evaluate the keyer's speed because you'll likely
 * know quickly if the speed is set too fast or too slow by listening to CQ.
 */
void WPMSliderPopCallback(void *ptr) {
    tx_and_sidetone_key(0);
    uint32_t sVal;
    WPMSlider.getValue(&sVal);
    k_WPM = sVal;
    f_WPM = k_WPM; //Farnsworth speed = keyer speed
    WPMSlider.setValue(k_WPM);  
    n0.setValue(k_WPM);
    n4.setValue(f_WPM);
    n5.setValue(rpt);
    numi=0; //setting numi=0 effectively clears any digits that may be in the "number buffer"
    PRACPushCallback(&PRAC);
    k_const=T_Const/100;
    dit = (1200/k_WPM)*k_const; // Dit length calculation
    keyBuff = "cq";
    while (keyBuff != ""){
      loop(); //Gives the sketch the chance to send CQ
    }
    saveSettings();
    STOPPushCallback(&STOP);
}

/*
 * RANGE button push callback functions. 
 * When the RANGE button is touched, the WPM slider range changes and the thumb repositions. 
 */
void RANGEPushCallback(void *ptr) {
    tx_and_sidetone_key(0);
    numi=0;
    beep();
}

void RANGEPopCallback(void *ptr) {
    WPMSlider.setValue(k_WPM-1);  
    WPMSlider.setValue(k_WPM);  
}
 
/*
 * The following 2 functions take a string and display it in the message box area on the Main & QWERTY pages
 */
void MsgDisp(String MsgTxt){
  char charHold[30];
  MsgTxt.toCharArray(charHold, 30);
  ErrTxt.setText(charHold);
}

void qwertyDisp(String MsgTxt){
  if (dispTxt || cmnt){             // 1 NOV 2020 WW9JD - Show/hide text so that Farnsworth text speed is accurate
    char charHold[30];
    MsgTxt.toCharArray(charHold, 30);
    qwertyText.setText(charHold);
    kybdText.setText(charHold);
    n1q.setValue(k_WPM);//5 DEC 2020 WW9JD QWERTY Page WPM display added for Contest Mode
  }
}

/*
 *These functions are called when the call sign displayed on the Main or QWERTY pages are tapped. 
 */
void callTextPushCallback(void *ptr) {
    //Not used at this time
}

void callText4PushCallback(void *ptr) {
    callTextPushCallback(&callText);
}

/*
 * ===========================================================================================
 * Display Settings page functions
 * ===========================================================================================
 */

/*
 * Backlight button push callback function. 
 * When the Backlight checkbox is touched, the BacklightOnly features toggles between true & false. 
 * The chosen state is saved in EEPROM, and restored on the next startup //31 MAY 2020 WW9JD
 */
void BacklightPushCallback(void *ptr) {
  if (BacklightOnly == false){
    BacklightOnly = true;
    Backlight.setValue(1);
  }else{
    BacklightOnly = false;
    Backlight.setValue(0);
    }
  saveSettings();
  beep();
  }

void BLTextPushCallback(void *ptr) {
  BacklightPushCallback(&Backlight);
}


/*
 * ===========================================================================================
 * Keyer Settings page functions
 * ===========================================================================================
 */

/*
 * The mode_slider component pop function. 
 * When the mode_slider is released, the keyer Mode is selected. 
 * Mode possibilities are: Iambic, Iambic Reversed, Bug, Single, and Sngl_Rev
 */
void mode_sliderPopCallback(void *ptr) {
    uint32_t mVal = 0;
    mode_slider.getValue(&mVal);
    switch(mVal){
      case 0 : {  Mode = Iambic; //1/8" stereo plug: tip-dit / ring-dah / sleeve-ground
                  mode_text.setText("Iambic");
                  break;
                  }
      case 1 : {  Mode = Iambic_Rev; //1/8" stereo plug: tip-dah / ring-dit / sleeve-ground
                  mode_text.setText("Reversed");
                  break;
                  }
      case 2 : {  Mode = Bug; //1/8" stereo plug: tip-dit / ring-straight key / sleeve-ground
                  mode_text.setText("Bug");
                  break;
                  }
                   //19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
      case 3 : {  Mode = Single; //1/8" stereo plug: tip-dit / ring-dah / sleeve-ground - NO dit/dah memory
                  mode_text.setText("Single");
                  break;
                  }
                   //19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key)
      case 4 : {  Mode = Single_Rev; //1/8" stereo plug: tip-dah / ring-dit / sleeve-ground - NO dit/dah memory
                  mode_text.setText("Sngl-Rev");
                  break;
                  }
    }
    beep();   
    STOPPushCallback(&STOP);
}
  

/*
 * PitchSlider component pop function. 
 * When the PitchSlider is released, the pitch (frequency) of the sidetone is set, saved, and the keyer
 * plays a sidetone at the new pitch. 
 */
void PitchSliderPopCallback(void *ptr) {
    uint32_t pVal = 0;
    PitchSlider.getValue(&pVal);
    toneFreq = pVal;
    toneAC(toneFreq, toneVol);
    delay(500); //Plays tone at new pitch for 1/2 sec
    noToneAC();
    saveSettings();
}
  

/*
 * The VolSlider component push function. 
 * When the VolSlider is released, the Volume of the sidetone is set, saved, and the keyer
 * plays a sidetone at the new volume. 
 */
void VolSliderPopCallback(void *ptr) {
    uint32_t vVal = 0;
    VolSlider.getValue(&vVal);
    Vol.setValue(vVal);
    toneVol = vVal;
    toneAC(toneFreq, toneVol);
    delay(500); //Plays tone at new volume for 1/2 sec
    noToneAC();
    saveSettings();
}
  

/*
 * The T_Slider component pop function. 
 * When the T_Slider is released, the keyer Timing Constant is set, saved, and the keyer
 * plays a beep. 
 * 
 * NOTE: You must restart the keyer for this action to take effect
 * 
 * The Timing Constant is adjusted to make up for differences in the internal timing
 * of different microcontroller boards.  A lower % speeds up sending.  Coupled with the WPM
 * setting, you should be able to adjust the Timing Constant so that at 10 WPM "paris" is sent 
 * 10 times in 60 seconds.  While not hard to create, files containing 5, 10, 13, 15, 18,
 * 20, 25, and 30 "paris" are included in the SD card download area.  Adjust the T_Slider so
 * that you get the right number of "paris" sent in the selected WPM.
 */
void T_SliderPopCallback(void *ptr) {
    uint32_t tVal = 0;
    T_Slider.getValue(&tVal);
    Time.setValue(tVal);
    T_Const = tVal;
    saveSettings();
    k_const=T_Const/100;        // 11 JUL 2020 WW9JD - Added DIT length reset every time 
    dit = (1200/k_WPM)*k_const; //  the Timing Constant is changed 
    beep();
}
  

/*
 * Button PTTSlider component push callback function. 
 * When the PTTSlider  is released, the time difference between closing the contacts for PTT and SEND
 * is set and saved.
 * This feature is included in case your equipment needs to have the PTT closed before the transmitter
 * can be keyed.  If you need to switch much current, you will need to supply an outboard relay.
 * There is only an optoisolator between the microprocessor board and the transmitter keying jack.
 */
void PTTSliderPopCallback(void *ptr) {
    uint32_t pVal = 0;
    PTTSlider.getValue(&pVal);
    PTT.setValue(pVal);
    PTTwait = pVal;
    saveSettings();
    beep();
}
  

/*
 * QWERTY push callback functions. 
 * When the QWERTY key is touched, the QWERTY keyboard is displayed with the LEDs set. 
 */
void qwerty_2PushCallback(void *ptr) {
    qwerty_1PushCallback(&qwerty_1);
}
  
void qwerty_3PushCallback(void *ptr) {
    qwerty_1PushCallback(&qwerty_1);
}
  
void qwerty_4PushCallback(void *ptr) {
    qwerty_1PushCallback(&qwerty_1);
}
  
void qwerty_5PushCallback(void *ptr) {
    qwerty_1PushCallback(&qwerty_1);
}
  

/*
 * Clear the EEPROM completely
 * Store an "R" at EEPROM location 1023 so that the keyer will load the default values the next time
 */
void resetKeyerPushCallback(void *ptr){
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
      EEPROM.put(E_INIT, 'R'); 
      beep();   
}


/*
 * 5 DEC 2020 WW9JD Added for Contest Mode
 * Reset the selected Contest Serial Number to 0
 */
void resetIPushCallback(void *ptr){
    testI = 1; 
    switch (testN){
      case 1: test1 = testI; break;                     
      case 2: test2 = testI; break;                     
      case 3: test3 = testI; break;                     
      case 4: test4 = testI; break;                     
      case 5: test5 = testI; break;                     
      case 6: test6 = testI; break;                     
    }
    n0t.setValue(test1);
    n2t.setValue(test2);
    n6t.setValue(test3);
    n7t.setValue(test4);
    n8t.setValue(test5);
    n9t.setValue(test6);
    saveSettings();
    beep();   
}


/*
 * 5 DEC 2020 WW9JD Added for Contest Mode
 * Set serial number leading zeros
 */
void bZeroPushCallback(void *ptr){
    String holdText;
    char holdTxt[15];
    Zero++;
    if (Zero >= 6) Zero = 0;
    saveSettings();
    loadSettings();
    holdText = "Zeros - " + String(Zero);
    holdText.toCharArray(holdTxt, 15);
    bZero.setText(holdTxt);
    beep();   
}


/*
 * 5 DEC 2020 WW9JD Added for Contest Mode
 * Set Send CQ or Hunt & Pounce mode
 */
void bqModePushCallback(void *ptr){
    uint32_t hideTxt;
    bqMode.getValue(&hideTxt);
    if (hideTxt==1){
     qMode = 1; 
    }
    if (hideTxt==0){
     qMode = 0; 
    }
    beep();
    saveSettings();
}


/*
 * 5 DEC 2020 WW9JD Added for Contest Mode
 * Select the Contest Serial Number
 */
void r0PushCallback(void *ptr){
      testN = 1; 
      n0t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}
void r1PushCallback(void *ptr){
      testN = 2; 
      n2t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}
void r2PushCallback(void *ptr){
      testN = 3; 
      n6t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}
void r3PushCallback(void *ptr){
      testN = 4; 
      n7t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}
void r4PushCallback(void *ptr){
      testN = 5; 
      n8t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}
void r5PushCallback(void *ptr){
      testN = 6; 
      n9t.getValue(&holdI);
      testI = holdI;
      saveSettings();
      beep();   
}


/*
 * Function added 1 NOV 2020 WW9JD
 * Display/hide the text from a file as a file is sent.
 * - It turns out that the trouble I was having getting the Farnsworth timing correct was the 
 * delay added by communicating the display text to the Nextion. With the display turned off,
 * the Farnsworth formula worked perfectly AND no timing factor was needed (Timing Constant = 100%)
 * - The Display/Hide Text button on the Keyer Settings page gives the option to hide text display
 * for those times that exact timing is neede (e.g. CW training/practice). 
 * - Text is only hidden during transmission. Comment text is always displayed.
 */
void txtDspPushCallback(void *ptr){
    uint32_t hideTxt;
    txtDsp.getValue(&hideTxt);
    if (hideTxt==1){
     dispTxt = false; 
    }
    if (hideTxt==0){
     dispTxt = true; 
    }
      beep(); 
}

/*
 * Function added 19 NOV 2020 WW9JD
 * Switch between Iambic modes A & B
 */
void iModePushCallback(void *ptr){
    uint32_t abHold;
    iMode.getValue(&abHold);
    if (abHold==0){
     abMode = false; 
    }
    if (abHold==1){
     abMode = true; 
    }
    beep();
    saveSettings();
}


/*
 * ===========================================================================================
 * Page display functions
 * ===========================================================================================
 */

/*
 * Splash Page component push callback function. 
 * When the Splash Page is touched, the Main page is displayed with its display values set. 
 */
void SplashPushCallback(void *ptr) {
    numi=0; //setting numi=0 effectively clears any digits that might be in the "number buffer" when opening the MAIN page
    callText.setText(call);
    WPMSlider.setValue(k_WPM);  
    n0.setValue(k_WPM);
    if (Weighting) n3.Set_font_color_pco(0); //WT font color = black
    n3.setValue(wt);
    n4.setValue(f_WPM);
    n5.setValue(rpt);
    Backlight.setValue(BacklightOnly);
    if (!unMute){
      MUTE.setText("UNMUTE");
    }else{
      MUTE.setText("MUTE");
    }
    STOPPushCallback(&STOP);
}
  
/*
 * Splash Page SplashPic picture component push callback function. 
 * Since the photograph on the Splash page covers the entire page, when the SplashPic picture is touched,
 * the Splash push callback function is called. 
 */
void SplashPicPushCallback(void *ptr) {
  SplashPushCallback(&Splash);
}

/*
 * Main Page QWERTY push callback function. 
 * When the QWERTY KYBD key is touched, the QWERTY keyboard is displayed with the LEDs set. 
 */
void qwerty_1PushCallback(void *ptr) {
  String holdLabel;
  char charLabel[6];
  loadSettings();
  holdLabel = String(testI);
  holdLabel.toCharArray(charLabel, 6);
  bSerial.setText(charLabel);
  SerialFont();
  if(vMode){
    if (qMode == 0) nMode.setText("Call CQ");
    if (qMode == 1) nMode.setText("H & P");
    nMode.Set_background_color_bco(64512);
    b16.Set_background_color_bco(64512);
    b20.Set_background_color_bco(64512);
    b21.Set_background_color_bco(64512);
    b22.Set_background_color_bco(64512);
    b23.Set_background_color_bco(64512);
    b24.Set_background_color_bco(64512);
    b25.Set_background_color_bco(64512);
    b26.Set_background_color_bco(64512);
    b27.Set_background_color_bco(64512);
    b28.Set_background_color_bco(64512);
    b29.Set_background_color_bco(64512);
    bSerial.Set_background_color_bco(64512);
    PRAC2.Set_background_color_bco(64512);
    PRAC2.setText("C");
    callText4.setText(call2);
  }else{
    nMode.setText("Normal");
    nMode.Set_background_color_bco(2024);
    b16.Set_background_color_bco(2016);
    b20.Set_background_color_bco(0);
    b21.Set_background_color_bco(0);
    b22.Set_background_color_bco(0);
    b23.Set_background_color_bco(0);
    b24.Set_background_color_bco(0);
    b25.Set_background_color_bco(0);
    b26.Set_background_color_bco(0);
    b27.Set_background_color_bco(0);
    b28.Set_background_color_bco(0);
    b29.Set_background_color_bco(0);
    bSerial.Set_background_color_bco(34815);
    PRAC2.Set_background_color_bco(34815);
    PRAC2.setText("WPM");
    callText4.setText(call);
  }
  STOPPushCallback(&STOP);
  beep();
}
  

/*
 * REFS Page push callback function. 
 * When the REFS key is touched, the References Index page is displayed. 
 */
void Refs_1PushCallback(void *ptr) {
    beep();
}
  
void Refs_2PushCallback(void *ptr) {
    beep();
}
  
void Refs_3PushCallback(void *ptr) {
    beep();
}
  
void Refs_4PushCallback(void *ptr) {
    beep();
}
  
void Refs_5PushCallback(void *ptr) {
    beep();
}
  

/*
 * KEYER push callback function. 
 * When the KEYER key is touched, the Keyer Settings page is displayed. 
 */
void Keyer_1PushCallback(void *ptr) {
    Keyer_3PushCallback(&Keyer_3); 
}
  
void Keyer_2PushCallback(void *ptr) {
    Keyer_3PushCallback(&Keyer_3); 
}
  
void Keyer_4PushCallback(void *ptr) {
    Keyer_3PushCallback(&Keyer_3); 
}
  
void Keyer_5PushCallback(void *ptr) {
    Keyer_3PushCallback(&Keyer_3); 
}
  

/*
 * Settings page KEYER SETTINGS button push callback function. 
 * When the KEYER SETTINGS button is touched, the Keyer Settings page is displayed 
 * and the page's controls are set to display their current values.
 */
void Keyer_3PushCallback(void *ptr) {
    String holdText;
    char holdTxt[15];
    saveSettings();
    loadSettings();
    holdText = "Zeros - " + String(Zero);
    holdText.toCharArray(holdTxt, 15);
    bZero.setText(holdTxt);
    n0t.setValue(test1); //5 DEC 2020 WW9JD Contest Mode
    n2t.setValue(test2); //
    n6t.setValue(test3); //
    n7t.setValue(test4); //
    n8t.setValue(test5); //
    n9t.setValue(test6); //
    Pitch.setValue(toneFreq);
    VolSlider.setValue(toneVol);
    Vol.setValue(toneVol);
    T_Slider.setValue(T_Const);
    Time.setValue(T_Const);
    PTTSlider.setValue(PTTwait);
    PTT.setValue(PTTwait);
    mode_slider.setValue(Mode);
    iMode.setValue(abMode);// 19 NOV 2020 WW9JD - Set Iambic Mode button
    bqMode.setValue(qMode);// 19 NOV 2020 WW9JD - Set QSO Mode button
    if (!dispTxt){
      txtDsp.setValue(1);
    }
    if (abMode==1){
      iMode.setText("Iambic Mode B");
    }
    if (qMode==1){
      bqMode.setText("Hunt & Pounce");
    }
    switch (Mode){
      case 0 : {mode_text.setText("Iambic"); 
                break;
                }
      case 1 : {mode_text.setText("Reversed");
                break;
                }
      case 2 : {mode_text.setText("Bug");
                break;
                }
      case 3 : {mode_text.setText("Single"); //19 NOV 2020 WW9JD - Added Single-Lever paddle mode (with dual-lever key) 
                break;
                }
      case 4 : {mode_text.setText("Sngl_Rev");//
                break;
                }
    }
    switch (testN){
      case 1 : {r0.setValue(1);
                r1.setValue(0);
                r2.setValue(0);
                r3.setValue(0);
                r4.setValue(0);
                r5.setValue(0);
                break;
                }
      case 2 : {r0.setValue(0);
                r1.setValue(1);
                r2.setValue(0);
                r3.setValue(0);
                r4.setValue(0);
                r5.setValue(0);
                break;
                }
      case 3 : {r0.setValue(0);
                r1.setValue(0);
                r2.setValue(1);
                r3.setValue(0);
                r4.setValue(0);
                r5.setValue(0);
                break;
                }
      case 4 : {r0.setValue(0);
                r1.setValue(0);
                r2.setValue(0);
                r3.setValue(1);
                r4.setValue(0);
                r5.setValue(0);
                break;
                }
      case 5 : {r0.setValue(0);
                r1.setValue(0);
                r2.setValue(0);
                r3.setValue(0);
                r4.setValue(1);
                r5.setValue(0);
                break;
                }
      case 6 : {r0.setValue(0);
                r1.setValue(0);
                r2.setValue(0);
                r3.setValue(0);
                r4.setValue(0);
                r5.setValue(1);
                break;
                }
    }
    beep();
}

/*
 * Main page SETTINGS button push callback function. 
 * When the Main Page SETTINGS button is touched, the Display Settings page is shown. 
 */
void Settings_2PushCallback(void *ptr) {
    Backlight.setValue(BacklightOnly);
    beep();
}

/*
 * QWERTY page SETTINGS button push callback function. 
 * When the QWERTY page SETTINGS button is touched,
 * the Display Settings page is displayed. 
*/
void Settings_1PushCallback(void *ptr) {
    Settings_2PushCallback(&Settings_2); //5 DEC 2020 WW9JD
}

void Settings_3PushCallback(void *ptr) {
    Settings_2PushCallback(&Settings_2); //5 DEC 2020 WW9JD
}

void Settings_4PushCallback(void *ptr) {
    Settings_2PushCallback(&Settings_2); //5 DEC 2020 WW9JD
}

void Settings_5PushCallback(void *ptr) {
    Settings_2PushCallback(&Settings_2); //5 DEC 2020 WW9JD
}


/*
 * Display Settings page 1ST button push callback function. 
 * When the Display Settings page 1ST button is touched, the 1st page
 * (also known as the splash page) is displayed. 
 */
void bsplashPushCallback(void *ptr) {
    beep();
}

/*
 * Display Settings page REF button push callback function. 
 * When the Display Settings page REF button is touched,
 * the References Index page is displayed. 
 */
void brefPushCallback(void *ptr) {
    beep();
}

/*
 * Main page 1st button push callback function (changes to the 1st "splash" Page)). 
 */
void First_1PushCallback(void *ptr) {
    beep();
}

/*
 * Settings page 1st button push callback function (changes to the 1st "splash" Page)). 
 */
void First_2PushCallback(void *ptr) {
    beep();
}

/*
 * Keyer Settings page 1st button push callback function (changes to the 1st "splash" Page)). 
 */
void First_3PushCallback(void *ptr) {
    beep();
}

/*
 * QWERTY page 1st button push callback function (changes to the 1st "splash" Page)). 
 */
void First_4PushCallback(void *ptr) {
    beep();
}

/*
 * Refs page 1st button push callback function (changes to the 1st "splash" Page)). 
 */
void First_5PushCallback(void *ptr) {
    beep();
}

/*
 * Main page HOME button push callback function (calls the Splash function). 
 * When the Display Settings page HOME button is touched,
 * the Main page is displayed with its display values set. 
 */
void Home_1PushCallback(void *ptr) {
   SplashPushCallback(&Splash);
}

/*
 * Display Settings page HOME button push callback function (calls the Splash function). 
 * When the Display Settings page HOME button is touched,
 * the Main page is displayed with its display values set. 
 */
void Home_2PushCallback(void *ptr) {
   SplashPushCallback(&Splash);
}

/*
 * Keyer Settings page HOME button push callback function (calls the Splash function). 
 * When the Display Settings page HOME button is touched,
 * the Main page is displayed with its display values set. 
 */
void Home_3PushCallback(void *ptr) {
   SplashPushCallback(&Splash);
}

/*
 * QWERTY page HOME button push callback function (calls the Splash function). 
 * When the QWERTY page HOME button is touched,
 * the Main page is displayed with its display values set. 
 */
void Home_4PushCallback(void *ptr) {
   SplashPushCallback(&Splash);
}


/*
 * References Index page HOME button push callback function (calls the Splash function). 
 * When the References Index page HOME button is touched,
 * the Main page is displayed with its display values set. 
 */
void Home_5PushCallback(void *ptr) {
   SplashPushCallback(&Splash);
}


/*
 * ===========================================================================================
 * Main Page Number Key functions
 * ===========================================================================================
 */

void _0PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"0";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="0";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }


void _1PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"1";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="1";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _2PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"2";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="2";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _3PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"3";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="3";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _4PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"4";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="4";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _5PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"5";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="5";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _6PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"6";
      numi++;
      boop_beep();
   }
    if(numi==0){
      numb="6";  
      numi++;
      beep();
   }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _7PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"7";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="7";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _8PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"8";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="8";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }

void _9PushCallback(void *ptr) {
    if (numi>1) numi=0;
    if (tuneState){
      numi=4; //Ignore number property of button if tuning
      ledNORMon();
      beep();
    }
     ledNORMdim();
    if(numi==1){
      numb=numb+"9";
      numi++;
      boop_beep();
    }
    if(numi==0){
      numb="9";  
      numi++;
      beep();
    }
    DisplayText(numb);
    if (tuneState) ledNORMon();
    tx_and_sidetone_key(0);
   }


/*
 * Function to display number key text on the Main & QWERTY pages
 */
void DisplayText (String nn){
      char buffer [100];
      nn.toCharArray(buffer,100);
      EnterText.setText(buffer);
      EnterText4.setText(buffer);
}


/*
 * ===========================================================================================
 * QWERTY Page button functions
 * ===========================================================================================
 */

void RPT2PushCallback(void *ptr) {
    RPTPushCallback(&RPT); 
}

void PRAC2PushCallback(void *ptr) {
    if (vMode == 0){
      WPMPushCallback(&WPM); 
    }else{
      numi = 1;
      numb = "C";
      DisplayText(numb);
    }
}

void PCbPushCallback(void *ptr) {
    PCPushCallback(&PC); 
}

void LOADPushCallback(void *ptr) {
   if (tuneState){
      warble();
      STOPPushCallback(&STOP);
      return;
    }
   if (numi != 0){
      sendCommand("page qwerty");  //14 APR 2020 WW9JD - Finish LOAD on QWERTY page if started on MAIN page
      Serial1.flush();             // 
      LOAD2PushCallback(&LOAD2);   //
   }else{
      warble();
   }
}

void SEND2PushCallback(void *ptr) {
    SENDPushCallback(&SEND); 
}

void STOP2PushCallback(void *ptr) {
    STOPPushCallback(&STOP); 
}

/*
 * 5 DEC 2020 WW9JD
 * This increments the contest Serial Number and sends the contents of the C1 memory,
 * which is intended to be the contest CQ
 */
void bSerialPushCallback(void *ptr) {
  String holdLabel;
  char charLabel[6];
  testI++;
  holdLabel = "[N/A]";
  holdLabel.toCharArray(call2, 30);
  if (vMode == 0) callText4.setText(call);
  if (vMode == 1) callText4.setText(call2);
  sTest = false;
  saveSettings();
  loadSettings();
  holdLabel = String(testI);
  holdLabel.toCharArray(charLabel, 6);
  bSerial.setText(charLabel);
  SerialFont();
  if ((vMode == 1) && (qMode == 0)) b20PushCallback(&b20); 
}

void _ZPushCallback(void *ptr) {
  keyBuff = keyBuff + "z";
}

void BXPushCallback(void *ptr) {
  keyBuff = keyBuff + "x";
}

void BCPushCallback(void *ptr) {
  keyBuff = keyBuff + "c";
}

void _VPushCallback(void *ptr) {
  keyBuff = keyBuff + "v";
}

void BBPushCallback(void *ptr) {
  keyBuff = keyBuff + "b";
}

void BNPushCallback(void *ptr) {
  keyBuff = keyBuff + "n";
}

void _MPushCallback(void *ptr) {
  keyBuff = keyBuff + "m";
}

void COMPushCallback(void *ptr) {
  keyBuff = keyBuff + ",";
}

void PERPushCallback(void *ptr) {
  keyBuff = keyBuff + ".";
}

void BARPushCallback(void *ptr) {
  keyBuff = keyBuff + "/";
}

void _APushCallback(void *ptr) {
  keyBuff = keyBuff + "a";
}

void BSPushCallback(void *ptr) {
  keyBuff = keyBuff + "s";
}

void _DPushCallback(void *ptr) {
  keyBuff = keyBuff + "d";
}

void _FPushCallback(void *ptr) {
  keyBuff = keyBuff + "f";
}

void _GPushCallback(void *ptr) {
  keyBuff = keyBuff + "g";
}

void _HPushCallback(void *ptr) {
  keyBuff = keyBuff + "h";
}

void _JPushCallback(void *ptr) {
  keyBuff = keyBuff + "j";
}

void _KPushCallback(void *ptr) {
  keyBuff = keyBuff + "k";
}

void BLPushCallback(void *ptr) {
  keyBuff = keyBuff + "l";
}

void _QMPushCallback(void *ptr) {
  keyBuff = keyBuff + "?";
}

void _QPushCallback(void *ptr) {
  keyBuff = keyBuff + "q";
}

void _WPushCallback(void *ptr) {
  keyBuff = keyBuff + "w";
}

void _EPushCallback(void *ptr) {
  keyBuff = keyBuff + "e";
}

void _RPushCallback(void *ptr) {
  keyBuff = keyBuff + "r";
}

void _TPushCallback(void *ptr) {
  keyBuff = keyBuff + "t";
}

void _YPushCallback(void *ptr) {
  keyBuff = keyBuff + "y";
}

void BUPushCallback(void *ptr) {
  keyBuff = keyBuff + "u";
}

void _IPushCallback(void *ptr) {
  keyBuff = keyBuff + "i";
}

void _OPushCallback(void *ptr) {
  keyBuff = keyBuff + "o";
}

void BPPushCallback(void *ptr) {
  keyBuff = keyBuff + "p";
}

void b_1PushCallback(void *ptr) {
  keyBuff = keyBuff + "1";
}

void b_2PushCallback(void *ptr) {
  keyBuff = keyBuff + "2";
}

void b_3PushCallback(void *ptr) {
  keyBuff = keyBuff + "3";
}

void b_4PushCallback(void *ptr) {
  keyBuff = keyBuff + "4";
}

void b_5PushCallback(void *ptr) {
  keyBuff = keyBuff + "5";
}

void b_6PushCallback(void *ptr) {
  keyBuff = keyBuff + "6";
}

void b_7PushCallback(void *ptr) {
  keyBuff = keyBuff + "7";
}

void b_8PushCallback(void *ptr) {
  keyBuff = keyBuff + "8";
}

void b_9PushCallback(void *ptr) {
  keyBuff = keyBuff + "9";
}

void b_0PushCallback(void *ptr) {
  keyBuff = keyBuff + "0";
}

void errorPushCallback(void *ptr) {
  keyBuff = keyBuff + "<%>";
}

void WORDPushCallback(void *ptr) {
  keyBuff = keyBuff + " ";
}

void BTPushCallback(void *ptr) {
  keyBuff = keyBuff + "=";
}

//Shave & a haircut (pause) 2 bits
void b0PushCallback(void *ptr) {
  keyBuff = keyBuff + "<ese | ee>";
}

void b1PushCallback(void *ptr) {
  keyBuff = keyBuff + "qrl?";
}

void b2PushCallback(void *ptr) {
  keyBuff = keyBuff + "qth";
}

void b3PushCallback(void *ptr) {
  keyBuff = keyBuff + "qsl";
}

void b4PushCallback(void *ptr) {
  keyBuff = keyBuff + "tnx";
}

void b5PushCallback(void *ptr) {
  keyBuff = keyBuff + "73";
}

void b6PushCallback(void *ptr) {
  keyBuff = keyBuff + "<bk>";
}

void b7PushCallback(void *ptr) {
  keyBuff = keyBuff + "<sk>";
}

void b8PushCallback(void *ptr) {
  keyBuff = keyBuff + "<ar>";
}

void b9PushCallback(void *ptr) {
  keyBuff = keyBuff + "cq";
}

void b10PushCallback(void *ptr) {
  keyBuff = keyBuff + ">";
}

void b11PushCallback(void *ptr) {
  keyBuff = keyBuff + "<";
}

void b12PushCallback(void *ptr) {
  keyBuff = keyBuff + "|";
}

void b13PushCallback(void *ptr) {
  keyBuff = keyBuff + "#";
}

void b14PushCallback(void *ptr) {
  keyBuff = keyBuff + "]";
}

void b15PushCallback(void *ptr) {
  keyBuff = keyBuff + "[";
}

void b16PushCallback(void *ptr) {
  if (vMode == 0){
    if (LOADon){
      keyBuff = keyBuff + "*";
    }else{
      keyBuff = keyBuff + call;
    }
  }else{  
    if (LOADon){
      keyBuff = keyBuff + "^";
    }else{
      keyBuff = keyBuff + call2;
    }
  }
}

void b17PushCallback(void *ptr) {
  keyBuff = keyBuff + "qrp";
}

void b18PushCallback(void *ptr) {
  keyBuff = keyBuff + "rig";
}

void b19PushCallback(void *ptr) {
  keyBuff = keyBuff + "wx";
}

void bendFilePushCallback(void *ptr){
  endFile = true;
}

void b20PushCallback(void *ptr) {
  if (vMode == 0){
    _1PushCallback(&_1);
  }else{
    numb = "C1";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b21PushCallback(void *ptr) {
  if (vMode == 0){
    _2PushCallback(&_2);
  }else{
    numb = "C2";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b22PushCallback(void *ptr) {
  if (vMode == 0){
    _3PushCallback(&_3);
  }else{
    numb = "C3";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b23PushCallback(void *ptr) {
  if (vMode == 0){
   _4PushCallback(&_4);
  }else{
    numb = "C4";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b24PushCallback(void *ptr) {
  if (vMode == 0){
    _5PushCallback(&_5);
  }else{
    numb = "C5";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b25PushCallback(void *ptr) {
  if (vMode == 0){
    _6PushCallback(&_6);
  }else{
    numb = "C6";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b26PushCallback(void *ptr) {
  if (vMode == 0){
    _7PushCallback(&_7);
  }else{
    numb = "C7";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b27PushCallback(void *ptr) {
  if (vMode == 0){
    _8PushCallback(&_8);
  }else{
    numb = "C8";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b28PushCallback(void *ptr) {
  if (vMode == 0){
    _9PushCallback(&_9);
  }else{
    numb = "C9";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

void b29PushCallback(void *ptr) {
  if (vMode == 0){
   _0PushCallback(&_0);
  }else{
    numb = "C0";
    numi = 1;
    SENDPushCallback(&SEND); 
  }
}

//5 DEC 2020 WW9JD Added for Contest Mode
void nModePushCallback(void *ptr){
  loadSettings();
  if (vMode == 0){
    callText4.setText(call2); //Updates the call display on the QWERTY page
    vMode = 1;
  }else{
    callText4.setText(call); //Updates the call display on the QWERTY page 
    vMode = 0;
  }
  SerialFont();
  if(vMode){
    if (qMode == 0) nMode.setText("Call CQ");
    if (qMode == 1) nMode.setText("H & P");
    nMode.Set_background_color_bco(64512);
    b16.Set_background_color_bco(64512);
    b20.Set_background_color_bco(64512);
    b21.Set_background_color_bco(64512);
    b22.Set_background_color_bco(64512);
    b23.Set_background_color_bco(64512);
    b24.Set_background_color_bco(64512);
    b25.Set_background_color_bco(64512);
    b26.Set_background_color_bco(64512);
    b27.Set_background_color_bco(64512);
    b28.Set_background_color_bco(64512);
    b29.Set_background_color_bco(64512);
    bSerial.Set_background_color_bco(64512);
    PRAC2.Set_background_color_bco(64512);
    PRAC2.setText("C");
  }else{
    nMode.setText("Normal");
    nMode.Set_background_color_bco(2024);
    b16.Set_background_color_bco(2016);
    b20.Set_background_color_bco(0);
    b21.Set_background_color_bco(0);
    b22.Set_background_color_bco(0);
    b23.Set_background_color_bco(0);
    b24.Set_background_color_bco(0);
    b25.Set_background_color_bco(0);
    b26.Set_background_color_bco(0);
    b27.Set_background_color_bco(0);
    b28.Set_background_color_bco(0);
    b29.Set_background_color_bco(0);
    bSerial.Set_background_color_bco(34815);
    PRAC2.Set_background_color_bco(34815);
    PRAC2.setText("WPM");
  }
  saveSettings();
  beep();
}

/*
 * 
 */
void SerialFont(){
  if(vMode){
    if (sTest){
      bSerial.Set_font_color_pco(65535);
    }else{
      bSerial.Set_font_color_pco(0);
    }
  }else{
    if (sTest){
      bSerial.Set_font_color_pco(63488);
    }else{
      bSerial.Set_font_color_pco(0);
    }
  }
}

/*
 * ===========================================================================================
 * "LED" functions
 * ===========================================================================================
 */


void ledNORMon(){
    ledNORM.Set_background_color_bco(63488);
    ledNORM.Set_font_color_pco(63488);
    ledNORM2.Set_background_color_bco(63488);
    ledNORM2.Set_font_color_pco(63488);
    NORMon = true;
}

void ledNORMdim(){
    ledNORM.Set_background_color_bco(65535);
    ledNORM.Set_font_color_pco(63488);
    ledNORM2.Set_background_color_bco(65535);
    ledNORM2.Set_font_color_pco(63488);
    NORMon = true;
}

void ledNORMoff(){
    ledNORM.Set_background_color_bco(65535);
    ledNORM.Set_font_color_pco(65535);
    ledNORM2.Set_background_color_bco(65535);
    ledNORM2.Set_font_color_pco(65535);
    NORMon = false;
}

void ledPRACon(){
    ledPRAC.Set_background_color_bco(63488);
    ledPRAC.Set_font_color_pco(63488);
    ledPRAC2.Set_background_color_bco(63488);
    ledPRAC2.Set_font_color_pco(63488);
    PRACon = true;
}

void ledPRACoff(){
    ledPRAC.Set_background_color_bco(65535);
    ledPRAC.Set_font_color_pco(65535);
    ledPRAC2.Set_background_color_bco(65535);
    ledPRAC2.Set_font_color_pco(65535);
    PRACon = false;
}

void ledLOADon(){
    ledLOAD.Set_background_color_bco(63488);
    ledLOAD.Set_font_color_pco(63488);
    ledLOAD2.Set_background_color_bco(63488);
    ledLOAD2.Set_font_color_pco(63488);
    LOADon = true;
}

void ledLOADoff(){
    ledLOAD.Set_background_color_bco(65535);
    ledLOAD.Set_font_color_pco(65535);
    ledLOAD2.Set_background_color_bco(65535);
    ledLOAD2.Set_font_color_pco(65535);
    LOADon = false;
}

void ledPAUSEon(){
    boop_beep();
    ledPAUSE.Set_background_color_bco(63488);
    ledPAUSE.Set_font_color_pco(63488);
    ledPAUSE2.Set_background_color_bco(63488);
    ledPAUSE2.Set_font_color_pco(63488);
    PAUSEon = true;
}

void ledPAUSEoff(){
    ledPAUSE.Set_background_color_bco(65535);
    ledPAUSE.Set_font_color_pco(65535);
    ledPAUSE2.Set_background_color_bco(65535);
    ledPAUSE2.Set_font_color_pco(65535);
    PAUSEon = false;
}

void ledSENDon(){
    ledSEND.Set_background_color_bco(63488);
    ledSEND.Set_font_color_pco(63488);
    ledSEND2.Set_background_color_bco(63488);
    ledSEND2.Set_font_color_pco(63488);
    SENDon = true;
}

void ledSENDoff(){
    ledSEND.Set_background_color_bco(65535);
    ledSEND.Set_font_color_pco(65535);
    ledSEND2.Set_background_color_bco(65535);
    ledSEND2.Set_font_color_pco(65535);
    SENDon = false;
}


/*
 * ===========================================================================================
 * Notification sounds functions
 * ===========================================================================================
 */

void beep(){
    toneAC(800, toneVol, 200);
}

void boop(){
    toneAC(400, toneVol, 200);
}

void beep_boop(){
  beep();
  delay(50);
  boop();
}

void boop_beep(){
  boop();
  delay(50);
  beep();
}

void warble (){
  int i = 0;
  do {
    toneAC(600, toneVol, 15);
    toneAC(1500, toneVol, 15);
    toneAC(800, toneVol, 15);
    toneAC(1200, toneVol, 15);
    i++;
    } while (i < 7);
}
