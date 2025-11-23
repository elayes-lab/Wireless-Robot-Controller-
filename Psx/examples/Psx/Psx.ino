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

#include <Psx.h>                                          // Includes the Psx Library 
                                                          // Any pins can be used since it is done in software
#define dataPin 4
#define cmndPin 3
#define attPin 2
#define clockPin 5

Psx Psx;                                                  // Initializes the library

byte buffer[8];
uint8_t index;

void setup()
{
	Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10);  // Defines what each pin is used
                                                          // (Data Pin #, Cmnd Pin #, Att Pin #, Clk Pin #, Delay)
                                                          // Delay measures how long the clock remains at each state,
                                                          // measured in microseconds.
                                                          // too small delay may not work (under 5)
	Serial.begin(57600);
}

void loop()
{
	Psx.read();                                      

 	if(Psx.psxID != 0x00)
	{
		buffer[0] = Psx.psxID;
		buffer[1] = Psx.psxStatus;
		buffer[2] = Psx.psxThumbL;
		buffer[3] = Psx.psxThumbR;
		buffer[4] = Psx.psxJoyRX;
		buffer[5] = Psx.psxJoyRY;
		buffer[6] = Psx.psxJoyLX;
		buffer[7] = Psx.psxJoyLY;

		for(index = 0; index<8; index++)
		{
			Serial.print(buffer[index], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}

	delay(50);
}
