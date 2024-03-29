/*
 * This files holds definitions and functions that are specific to the Nextion display use in this project.
 */

// Declare your Nextion objects - Example (page id = 0, component id = 1, component name = "b0") 
NexButton AutoSleep = NexButton(0, 2, "AutoSleep");
NexButton TUNE = NexButton(1, 20, "TUNE");
NexButton PRAC = NexButton(1, 16, "PRAC");
NexButton RPT = NexButton(1, 11, "RPT");
NexButton WT = NexButton(1, 6, "WT");
NexButton PC = NexButton(1, 1, "PC");
NexButton LOAD = NexButton(1, 2, "LOAD");
NexButton SPACING = NexButton(1, 15, "SPACING");
NexButton WPM = NexButton(1, 10, "WPM");
NexButton SEND = NexButton(1, 4, "SEND");
NexButton STOP = NexButton(1, 5, "STOP");
NexCheckbox Backlight = NexCheckbox(2, 25, "Backlight");
NexText BLText = NexText(2, 1, "BLText");
NexButton SLEEP0 = NexButton(2, 22, "SLEEP0");
NexButton SLEEP1 = NexButton(2, 2, "SLEEP1");
NexButton SLEEP30 = NexButton(2, 8, "SLEEP30");
NexButton SLEEP2 = NexButton(2, 9, "SLEEP2");
NexButton SLEEP5 = NexButton(2, 10, "SLEEP5");
NexButton SLEEP10 = NexButton(2, 24, "SLEEP10");
NexButton RPT2 = NexButton(4, 55, "RPT2");
NexButton PRAC2 = NexButton(4, 59, "PRAC2");
NexButton PCb = NexButton(4, 58, "PCb");
NexButton LOAD2 = NexButton(4, 57, "LOAD2");
NexButton SEND2 = NexButton(4, 56, "SEND2");
NexButton STOP2 = NexButton(4, 54, "STOP2");
NexSlider WPMSlider = NexSlider(1, 32, "WPMSlider");
NexProgressBar Prog = NexProgressBar(1, 31, "Prog");
NexButton _0 = NexButton(1, 3, "_0");
NexButton _1 = NexButton(1, 7, "_1");
NexButton _2 = NexButton(1, 8, "_2");
NexButton _3 = NexButton(1, 9, "_3");
NexButton _4 = NexButton(1, 12, "_4");
NexButton _5 = NexButton(1, 13, "_5");
NexButton _6 = NexButton(1, 14, "_6");
NexButton _7 = NexButton(1, 17, "_7");
NexButton _8 = NexButton(1, 18, "_8");
NexButton _9 = NexButton(1, 19, "_9");
NexButton MUTE = NexButton(1, 34, "MUTE");
NexButton RANGE = NexButton(1, 33, "RANGE");
NexPage Splash = NexPage(0, 0, "Splash");
NexPicture SplashPic = NexPicture(0, 1, "SplashPic");
NexNumber n0 = NexNumber(1, 38, "n0");
NexNumber n3 = NexNumber(1, 39, "n3");
NexNumber n4 = NexNumber(1, 40, "n4");
NexNumber n5 = NexNumber(1, 46, "n5");
NexRadio ledNORM = NexRadio(1, 25, "ledNORM");
NexRadio ledPRAC = NexRadio(1, 21, "ledPRAC");
NexRadio ledLOAD = NexRadio(1, 22, "ledLOAD");
NexRadio ledPAUSE = NexRadio(1, 23, "ledPAUSE");
NexRadio ledSEND = NexRadio(1, 24, "ledSEND");
NexRadio ledNORM2 = NexRadio(4, 48, "ledNORM2");
NexRadio ledPRAC2 = NexRadio(4, 44, "ledPRAC2");
NexRadio ledLOAD2 = NexRadio(4, 45, "ledLOAD2");
NexRadio ledPAUSE2 = NexRadio(4, 46, "ledPAUSE2");
NexRadio ledSEND2 = NexRadio(4, 47, "ledSEND2");
NexText EnterText = NexText(1, 41, "EnterText");
NexText callText = NexText(1, 47, "callText");
NexText callText4 = NexText(4, 93, "callText4");
NexText EnterText4 = NexText(4, 92, "EnterText4");
NexText qwertyText = NexText(4, 60, "qwertyText");
NexText kybdText = NexText(1, 45, "kybdText");
NexSlider mode_slider = NexSlider(3, 17, "mode_slider"); //30 MAR 20 WW9JD Error discovered & Helped troubleshooting N4TVC
NexSlider PitchSlider = NexSlider(3, 1, "PitchSlider");
NexSlider VolSlider = NexSlider(3, 5, "VolSlider");
NexSlider T_Slider = NexSlider(3, 10, "T_Slider"); //30 MAR 20 WW9JD Error discovered & Helped troubleshooting N4TVC
NexSlider PTTSlider = NexSlider(3, 14, "PTTSlider"); //30 MAR 20 WW9JD Error discovered & Helped troubleshooting N4TVC
NexButton resetKeyer = NexButton(3, 4, "resetKeyer");
NexDSButton txtDsp = NexDSButton(3, 23, "txtDsp");
NexDSButton iMode = NexDSButton(3, 24, "iMode");
NexButton resetI = NexButton(3, 38, "resetI");
NexButton bZero = NexButton(3, 39, "bZero");
NexDSButton bqMode = NexDSButton(3, 40, "bqMode");
NexText mode_text = NexText(3, 19, "mode_text");
NexNumber Pitch = NexNumber(3, 2, "Pitch");
NexNumber Vol = NexNumber(3, 6, "Vol");
NexNumber Time = NexNumber(3, 9, "Time");
NexNumber Weight = NexNumber(3, 13, "Weight");
NexNumber PTT = NexNumber(3, 13, "PTT");
NexNumber n0t = NexNumber(3, 25, "n0t");
NexNumber n2t = NexNumber(3, 28, "n2t");
NexNumber n6t = NexNumber(3, 30, "n6t");
NexNumber n7t = NexNumber(3, 31, "n7t");
NexNumber n8t = NexNumber(3, 34, "n8t");
NexNumber n9t = NexNumber(3, 36, "n9t");
NexRadio r0 = NexRadio(3, 26, "r0");
NexRadio r1 = NexRadio(3, 27, "r1");
NexRadio r2 = NexRadio(3, 29, "r2");
NexRadio r3 = NexRadio(3, 32, "r3");
NexRadio r4 = NexRadio(3, 33, "r4");
NexRadio r5 = NexRadio(3, 35, "r5");
NexButton First_1 = NexButton(1, 55, "First_1");
NexButton First_2 = NexButton(2, 28, "First_2");
NexButton First_3 = NexButton(3, 43, "First_3");
NexButton First_4 = NexButton(4, 102, "First_4");
NexButton First_5 = NexButton(5, 23, "First_5");
NexButton Home_1 = NexButton(1, 56, "Home_1");
NexButton Home_2 = NexButton(2, 27, "Home_2");
NexButton Home_3 = NexButton(3, 42, "Home_3");
NexButton Home_4 = NexButton(4, 101, "Home_4");
NexButton Home_5 = NexButton(5, 22, "Home_5");
NexButton qwerty_1 = NexButton(1, 43, "qwerty_1");
NexButton qwerty_2 = NexButton(2, 26, "qwerty_2");
NexButton qwerty_3 = NexButton(3, 41, "qwerty_3");
NexButton qwerty_4 = NexButton(4, 100, "qwerty_4");
NexButton qwerty_5 = NexButton(5, 21, "qwerty_5");
NexButton Keyer_1 = NexButton(1, 53, "Keyer_1");
NexButton Keyer_2 = NexButton(2, 29, "Keyer_2");
NexButton Keyer_3 = NexButton(3, 44, "Keyer_3");
NexButton Keyer_4 = NexButton(4, 103, "Keyer_4");
NexButton Keyer_5 = NexButton(5, 24, "Keyer_5");
NexButton Settings_1 = NexButton(1, 54, "Settings_1");
NexButton Settings_2 = NexButton(2, 30, "Settings_2");
NexButton Settings_3 = NexButton(3, 45, "Settings_3");
NexButton Settings_4 = NexButton(4, 104, "Settings_4");
NexButton Settings_5 = NexButton(5, 25, "Settings_5");
NexButton Refs_1 = NexButton(1, 44, "Refs_1");
NexButton Refs_2 = NexButton(2, 31, "Refs_2");
NexButton Refs_3 = NexButton(3, 46, "Refs_3");
NexButton Refs_4 = NexButton(4, 105, "Refs_4");
NexButton Refs_5 = NexButton(5, 26, "Refs_5");
NexButton bsplash = NexButton(2, 12, "bsplash");
NexButton bref = NexButton(2, 8, "bref");
NexButton bendFile = NexButton(4, 81, "bendFile");
NexButton nMode= NexButton(4, 99, "nMode"); // 5 DEC 2020 WW9JD Added for Contest Mode
NexScrolltext ErrTxt = NexScrolltext(1, 42, "ErrTxt");
NexButton _Z = NexButton(4, 1, "_Z");
NexButton BX = NexButton(4, 2, "BX"); //The Arduino compiler, for the SparkFun BlackBoard Artemis Nano board did not like my keys naming convention for some keys
                                      //so I changed the leading underscore to a "B"
                                      //It only effected about 8 letter keys, and I couldn't determine the problem quickly, so I just switched
