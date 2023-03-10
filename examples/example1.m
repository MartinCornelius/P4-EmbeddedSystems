/*  Measures heat in Fahrenheit and calculates the heat capacity over 1 minutes of measurements,
 *  and prints the measurements and result in a terminal
 */

import <asmio.h>

define MS_PER_MIN 1000 * 60

func CalcHeatCapa(float16 tStart, float16 tEnd, float16 q, float16 m -> float16 result)
{
  result <- q / (m * (tEnd - tStart));
}

setup()
{
  input heatPort = 0x300;
/*output sysOut = stdout(); */

  float16 temperatureStart;
  float16 temperatureEnd;
  float16 result;
  float16 heaterStrength = 106.92; /* random heater power pr. second multiplied by a minute */
  float16 weight = 1.0;            /* 1L wattah 🧊🔥 */

  ioperm(0x300, 1, 1); // permission to use IO port 0x300 true
}

mainloop()
{
  inb(heatPort -> temperatureStart);
  delay(MS_PER_MIN);
  inb(heatPort -> temperatureEnd);
  CalcHeatCapa(temperatureStart, temperatureEnd, heaterStrength, weight -> result);
  PrintTerminal(“Heat capacity is: ”);
  PrintTerminalLine(result);
  EndMainLoop();
}