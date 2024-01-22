// ########### LOCOMOTIVES ###########
// ROSTER(999,"Loco Name","F0/F1/*F2/F3/F4/F5/F6/F7/F8")
// Skipped Vossloh's functions: F8 (Diesel Notch Up), F9 (Diesel Notch Down), F11 (Horn LO Short), F12 (Horn HI Short)
ROSTER(4,"Vossloh 1701","Lights/Engine Sound/*Horn LO/*Horn HI/Coupler/Ventilator/Shunting mode/Compressor///Sanding///*Compressed Air/Brake/Interior light")
ALIAS(VOSSLOH, 4)
ALIAS(VOSSLOH_ENGINE_SOUND_F, 1)
ALIAS(VOSSLOH_HORN_LO_F, 2)
ALIAS(VOSSLOH_HORN_HI_F, 3)
ALIAS(VOSSLOH_INTERIOR_LIGHT_F, 15)
ROSTER(5,"SBB Cargo Re 482","Lights")
ALIAS(CARGO, 5)

// ########### SENSORS ###########
ALIAS(SNS1, 164)
ALIAS(SNS2, 165)

// ########### SIGNALS ###########
SIGNAL(100, 101, 102)
ALIAS(SIGNAL_1, 100)

// ########### TURNOUTS ###########
// TURNOUT( id, addr, sub_addr [, "description"] )
// Take the address and divide by 4. The result is the address, the remainder is the subaddress.
// For example, Address 51 (no subaddress) is DCC++ Address 12, Subaddress 3.
TURNOUT(101, 1, 0, "Near wall turnout")
ALIAS(TURNOUT_WALL, 101)
TURNOUT(102, 1, 1, "Near edge turnout")
ALIAS(TURNOUT_EDGE, 102)

// ########### STARTUP ###########

POWERON         // turn on track power
// START(201)      // Start test sequence 201
DONE            // End of startup sequence


// ########### SEQUENCES ############
// Test SNS1 and SIGNAL_1
SEQUENCE(201)
  // SETLOCO(VOSSLOH)
  // AT(SNS1)
  //   FON(VOSSLOH_INTERIOR_LIGHT_F)
  // AT(-SNS1)
  //   FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  GREEN(SIGNAL_1)
  AT(SNS1)
    AMBER(SIGNAL_1)
    DELAY(500)
    RED(SIGNAL_1)
  AT(-SNS1)
    AMBER(SIGNAL_1)
    DELAY(500)
    GREEN(SIGNAL_1)
  FOLLOW(201)


// Test signals
SEQUENCE(202)
  RED(SIGNAL_1)
  DELAY(500)
  AMBER(SIGNAL_1)
  DELAY(500)
  GREEN(SIGNAL_1)
  DELAY(500)
  FOLLOW(202)


// TODO: find a way to make this work
// When turnout 101 is thrown, also throw turnout 102
// ONTHROW(101) THROW(102)
// When turnout 101 is closed, also close turnout 102
// ONCLOSE(101) CLOSE(102)


// ########### AUTOMATIONS ###########
AUTOMATION(300,"Round in circles SNS1")
  FWD(40)
  DELAY(5000)
  STOP
  DELAY(3000)
  REV(30)
  AT(SNS1)
  STOP
  DELAYRANDOM(3000, 6000)
  FOLLOW(300) // and continue to follow the automation



/* ########### NOTES ############
To test PA0 input of expander, send this command to Serial port:
<S 2 164 1>
Then it must return messages depending on the state of the input:
00:50:22.425 > <Q 2>
00:50:23.145 > <q 2>
*/
