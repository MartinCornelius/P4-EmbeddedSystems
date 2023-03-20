/* Measures brightness in a room in the unit Lux every minute, 
 * if the brightness is higher than 10% and the light is on it toggles off the LED, 
 * if brightness is lower than 10% and light is off it toggles on the LED.
 */

define MAX_LUMEN 1000
define MIN_LUMEN 0.1
define LUX_TO_PERCENT 100 / 1000
define MS_PER_MIN 1000 * 60

setup()
{
  input  lightSensor  = 0x300;
  output lightLED = 0x301; 

  bool isOn = false;
  float16 lightIn; /* 8 bits range ~ 0.0078 to 480. lux range ~ 0.1 to 1000. */
  uint8 lightPercent; /* goes from 0 to 100 % technically only 7 bits needed */
  uint8 brightnessThreshold = 10;

  ioperm(0x300, 2, 1); /* permission to use IO ports 0x300 and 0x301 true */
}

mainloop()
{
	inb(lightPort -> lightIn); /* Should be 2 bytes but idk */
	LuxToPercent(lightIn -> lightPercent);
	if(lightPercent > brightnessThreshold && isOn == false)
  {
	  outb(1, lightLED); /* assume 0x0 means turn off and 0x1 means turn on for this LED */
	}
	else if(lightPercent < brightnessThreshold && isOn == true)
  {
	  outb(0, lightLED);
	}
	delay(MS_PER_MIN); /* waits 1 minute between each check */
}

func LuxToPercent(float16 lux -> uint8 intensityPercent)
{
  if(lux > MAX_LUMEN) {
    lux = MAX_LUMEN;
  }
  else if(lux < MIN_LUMEN){
    lux = MIN_LUMEN;
  }	
  Floor(lux * LUX_TO_PERCENT -> lux);
  ToInt8(lux -> intensityPercent);
}