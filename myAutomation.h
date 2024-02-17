/* ########### NOTES ############
- To test PA0 input of expander, send this command to Serial port:
    <S 3 179 1>
    Then it must return messages depending on the state of the input:
    00:50:22.425 > <Q 2>
    00:50:23.145 > <q 2>
- Default volume by CV:
  63 - 64 or (main volume: 64 - 100%, 20 - 31%)
  121 - 64 (low horn)
  122 - 64
  123 - 64 (engine sound)

  // Test L_EDGE and SIGNAL_1
  SEQUENCE(201)
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
*/

// ##################################
// ########### CONFIGURATION #########
// ##################################

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
ALIAS(VOSSLOH_VENTILATOR_F, 5)
ALIAS(VOSSLOH_HORN_LO_SHORT_F, 11)
ALIAS(VOSSLOH_HORN_HI_SHORT_F, 12)
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
ALIAS(TURNOUT_WALL, 101)
TURNOUT(TURNOUT_WALL, 1, 0, "Near wall turnout")
ALIAS(TURNOUT_EDGE, 102)
TURNOUT(TURNOUT_EDGE, 1, 1, "Near edge turnout")



// ##################################
// ########### SEQUENCES ############
// ##################################

// ########### TURNOUT SEQUENCES ###########
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

// ########### HORN SEQUENCES ###########
ALIAS(HORN_VOSSLOH_HI_SHORT)
SEQUENCE(HORN_VOSSLOH_HI_SHORT)
  FON(VOSSLOH_HORN_HI_SHORT_F)  DELAY(1000)  FOFF(VOSSLOH_HORN_HI_SHORT_F)
  RETURN
ALIAS(HORN_VOSSLOH_HI_LONG)
SEQUENCE(HORN_VOSSLOH_HI_LONG)
  FON(VOSSLOH_HORN_HI_F)  DELAY(1500)  FOFF(VOSSLOH_HORN_HI_F)
  RETURN
ALIAS(HORN_VOSSLOH_LOW_SHORT)
SEQUENCE(HORN_VOSSLOH_LOW_SHORT)
  FON(VOSSLOH_HORN_LO_SHORT_F)  DELAY(1000)  FOFF(VOSSLOH_HORN_LO_SHORT_F)
  RETURN
ALIAS(HORN_VOSSLOH_LOW_LONG)
SEQUENCE(HORN_VOSSLOH_LOW_LONG)
  FON(VOSSLOH_HORN_LO_F)  DELAY(1500)  FOFF(VOSSLOH_HORN_LO_F)
  RETURN

