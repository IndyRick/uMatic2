# µMatic²
WW9JD Arduino µMatic² Memory Keyer <br>
<br>
<br>
*NOTE: To download ANY file
- click on the file on the list, above
- right-click on the Raw or Download button (whichever is presented)
- select Save File (Target) As
<br>
======================================================<br>
MAIN µMATIC² SKETCH SOURCE CODE (for use in Arduino IDE) <br>
Current version: 5 DEC 2020 "D"<br>
The letter version sounds when EEPROM is reset<br>
======================================================<br>
uMatic2_MEGA_2560_PRO.ino* - Main µMatic² sketch<br>
uMatic2.h* - Header file that contains Nextion display controls definitions<br>
<br>
<br>
======================================================<br>
MAIN µMATIC² SKETCH COMPILED BINARY CODE (for use with AVRDUDESS)<br>
  Please see WIKI for instructions<br>
======================================================<br>
uMatic2_MEGA_2560_PRO.ino.mega.hex* - Compiled binary file ready to upload (via USB) directly into MEGA_2560_PRO<br>
uMatic2_MEGA_2560_PRO.ino.with_bootloader.mega.hex* - Compiled binary file ready to upload (via ISP) directly into MEGA_2560_PRO<br>
<br>
<br>
======================================================<br>
NEXTION DISPLAY  <br>
Current version: 5 DEC 2020<br>
======================================================<br>
uMatic2.HMI* - is the UI creation file to use with the Nextion Editor to modify the µMatic² UI <br>
uMatic2.tft* - is the current compiled UI that you would copy to a µSD card, insert in the Nextion display, turn the power on, wait for the "successed" display, remove the µSD card, cycle the power to use the UI <br>
uMatic2FontsPics.zip* - is a ZIP file containing all of the fonts and pictures used to make the µMatic² UI<br>
NexHardware.cpp* - Nextion library files modified for this project<br>
NexConfig.h* - Nextion library files modified for this project<br>
<br>
<br>
======================================================<br>
MAIN µMATIC² SKETCH - Recent changes <br>
======================================================<br>
- Fixed an error where entering too many characters for a callsign corrupted EEPROM. Set 12 character limit. (10 MAY 2021) <br>
- Contest Mode permits SENDing any one of 10 memories with a single button tap (5 DEC 2020)<br>
- Added keyer modes of Single lever paddle and Single lever paddle Reversed. While you have always <br>
  been able to use a Single lever paddle (foregoing squeeze keying), now you can use a Dual lever <br>
  paddle the same way. (19 NOV 2020) <br>
- Fixed a behavior with the Bug mode that permitted sending dits with the dah lever depressed (19 NOV 2020) <br>  
- Added ability to send using Iambic Mode B, and made that setting sticky. (19 NOV 2020) <br>
- Added the ability of SENDing files without displaying the text. Text display was wrecking <br>
  the WPM & Farnsworth timing. The time it takes to communicate the letter to be displayed<br>
  on the Nextion via serial added a fixed delay that did not vary with WPM speed. The<br>
  Keyer Settings Page has added a two-state button that enables/disables text display. (1 NOV 2020)<br>
- Fixed a timimg error that was introduced with the previously added "Sticky" Sleep function  (23 OCT 2020)<br>
- Sticky MUTE button (maintains state upon reboot)<br>
- SPC'G permits setting > WPM (greater control of spacing between characters)<br>
- Added functions to permit recurring automatic Arduino and/or Nextion sleep <br>
- Sleep interval is indicated with button color change (active interval is yellow)<br>
- Sleep state (Nextion backlight & Arduino, or Backlight Only) is saved is upon reboot<br>
<br>
<br>
======================================================<br>
NEXTION DISPLAY - Recent changes   <br>
======================================================<br>
- Added a page that just displays the Day, Date, & Time. Accessed from Settings page (16 OCT 2021)<br>
- Tabbed page display on each page (other than the 1st) (5 DEC 2020)<br>
- Contest serial number display & status (5 DEC 2020)<br>
- QWERTY Page "Command Key" changes (5 DEC 2020)<br>
- The Keyer Settings Page has added a two-state button that sets Iambic Mode A/B. (19 NOV 2020)<br>
- The Keyer Mode slider has been modified to include Single lever and Sngl-Rev modes. (19 NOV 2020)<br>
- The Keyer Settings Page has added a two-state button that enables/disables text display. (1 NOV 2020)<br>
- Settings / Clock (minutes update resets seconds to zero)<br>
- Settings / Clock (display shows HH:MM:SS)<br>
- Settings / Sleep buttons to include OFF & 10 min<br>
- Settings / Sleep changes saved to Nextion EPROM<br>
<br>
<br>
<br>
<br>
