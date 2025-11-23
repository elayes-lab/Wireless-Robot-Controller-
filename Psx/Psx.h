/*  PSX Controller Decoder Library
	Modified by: 	Fabio Angeletti May 22, 2014
					email: fabio.angeletti89@gmail.com

	Added functionality based on this work:
	http://store.curiousinventor.com/guides/PS2/
	
	The following is based on:
		PSX Controller Decoder Library
		Written by: Kevin Ahrendt June 22nd, 2008
	
	Controller protocol implemented using Andrew J McCubbin's analysis.
	http://www.gamesx.com/controldata/psxcont/psxcont.htm
	
	Shift command is based on tutorial examples for ShiftIn and ShiftOut
	functions both written by Carlyn Maw and Tom Igoe
	http://www.arduino.cc/en/Tutorial/ShiftIn
	http://www.arduino.cc/en/Tutorial/ShiftOut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef Psx_h
#define Psx_h

#include "Arduino.h"

class Psx
{
	public:
		Psx();

		void setupPins(byte dataPin, byte cmndPin, byte attPin, byte clockPin, byte delay);
		// (Data Pin #, CMND Pin #, ATT Pin #, CLK Pin #, Delay)
		// Delay is how long the clock goes without changing state
		// in Microseconds. It can be lowered to increase response,
		// but if it is too low it may cause glitches and have some
		// keys spill over with false-positives. A regular PSX controller
		// works fine at 50 uSeconds.
															
		void read();										// Reads the controller status

		uint8_t psxID, psxStatus;
		uint8_t psxThumbL, psxThumbR;
		uint8_t psxJoyRX, psxJoyRY, psxJoyLX, psxJoyLY;

	private:
		byte shift(byte _dataOut);

		byte _dataPin;
		byte _cmndPin;
		byte _attPin;
		byte _clockPin;
		
		byte _delay;
		byte _i;
		boolean _temp;
		byte _dataIn;
		byte _dataOut;
};

#endif