// ########### MANEUVER SEQUENCES ###########
ALIAS(VOSSLOH_COLD_START)
SEQUENCE(VOSSLOH_COLD_START)
  // Train driver comes in
  FON(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(1500)
  FON(VOSSLOH_BRAKE_F)
  DELAY(4000)
  // Start the engine
  FON(VOSSLOH_ENGINE_SOUND_F)
  DELAY(5000)
  // Lights on
  FON(VOSSLOH_LIGHTS_F)
  DELAY(2000)
  FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(2000)
  RETURN

ALIAS(VOSSLOH_FULL_STOP)
SEQUENCE(VOSSLOH_FULL_STOP)
  FOFF(VOSSLOH_ENGINE_SOUND_F)
  DELAY(5000)
  FON(VOSSLOH_INTERIOR_LIGHT_F)
  DELAY(5000)
  FOFF(VOSSLOH_LIGHTS_F)
  DELAY(5000)
  FOFF(VOSSLOH_INTERIOR_LIGHT_F)
  RETURN

ALIAS(VOSSLOH_PREPARE_TO_MOVE)
SEQUENCE(VOSSLOH_PREPARE_TO_MOVE)
  FOFF(VOSSLOH_BRAKE_F)
  DELAY(4000)
  CALL(HORN_VOSSLOH_HI_SHORT)
  DELAY(2000)
  RETURN

ALIAS(VOSSLOH_ENGAGE_BRAKES)
SEQUENCE(VOSSLOH_ENGAGE_BRAKES)
  FON(VOSSLOH_BRAKE_F)
  DELAY(4000)
  RETURN

ALIAS(VOSSLOH_SET_FORWARD_DIRECTION)
SEQUENCE(VOSSLOH_SET_FORWARD_DIRECTION)
  FWD(0)
  DELAY(2000)
  RETURN

ALIAS(VOSSLOH_SET_REVERSE_DIRECTION)
SEQUENCE(VOSSLOH_SET_REVERSE_DIRECTION)
  REV(0)
  DELAY(2000)
  RETURN

ALIAS(CLOSE_TURNOUTS)
SEQUENCE(CLOSE_TURNOUTS)
  CLOSE(TURNOUT_WALL) DELAY(100) CLOSE(TURNOUT_EDGE)
  DELAY(2000)
  RETURN

ALIAS(THROW_TURNOUTS)
SEQUENCE(THROW_TURNOUTS)
  THROW(TURNOUT_WALL) DELAY(100) THROW(TURNOUT_EDGE)
  DELAY(2000)
  RETURN

ALIAS(SBB_CARGO_PREPARE_TO_MOVE)
SEQUENCE(SBB_CARGO_PREPARE_TO_MOVE)
  SENDLOCO(VOSSLOH, HORN_VOSSLOH_LOW_SHORT)
  DELAY(2000)
  RETURN


// ##################################
// ########### AUTOMATIONS ###########
// ##################################

AUTOMATION(301,"Test Route 301")
  CALL(HORN_VOSSLOH_HI_SHORT)
  DELAY(1000)
  SENDLOCO(VOSSLOH, HORN_VOSSLOH_LOW_LONG)
  DONE

AUTOMATION(300,"Vossloh Route 300")
  SET_TRACK(A, MAIN)
  SET_TRACK(B, MAIN)
  POWERON
  DELAY(3000)
  // STARTUP
  SETLOCO(VOSSLOH)
  CALL(VOSSLOH_COLD_START)
  // Set turnouts
  CALL(THROW_TURNOUTS)
  // Prepare to move
  CALL(VOSSLOH_PREPARE_TO_MOVE)

  // ###### Go till the right edge of the layout
  FWD(40)
  ATTIMEOUT(R_EDGE, 18000) // Use timeout to avoid accidents
  DELAY(600)
  STOP
  DELAY(4000)
  CALL(VOSSLOH_ENGAGE_BRAKES)
  // Set turnouts
  CALL(CLOSE_TURNOUTS)
  CALL(VOSSLOH_SET_REVERSE_DIRECTION)
  CALL(VOSSLOH_PREPARE_TO_MOVE)

  // ###### Go till the left edge of the layout
  REV(48)
  ATTIMEOUT(L_EDGE, 16000)
  STOP
  DELAY(7000)
  CALL(VOSSLOH_ENGAGE_BRAKES)
  // Change lights direction
  CALL(VOSSLOH_SET_FORWARD_DIRECTION)
  // Sound the LOW horn to notify that SBB Cargo is leaving
  CALL(HORN_VOSSLOH_LOW_LONG)
  DELAY(3000)
  // FON(VOSSLOH_VENTILATOR_F)

  // ###### SBB Cargo goes to the left
  SETLOCO(CARGO)
  FWD(34)
  ATTIMEOUT(L_WALL, 10000)
  DELAY(100)
  STOP
  DELAY(5000)
  // Set turnouts
  CALL(THROW_TURNOUTS)
  // Change lights direction
  REV(0)
  DELAY(2000)
  CALL(SBB_CARGO_PREPARE_TO_MOVE)
  REV(30)
  ATTIMEOUT(R_EDGE, 15000)
  // Delay a little bit to move loco further
  DELAY(500)
  STOP
  DELAY(5000)
  // Change lights direction
  FWD(0)
  DELAY(1000)
  CALL(SBB_CARGO_PREPARE_TO_MOVE)
  // Go back to the left
  FWD(30)
  ATTIMEOUT(L_WALL, 15000)
  // Delay a little bit to move loco further
  DELAY(400)
  STOP
  DELAY(5000)
  CALL(CLOSE_TURNOUTS)
  // Change lights direction
  REV(0)
  DELAY(1000)
  CALL(SBB_CARGO_PREPARE_TO_MOVE)
  // Go to the home
  REV(33)
  ATTIMEOUT(R_WALL, 12000)
  STOP
  DELAY(6000)
  // Change lights direction
  FWD(0)
  // SENDLOCO(VOSSLOH, VOSSLOH_VENTILATOR_F)
  DELAY(2000)

  // ###### Switch back to Vossloh
  SETLOCO(VOSSLOH)
  CALL(VOSSLOH_PREPARE_TO_MOVE)
  // Go till the right edge of the layout again
  FWD(50)
  ATTIMEOUT(R_EDGE, 15000)
  STOP
  DELAY(6000)
  CALL(VOSSLOH_ENGAGE_BRAKES)
  // Set turnouts
  CALL(THROW_TURNOUTS)
  // Change lights direction
  CALL(VOSSLOH_SET_REVERSE_DIRECTION)
  CALL(VOSSLOH_PREPARE_TO_MOVE)

  // ###### Go to the home
  REV(40)
  ATTIMEOUT(L_WALL, 18000)
  // Delay a little bit to move loco further
  DELAY(550)
  STOP
  DELAY(6000)
  CALL(VOSSLOH_ENGAGE_BRAKES)
  CALL(VOSSLOH_SET_FORWARD_DIRECTION)
  DELAY(3000)

  // ###### Full stop
  CALL(VOSSLOH_FULL_STOP)
  DONE