NexButton BC = NexButton(4, 3, "BC");
NexButton _V = NexButton(4, 4, "_V");
NexButton BB = NexButton(4, 5, "BB");
NexButton BN = NexButton(4, 10, "BN");
NexButton _M = NexButton(4, 9, "_M");
NexButton COM = NexButton(4, 8, "COM");
NexButton PER = NexButton(4, 7, "PER");
NexButton BAR = NexButton(4, 6, "BAR");
NexButton _A = NexButton(4, 11, "_A");
NexButton BS = NexButton(4, 12, "BS");
NexButton _D = NexButton(4, 13, "_D");
NexButton _F = NexButton(4, 14, "_F");
NexButton _G = NexButton(4, 15, "_G");
NexButton _H = NexButton(4, 16, "_H");
NexButton _J = NexButton(4, 17, "_J");
NexButton _K = NexButton(4, 18, "_K");
NexButton BL = NexButton(4, 19, "BL");
NexButton _QM = NexButton(4, 20, "_QM");
NexButton _Q = NexButton(4, 30, "_Q");
NexButton _W = NexButton(4, 29, "_W");
NexButton _E = NexButton(4, 28, "_E");
NexButton _R = NexButton(4, 27, "_R");
NexButton _T = NexButton(4, 26, "_T");
NexButton _Y = NexButton(4, 25, "_Y");
NexButton BU = NexButton(4, 24, "BU");
NexButton _I = NexButton(4, 23, "_I");
NexButton _O = NexButton(4, 22, "_O");
NexButton BP = NexButton(4, 21, "BP");
NexButton b_1 = NexButton(4, 40, "b_1");
NexButton b_2 = NexButton(4, 39, "b_2");
NexButton b_3 = NexButton(4, 38, "b_3");
NexButton b_4 = NexButton(4, 37, "b_4");
NexButton b_5 = NexButton(4, 36, "b_5");
NexButton b_6 = NexButton(4, 35, "b_6");
NexButton b_7 = NexButton(4, 34, "b_7");
NexButton b_8 = NexButton(4, 33, "b_8");
NexButton b_9 = NexButton(4, 32, "b_9");
NexButton b_0 = NexButton(4, 31, "b_0");
NexButton error = NexButton(4, 43, "error");
NexButton WORD = NexButton(4, 41, "WORD");
NexButton BT = NexButton(4, 42, "BT");
NexButton b0 = NexButton(4, 61, "b0");
NexButton b1 = NexButton(4, 62, "b1");
NexButton b2 = NexButton(4, 63, "b2");
NexButton b3 = NexButton(4, 64, "b3");
NexButton b4 = NexButton(4, 65, "b4");
NexButton b5 = NexButton(4, 66, "b5");
NexButton b6 = NexButton(4, 67, "b6");
NexButton b7 = NexButton(4, 68, "b7");
NexButton b8 = NexButton(4, 69, "b8");
NexButton b9 = NexButton(4, 70, "b9");
NexButton b10 = NexButton(4, 71, "b10");
NexButton b11 = NexButton(4, 72, "b11");
NexButton b12 = NexButton(4, 73, "b12");
NexButton b13 = NexButton(4, 74, "b13");
NexButton b14 = NexButton(4, 75, "b14");
NexButton b15 = NexButton(4, 76, "b15");
NexButton b16 = NexButton(4, 77, "b16");
NexButton b17 = NexButton(4, 78, "b17");
NexButton b18 = NexButton(4, 79, "b18");
NexButton b19 = NexButton(4, 80, "b19");
NexButton b20 = NexButton(4, 82, "b20");
NexButton b21 = NexButton(4, 83, "b21");
NexButton b22 = NexButton(4, 84, "b22");
NexButton b23 = NexButton(4, 85, "b23");
NexButton b24 = NexButton(4, 86, "b24");
NexButton b25 = NexButton(4, 87, "b25");
NexButton b26 = NexButton(4, 88, "b26");
NexButton b27 = NexButton(4, 79, "b27");
NexButton b28 = NexButton(4, 90, "b28");
NexButton b29 = NexButton(4, 91, "b29");
NexNumber n0q = NexNumber(4, 95, "n0q");
NexNumber n1q = NexNumber(4, 97, "n1q");
NexButton bSerial = NexButton(4, 98, "bSerial");



