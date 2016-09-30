/*
 * mcp4921.c:
 *	Extend wiringPi with the MCP4921 SPI Digital to Analog convertor
 *	Copyright (c) 2016 Craig Robbins
 *	Copyright (c) 2012-2013 Gordon Henderson
 *
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

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "mcp4921.h"

static void DACwrite(struct wiringPiNodeStruct *node, int pin, int value)
{
	unsigned char spiData[2];

	// channel/buff/gain/shdn/d11/d10/d9/d8
	// channel is ignored for the MPC4921
	spiData[0] = 0b00010000 | ((value & 0xf00) >> 8);
	// Set buff and gain as supplied to the setup function
	spiData[0] |= node->data0;
	// d7-d0
	spiData[1] = value & 0xff;

	wiringPiSPIDataWrite(node->fd, spiData, 2) ;
}


/*!
 * @param bufferedMode 0 = No, anything else = Yes (Use DAC's buffered mode)
 * @param useGain 0 = No, anything else = Yes (2x gain for output voltage)
 */
int mcp4921Setup(int pinBase, int spiChannel, int bufferedMode, int useGain)
{
	struct wiringPiNodeStruct *node;

	if (wiringPiSPISetup (spiChannel, 2000000) < 0)
		return FALSE;

	node = wiringPiNewNode (pinBase, 1);

	node->fd = spiChannel;
	node->analogWrite = DACwrite;

	/* Use data0 to store whether or not to use the DAC's buffered mode and
	 * gain. The bits are stored in data0 in the same order and
	 * position that the SPI write command will require; i.e. when writing
	 * these can simply be added to the first command byte using an OR.
	 */
	node->data0 = (bufferedMode != 0) << 6;
	node->data0 |= (useGain != 0) << 5;

	return TRUE;
}
