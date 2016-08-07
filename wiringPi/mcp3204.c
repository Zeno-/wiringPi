/*
 * mcp3204.c:
 *	Extend wiringPi with the MCP3204 SPI Analog to Digital convertor
 *	Copyright (c) 2012-2013 Gordon Henderson
 *
 *	Thanks also to "ShorTie" on IRC for some remote debugging help!
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

/* Craig Robbins (Zeno-): Aug-2016, Modified 3004.c/.h to support MCP3204 */

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "mcp3204.h"

/*
 * myAnalogRead:
 *	Return the analog value of the given pin
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  unsigned char spiData [3] ;
  int chan = pin - node->pinBase ;

  /* D2 is a "don't care" bit for the MCP3204 */
  spiData [0] = 0b00000110 | ((chan & 4) >> 2); // 0,0,0,0,0,StartBit,SGL/DIFF,D2*
  spiData [1] = (chan & 3) << 6;                // D1, D0, x, x, x, x, x, x
  spiData [2] = 0;

  wiringPiSPIDataRW (node->fd, spiData, 3) ;

  return ((spiData [1] << 8) | spiData [2]) & 0xFFF ;
}


/*
 * mcp3204Setup:
 *	Create a new wiringPi device node for an mcp3204 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int mcp3204Setup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;

  if (wiringPiSPISetup (spiChannel, 1000000) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, 8) ;

  node->fd         = spiChannel ;
  node->analogRead = myAnalogRead ;

  return TRUE ;
}
