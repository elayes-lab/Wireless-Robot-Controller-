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


#include "Psx.h"

Psx::Psx()
{
	psxID = 0x00;
	psxStatus = 0x00;
	psxThumbL = 0x00;
	psxThumbR = 0x00;
	psxJoyRX = 0x00;
	psxJoyRY = 0x00;
	psxJoyLX = 0x00;
	psxJoyLY = 0x00;
}

byte Psx::shift(byte _dataOut)							// Does the actual shifting, both in and out simultaneously
{
	_temp = 0;
	_dataIn = 0;

	for (_i = 0; _i <= 7; _i++)
	{
		if ( _dataOut & (1 << _i) ) digitalWrite(_cmndPin, HIGH);	// Writes out the _dataOut bits
		else digitalWrite(_cmndPin, LOW);

		digitalWrite(_clockPin, LOW);
		
		delayMicroseconds(_delay);

		_temp = digitalRead(_dataPin);					// Reads the data pin

		if (_temp)
			_dataIn = _dataIn | (B10000000 >> _i);		// Shifts the read data into _dataIn

		digitalWrite(_clockPin, HIGH);
		delayMicroseconds(_delay);
	}
	return _dataIn;
}

void Psx::setupPins(byte dataPin, byte cmndPin, byte attPin, byte clockPin, byte delay)
{
	pinMode(dataPin, INPUT);
	digitalWrite(dataPin, HIGH);	// Turn on internal pull-up
	_dataPin = dataPin;

	pinMode(cmndPin, OUTPUT);
	_cmndPin = cmndPin;

	pinMode(attPin, OUTPUT);
	_attPin = attPin;
	digitalWrite(_attPin, HIGH);

	pinMode(clockPin, OUTPUT);
	_clockPin = clockPin;
	digitalWrite(_clockPin, HIGH);
	
	_delay = delay;
}


void Psx::read()
{
	digitalWrite(_attPin, LOW);
	delay(1);
 		
	shift(0x01);
	psxID = shift(0x42);
	psxStatus = shift(0x00);
	psxThumbL = shift(0x00);
	psxThumbR = shift(0x00);
	psxJoyRX = shift(0x00);
	psxJoyRY = shift(0x00);
	psxJoyLX = shift(0x00);
	psxJoyLY = shift(0x00);

	delay(1);
	digitalWrite(_attPin, HIGH);
}
