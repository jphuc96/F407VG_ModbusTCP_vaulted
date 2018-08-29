/* Copyright (C) 2013 Hiroshi Suga, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ModBus-TCP.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>



int Modbus::getHandler (int startRegister) {
    int i;

    for (i = 0; i < _handler_count; i ++) {
        if (_handler[i].startRegister == startRegister) {
            // found
            return i;
        }
    }
    return -1;
}


int Modbus::attachRead (void (*funcCgi)(int, unsigned short, unsigned short), int startRegister) {
    if (_handler_count < MODBUS_MAX_HANDLES) {
        _handler[_handler_count].startRegister = startRegister;
        _handler[_handler_count].funcCgiRead = funcCgi;
        _handler_count ++;
        return 0;
    } else {
        return -1;
    }
}


int Modbus::send (int id, unsigned short buffer[], int len) {
    
    char out[MODBUS_BUF_SIZE];
     
    
    // Build MBAP Header
    //Transaction Number :: Copy from Org message
    out[0] = _state[id].start_message[0];
    out[1] = _state[id].start_message[1];
    
    //Protcoll Identifier :: Always 0x0000
    out[2] = 0x00;
    out[3] = 0x00;
    
    // Length (bytes to follow after length bytes) of TCP Frame
    out[4] = (char)(((len*2)+3)>> 8);
    out[5] = (char)(((len*2)+3) & 0x00ff);
    
    //unit identifier (normally 0xff, but 0x00 is allowed
    out[6] = 0xff;
    
    //MBAP Header build finished
    
    // Now the ModBus response
    //Function code :: Copy from org message
    out[7] = _state[id].start_message[7];
    
    //Byte count of Modbus response
    out[8] = len*2;
    
    for(int i=0; i<len; i++) {
        out[(i*2)+9] = (char)(buffer[i] >> 8);  
        out[(i*2)+10] = (char)(buffer[i] & 0x00ff);  
    }
    
   
    
    DBG("sending anser on child %i", id);
    return _state[id].client->send(out, (len*2)+9);
}

int Modbus::sendToClient(int id, char buffer[], unsigned short len) {
        return _state[id].client->send(buffer, len);
 }
 

int Modbus::from_hex (int ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}
 
int Modbus::to_hex (int code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}
