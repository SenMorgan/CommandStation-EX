// ROSTER(999,"Loco Name","F0/F1/*F2/F3/F4/F5/F6/F7/F8")
ROSTER(3,"Vossloh 1701","Lights/Engine Sound/*Whistle LO/*Whistle HI/Coupler/Ventilator/Shunting mode/Compressor/Diesel Notch Up/Diesel Notch Down/Sanding/*Whistle LO Short/*Whistle HI Short/*Compressed Air/Brake/Interior light")

// TURNOUT( id, addr, sub_addr [, "description"] )
// Take the address and divide by 4. The result is the address, the remainder is the subaddress.
// For example, Address 51 (no subaddress) is DCC++ Address 12, Subaddress 3.
TURNOUT(101, 1, 0, "Near edge turnout")
TURNOUT(102, 1, 1, "Near wall turnout")

// AUTOMATIONS
// TODO: find a way to make this work
// When turnout 101 is thrown, also throw turnout 102
// ONTHROW(101) THROW(102)
// When turnout 101 is closed, also close turnout 102
// ONCLOSE(101) CLOSE(102)

// AUTOMATIONS in the Throttle menu
AUTOMATION(201,"Round in circles")
  FWD(10)
  DELAY(5000)
  STOP
  DELAYRANDOM(1000,10000)
  REV(10)
  DELAY(5000)
  STOP
  FOLLOW(201) // and continue to follow the automation