// Register a button object to the touch event list.  
NexTouch *nex_listen_list[] = {
  &AutoSleep,
  &TUNE,
  &PRAC,
  &RPT,
  &WT,
  &PC,
  &LOAD,
  &SPACING,
  &WPM,
  &SEND,
  &STOP,
  &Backlight,
  &BLText,
  &SLEEP0,
  &SLEEP1,
  &SLEEP30,
  &SLEEP2,
  &SLEEP5,
  &SLEEP10,
  &RPT2,
  &PRAC2,
  &PCb,
  &LOAD2,
  &SEND2,
  &STOP2,
  &WPMSlider,
  &_0,
  &_1,
  &_2,
  &_3,
  &_4,
  &_5,
  &_6,
  &_7,
  &_8,
  &_9,
  &MUTE,
  &RANGE,
  &Splash,
  &SplashPic,
  &n0,
  &n3,
  &n4,
  &n5,
  &ledNORM,
  &ledPRAC,
  &ledLOAD,
  &ledPAUSE,
  &ledSEND,
  &ledNORM2,
  &ledPRAC2,
  &ledLOAD2,
  &ledPAUSE2,
  &ledSEND2,
  &EnterText,
  &callText,
  &callText4,
  &EnterText4,
  &mode_slider,
  &PitchSlider,
  &resetKeyer,
  &txtDsp,
  &iMode,
  &resetI,
  &bZero,
  &bqMode,
  &n0t,
  &n2t,
  &n6t,
  &n7t,
  &n8t,
  &n9t,
  &r0,
  &r1,
  &r2,
  &r3,
  &r4,
  &r5,
  &First_1,
  &First_2,
  &First_3,
  &First_4,
  &First_5,
  &Home_1,
  &Home_2,
  &Home_3,
  &Home_4,
  &Home_5,
  &qwerty_1,
  &qwerty_2,
  &qwerty_3,
  &qwerty_4,
  &qwerty_5,
  &Keyer_1,
  &Keyer_2,
  &Keyer_3,
  &Keyer_4,
  &Keyer_5,
  &Settings_1,
  &Settings_2,
  &Settings_3,
  &Settings_4,
  &Settings_5,
  &Refs_1,
  &Refs_2,
  &Refs_3,
  &Refs_4,
  &Refs_5,
  &bsplash,
  &bref,
  &mode_text,
  &VolSlider,
  &T_Slider,
//  &W_Slider,
  &PTTSlider,
  &bendFile,
  &nMode,
  &_Z,
  &BX,  
  &BC,  
  &_V,  
  &BB,  
  &BN,  
  &_M,  
  &COM, 
  &PER, 
  &BAR, 
  &_A,  
  &BS,  
  &_D,  
  &_F,  
  &_G,  
  &_H,  
  &_J,  
  &_K,  
  &BL,  
  &_QM, 
  &_Q,  
  &_W,  
  &_E,  
  &_R,  
  &_T,  
  &_Y,  
  &BU,  
  &_I,  
  &_O,  
  &BP,  
  &b_1, 
  &b_2, 
  &b_3, 
  &b_4, 
  &b_5, 
  &b_6, 
  &b_7, 
  &b_8, 
  &b_9, 
  &b_0, 
  &error, 
  &WORD,  
  &BT,  
  &qwertyText,
  &kybdText,
  &b0,
  &b1,
  &b2,
  &b3,
  &b4,
  &b5,
  &b6,
  &b7,
  &b8,
  &b9,
  &b10,
  &b11,
  &b12,
  &b13,
  &b14,
  &b15,
  &b16,
  &b17,
  &b18,
  &b19,
  &b20,
  &b21,
  &b22,
  &b23,
  &b24,
  &b25,
  &b26,
  &b27,
  &b28,
  &b29,
  &n0q,
  &bSerial,
  NULL
};
