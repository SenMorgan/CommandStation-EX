//  myHal.cpp file for customizing the DCC++ EX HAL.

// The #if directive prevent compile errors for Uno and Nano by excluding the
//  HAL directives from the build.
#if !defined(IO_NO_HAL)

// Include devices you need.
#include "IODevice.h"

void halSetup() {
  //=======================================================================
  // The following directive defines a PCA9685 PWM Servo driver module.
  //=======================================================================
  // The parameters are:
  //   First Vpin=100
  //   Number of VPINs=16 (numbered 100-115)
  //   I2C address of module=0x40
  PCA9685::create(100, 16, 0x40);


  //=======================================================================
  // The following directive defines an MCP23017 16-port I2C GPIO Extender module.
  //=======================================================================
  // Alternative form, which allows the INT pin of the module to request a scan
  // by pulling Arduino pin 40 to ground.  Means that the I2C isn't being polled
  // all the time, only when a change takes place. Multiple modules' INT pins
  // may be connected to the same Arduino pin.
  // The parameters are:
  //   First Vpin=164
  //   Number of VPINs=16 (numbered 164-179)
  //   I2C address of module=0x20
  MCP23017::create(164, 16, 0x20, 17);
  // The next expander if will be connecter with interrupt pin
  // MCP23017::create(180, 16, 0x21, 17);
}

#endif
