// ########### STARTUP ###########
AUTOSTART
  SET_TRACK(A, MAIN)
  SET_TRACK(B, MAIN)
  POWERON
DONE

// ########### LOCOMOTIVES ###########
// ROSTER(999,"Loco Name","F0/F1/*F2/F3/F4/F5/F6/F7/F8")
// Skipped Vossloh's functions: F8 (Diesel Notch Up), F9 (Diesel Notch Down), F11 (Horn LO Short), F12 (Horn HI Short)
ROSTER(4,"Vossloh 1701","Lights/Engine Sound/*Horn LO/*Horn HI/Coupler/Ventilator/Shunting mode/Compressor///Sanding///*Compressed Air/Brake/Interior light")
ALIAS(VOSSLOH, 4)
ALIAS(VOSSLOH_LIGHTS_F, 0)
ALIAS(VOSSLOH_ENGINE_SOUND_F, 1)
ALIAS(VOSSLOH_HORN_LO_F, 2)
ALIAS(VOSSLOH_HORN_HI_F, 3)
ALIAS(VOSSLOH_BRAKE_F, 14)
ALIAS(VOSSLOH_INTERIOR_LIGHT_F, 15)
ROSTER(5,"SBB Cargo Re 482","Lights")
ALIAS(CARGO, 5)

// ########### SENSORS ###########
ALIAS(R_EDGE, 164)  // Right side - edge
ALIAS(R_WALL, 165)  // Right side - wall
ALIAS(L_WALL, 178)  // Left side - wall
ALIAS(L_EDGE, 179)  // Left side - edge

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



// ########### SEQUENCES ############
// TODO: This is not working. The horn is not played.
// Sound the Vossloh's horn
// SEQUENCE(200)
//   FON(VOSSLOH_HORN_HI_F)  DELAY(300)  FOFF(VOSSLOH_HORN_HI_F)
// RETURN
// ALIAS(HORN_VOSSLOH_HI, 200)


// Test L_EDGE and SIGNAL_1
SEQUENCE(201)
  // SETLOCO(VOSSLOH)
  // AT(L_EDGE)
  //   FON(VOSSLOH_INTERIOR_LIGHT_F)
  // AT(-L_EDGE)
  //   FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  GREEN(SIGNAL_1)
  AT(L_EDGE)
    AMBER(SIGNAL_1)
    DELAY(500)
    RED(SIGNAL_1)
  AT(-L_EDGE)
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


// Mirror the state of TURNOUT_WALL to TURNOUT_EDGE.
// Send command a few times to the both turnouts to
// ensure they are in the correct state.
ONTHROW(TURNOUT_WALL)
  DELAY(500) THROW(TURNOUT_WALL) DELAY(500)
  THROW(TURNOUT_EDGE) DELAY(500)
  THROW(TURNOUT_EDGE) DELAY(500)
  THROW(TURNOUT_EDGE)
DONE
ONCLOSE(TURNOUT_WALL)
  DELAY(500) CLOSE(TURNOUT_WALL) DELAY(500)
  CLOSE(TURNOUT_EDGE) DELAY(500)
  CLOSE(TURNOUT_EDGE) DELAY(500)
  CLOSE(TURNOUT_EDGE)
DONE

// ########### AUTOMATIONS ###########
AUTOMATION(300,"Vossloh Route 300")
  DELAY(3000)
  // ###### Train driver comes in
  SETLOCO(VOSSLOH)
  FON(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(3000)
  FON(VOSSLOH_BRAKE_F)
  DELAY(4000)
  // Start the engine
  FON(VOSSLOH_ENGINE_SOUND_F)
  DELAY(5000)
  // Prepare to move
  FON(VOSSLOH_LIGHTS_F)
  DELAY(2000)
  FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(2000)
  FOFF(VOSSLOH_BRAKE_F)
  DELAY(3000)
  // Set turnouts
  THROW(TURNOUT_WALL)
  DELAY(2000)
  // Sound the horn
  FON(VOSSLOH_HORN_HI_F)  DELAY(300)  FOFF(VOSSLOH_HORN_HI_F)
  DELAY(2000)

  // ###### Go till the right edge of the layout
  FWD(40)
  ATTIMEOUT(R_EDGE, 18000) // Use timeout to avoid accidents
  DELAY(600)
  STOP
  DELAY(3000)
  // Set turnouts
  CLOSE(TURNOUT_WALL)
  DELAY(2000)
  // Change lights direction
  REV(0)
  DELAY(2000)
  // Sound the horn
  FON(VOSSLOH_HORN_HI_F)  DELAY(300)  FOFF(VOSSLOH_HORN_HI_F)
  DELAY(2000)

  // ###### Go till the left edge of the layout
  REV(48)
  ATTIMEOUT(L_EDGE, 16000)
  STOP
  DELAY(7000)
  // Sound the LOW horn to notify that SBB Cargo is leaving
  FON(VOSSLOH_HORN_LO_F)  DELAY(2000)  FOFF(VOSSLOH_HORN_LO_F)
  DELAY(2000)

  // ###### SBB Cargo goes to the left
  // Set turnouts
  CLOSE(TURNOUT_WALL)
  DELAY(3000)
  SETLOCO(CARGO)
  FWD(34)
  ATTIMEOUT(L_WALL, 10000)
  DELAY(100)
  STOP
  DELAY(5000)
  // Set turnouts
  THROW(TURNOUT_WALL)
  DELAY(3000)
  // Change lights direction
  REV(0)
  DELAY(2000)
  REV(30)
  ATTIMEOUT(R_EDGE, 15000)
  // Delay a little bit to move loco further
  DELAY(500)
  STOP
  DELAY(5000)
  // Change lights direction
  FWD(0)
  DELAY(2000)
  // Go back to the left
  FWD(30)
  ATTIMEOUT(L_WALL, 15000)
  // Delay a little bit to move loco further
  DELAY(400)
  STOP
  DELAY(5000)
  // Set turnouts
  CLOSE(TURNOUT_WALL)
  DELAY(3000)
  // Change lights direction
  REV(0)
  DELAY(2000)
  // Go to the home
  REV(33)
  ATTIMEOUT(R_WALL, 12000)
  STOP
  DELAY(5000)
  // Change lights direction
  FWD(0)

  // ###### Switch back to Vossloh
  SETLOCO(VOSSLOH)
  // Change lights direction
  FWD(0)
  DELAY(2000)
  // Sound the horn
  FON(VOSSLOH_HORN_HI_F)  DELAY(300)  FOFF(VOSSLOH_HORN_HI_F)
  DELAY(2000)
  // Go till the right edge of the layout again
  FWD(50)
  ATTIMEOUT(R_EDGE, 15000)
  STOP
  DELAY(5000)
  // Set turnouts
  THROW(TURNOUT_WALL)
  DELAY(2000)
  // Change lights direction
  REV(0)
  DELAY(2000)
  // Sound the horn
  FON(VOSSLOH_HORN_HI_F)  DELAY(300)  FOFF(VOSSLOH_HORN_HI_F)
  DELAY(2000)

  // ###### Go to the home
  REV(40)
  ATTIMEOUT(L_WALL, 18000)
  // Delay a little bit to move loco further
  DELAY(550)
  STOP
  DELAY(5000)

  // Proceed the end of route
  FON(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(3000)
  FON(VOSSLOH_BRAKE_F)
  DELAY(5000)
  FOFF(VOSSLOH_ENGINE_SOUND_F)
  DELAY(5000)
  FOFF(VOSSLOH_LIGHTS_F)
  DELAY(5000)
  FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  DONE



/* ########### NOTES ############
To test PA0 input of expander, send this command to Serial port:
<S 3 179 1>
Then it must return messages depending on the state of the input:
00:50:22.425 > <Q 2>
00:50:23.145 > <q 2>
*/
