/***
 * This file is part of the ModbusServer (V0.1).
 *
 * Copyright (C) 2016 - Remy Guyonneau - remy.guyonneau(at)univ-angers.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "ModbusServer.h"

//a memory is needed in the constructor
//MemInterface* memory : an implementation of the MemInterface interface
//  (meminterface.h)
ModbusServer::ModbusServer(MemInterface* memory){
    _memory = memory;
}

ModbusServer::~ModbusServer(){
    //dtor
}

//function that processes the request modbus PDU
//byte_t* mb_pdu :          the modbus request
//unsigned int pdu_len :    the size of the modbus request (in bytes)
//byte_t* mb_rsp :          the modbus response (if not defined, the modbus
//                              request is used to store the response)
//return the number of bytes of the modbus response.
unsigned int ModbusServer::processPDU(byte_t* mb_pdu, unsigned int pdu_len,
                                        byte_t* mb_rsp){
    if(pdu_len == 0){
        //the modbus pdu is empty, nothing to be done
        return 0;
    }
    if(mb_rsp == 0){
        //The request and the response are the same array
        mb_rsp = mb_pdu;
    }else{
        //else we copy the function number
        //  (needed for exception response for instance)
        mb_rsp[0] = mb_pdu[0];
    }
    //we test the request function code
    switch(mb_pdu[0]){
        case MB_FCT_READ_DSCR_OUTPUTS:                  //0x01
            return readDiscreteOutputs(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_READ_DSCR_INPUTS:                   //0x02
            return readDiscreteInputs(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_READ_HOLDING_REG:                   //0x03
            return readHoldingRegisters(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_READ_INPUT_REG:                     //0x04
            return readInputRegisters(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_WRITE_SING_OUTPUT:                  //0x05
            return writeSingleOutput(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_WRITE_SING_REG:                     //0x06
            return writeSingleRegister(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_WRITE_MULT_COILS:                   //0x0F
            return writeMultipleOutputs(mb_pdu, pdu_len, mb_rsp);
            break;
        case MB_FCT_WRITE_MULT_REGS:                    //0x10
            return writeMultipleRegisters(mb_pdu, pdu_len, mb_rsp);
            break;
        default:
            //the function is not handled
            return mb_exception(mb_rsp, MB_EXC_ILLEGAL_FUNCTION);
    }
}

//processing of the exception response
unsigned int ModbusServer::mb_exception(byte_t* mb_rsp, byte_t code){
    mb_rsp[0] |= 0x80;               //set the most significant bit to 1
    mb_rsp[1] = code;                //set the corresponding error code
    return 2;                        //return the size of the response
}

// ----------------------------------------------------------------------
//Note that in all the following functions:
//byte_t* mb_pdu :          the modbus pdu request
//unsigned int pdu_len :    the modbus pdu request lenght
//byte_t* mb_rsp :          the modbus pdu response
// ----------------------------------------------------------------------


// ----------------------------FUNCTION 0x01 ----------------------------
//  Read Coils

unsigned int ModbusServer::readDiscreteOutputs(byte_t* mb_pdu,
                                        unsigned int pdu_len, byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Starting Address
    //2 bytes Quantity of Coils
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityOutputs = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityOutputs < 0x0001 || quantityOutputs > 0x07D0){
        //quantity of requested output is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidDiscreteOutput(startingAdd) ||
            !_memory->isValidDiscreteOutput(startingAdd+quantityOutputs)){
        //the addresses of the requested outputs are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    //i.e. we get the value of all the requested bits and
    //put them in the response array
    bit_t data[quantityOutputs];
    if(! _memory->readDiscreteOutputs(startingAdd,quantityOutputs,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    byte_t byte = 0;
    unsigned int bit = 0;
    //to undestand how the bits are stored in the response, check the
    //modbus application protocol
    for(int i=0; i<quantityOutputs; i++){
        if (bit==0){
            //we are starting a new byte, it is needed to be initialized to 0
            mb_rsp[2+byte] = 0x00;
            byte++;
        }
        mb_rsp[1+byte] |= ( (byte_t) data[i] << bit);
        bit++;
        if(bit == 8){
            //we need to start a new byte to store the next bit data
            bit = 0;
        }
    }
    mb_rsp[1] = byte;
    return (byte + 2);  //1 byte function code, 1 byte Byte count, N bytes
                        //  Coil Status
                        //*N = Quantity of Coils / 8 (+1 if Qty%8 != 0)
}

// ----------------------------FUNCTION 0x02 ----------------------------
//  Read Discrete Inputs

unsigned int ModbusServer::readDiscreteInputs(byte_t* mb_pdu,
                                        unsigned int pdu_len, byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Starting Address
    //2 bytes Quantity of Inputs
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityInputs = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityInputs < 0x0001 || quantityInputs > 0x07D0){
        //quantity of requested inputs is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidDiscreteInput(startingAdd) ||
                !_memory->isValidDiscreteInput(startingAdd+quantityInputs)){
        //the addresses of the requested inputs are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    //i.e. we get the value of all the requested bits and
    //put them in the response array
    bit_t data[quantityInputs];
    if(! _memory->readDiscreteInputs(startingAdd,quantityInputs,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    byte_t byte = 0;
    unsigned int bit = 0;
    for(int i=0; i<quantityInputs; i++){
        if (bit==0){
            //we are starting a new byte, it is needed to be initialized to 0
            mb_rsp[2+byte] = 0x00;
            byte++;
        }
        mb_rsp[1+byte] |= ( (unsigned char) data[i] << bit);
        bit++;
        if(bit == 8){
            //we need to start a new byte to store the bit data
            bit = 0;
        }
    }
    mb_rsp[1] = byte;
    return (byte + 2);  //1 byte function code, 1 byte Byte count,
                        //  N bytes Inputs Status
                        //*N = Quantity of Inputs / 8 (+1 if Qty%8 != 0)
}

// ----------------------------FUNCTION 0x03 ----------------------------
//  Read Holding Registers

unsigned int ModbusServer::readHoldingRegisters(byte_t* mb_pdu,
                                        unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Starting Address
    //2 bytes Quantity of Registers
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityRegisters = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityRegisters < 0x0001 || quantityRegisters > 0x007D){
        //quantity of requested registers is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidHoldingRegister(startingAdd) ||
            !_memory->isValidHoldingRegister(startingAdd+quantityRegisters)){
        //the addresses of the requested registers are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    //i.e. we get the value of all the requested registers and
    //put them in the response array
    word_t data[quantityRegisters];
    if(! _memory->readHoldingRegisters(startingAdd,quantityRegisters,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    for(int i=0; i<quantityRegisters; i++){
        mb_rsp[2+i*2] = (byte_t)((data[i] & 0xFF00) >> 8);
        mb_rsp[2+i*2+1] = (byte_t)(data[i] & 0x00FF);
    }
    mb_rsp[1] = quantityRegisters*2;
    return (quantityRegisters*2 + 2);   //1 byte function code,
                                    //1 byte Byte count, N bytes Register Values
                                    // N = Quantity of Registers *2
}

// ----------------------------FUNCTION 0x04 ----------------------------
//  Read Input Registers

unsigned int ModbusServer::readInputRegisters(byte_t* mb_pdu,
                                        unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Starting Address
    //2 bytes Quantity of Input Registers
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityRegisters = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityRegisters < 0x0001 || quantityRegisters > 0x007D){
        //quantity of requested registers is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidInputRegister(startingAdd) ||
                !_memory->isValidInputRegister(startingAdd+quantityRegisters)){
        //the addresses of the requested registers are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    //i.e. we get the value of all the requested registers and
    //put them in the response array
    word_t data[quantityRegisters];
    if(! _memory->readInputRegisters(startingAdd,quantityRegisters,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    for(int i=0; i<quantityRegisters; i++){
        mb_rsp[2+i*2] = (byte_t)((data[i] & 0xFF00) >> 8);
        mb_rsp[2+i*2+1] = (byte_t)(data[i] & 0x00FF);
    }
    mb_rsp[1] = quantityRegisters*2;
    return (quantityRegisters*2 + 2);   //1 byte function code, 1 byte Byte
                                        //      count, N bytes Register Values
                                        // N = Quantity of Registers *2
}

// ----------------------------FUNCTION 0x05 ----------------------------
//  Write Single Coil

unsigned int ModbusServer::writeSingleOutput(byte_t* mb_pdu,
                                        unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Output Address
    //2 bytes Output Value
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t outputValue = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if( outputValue != 0x0000 && outputValue != 0xFF00){
        //output value not valide
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t address = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidDiscreteOutput(address)){
        //the address of the requested output is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    bit_t val;
    outputValue == 0x0000 ? val = 0 : val = 1;  //convert the requested value
                                                //  into bit_t
    if(!_memory->writeSingleOutput(address, val)){
        //Something went wrong while writing the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    //format the response (must be the same as the request)
    if(mb_rsp != mb_pdu){
        for(int i=1; i< 5; i++){
            mb_rsp[i] = mb_pdu[i];
        }
    }
    return 5;
}

// ----------------------------FUNCTION 0x06 ----------------------------
//  Write Single Register

unsigned int ModbusServer::writeSingleRegister(byte_t* mb_pdu,
                                          unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte Function code
    //2 bytes Register Address
    //2 bytes Register Value
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t registerValue = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if( registerValue < 0x0000 || registerValue > 0xFFFF){
        //register value not valide
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t address = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidHoldingRegister(address)){
        //the address of the requested register is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    word_t val = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];

    if(!_memory->writeHoldingRegister(address, val)){
        //Something went wrong while writing the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    //format the response (must be the same as the request)
    if(mb_rsp != mb_pdu){
        for(int i=1; i< 5; i++){
            mb_rsp[i] = mb_pdu[i];
        }
    }
    return 5;
}


// ----------------------------FUNCTION 0x0F ----------------------------
// Write Multiple Coils

unsigned int ModbusServer::writeMultipleOutputs(byte_t* mb_pdu,
                                        unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte    :   Function code
    //2 bytes   :   Starting Address
    //2 bytes   :   Quantity of Outputs
    //1 byte    :   byte count
    //N bytes   :   Outputs value
    if(pdu_len < 5){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityOutputs = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityOutputs < 0x0001 || quantityOutputs > 0x07B0){
        //quantity of requested output is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }

    //we computer the number of bytes needed the store the bits
    byte_t n = quantityOutputs / 8;
    if( quantityOutputs%8 != 0){
        n++;
    }
    //and we check if it is consistant with the pdu
    if((n != mb_pdu[5]) || (pdu_len < 6+mb_pdu[5])){
        //the count in the pdu is not valid
        //or the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }

    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidDiscreteOutput(startingAdd)
            || !_memory->isValidDiscreteOutput(startingAdd+quantityOutputs)){
        //the addresses of the requested outputs are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    bit_t data[quantityOutputs];
    unsigned int bit=0;
    byte_t byte=0;
    //we format the requested values into the array data
    for(int i=0; i<quantityOutputs; i++){
        data[i] = ((mb_pdu[6+byte] >> bit) & 0x01);
        bit ++;
        if(bit == 8){
            bit = 0;
            byte ++;
        }
    }
    //we write the values
    if(! _memory->writeMultipleOutputs(startingAdd,quantityOutputs,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    //format the response (must be the same as the request)
    for(int i=1; i< 5; i++){
        mb_rsp[i] = mb_pdu[i];
    }
    return 5;
}

// ----------------------------FUNCTION 0x10 ----------------------------
//  Write Multiple registers

unsigned int ModbusServer::writeMultipleRegisters(byte_t* mb_pdu,
                                        unsigned int pdu_len,byte_t* mb_rsp){
    //request expected data:
    //1 byte    :   Function code
    //2 bytes   :   Starting Address
    //2 bytes   :   Quantity of Registers
    //1 byte    :   byte count
    //N bytes   :   Registers value
    if(pdu_len < 7){
        //the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    word_t quantityRegisters = ((word_t)(mb_pdu[3]) << 8) + mb_pdu[4];
    if(quantityRegisters < 0x0001 || quantityRegisters > 0x007B){
        //quantity of requested registers is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }
    byte_t n = quantityRegisters * 2;
    if( n != mb_pdu[5] || pdu_len < (6+mb_pdu[5])){
        //the count in the pdu is not valid
        //or the length of the request is not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_VALUE);     //0x03
    }

    word_t startingAdd = ((word_t)(mb_pdu[1]) << 8) + mb_pdu[2];
    if(!_memory->isValidHoldingRegister(startingAdd) ||
            !_memory->isValidHoldingRegister(startingAdd+quantityRegisters)){
        //the addresses of the requested outputs are not valid
        return mb_exception(mb_rsp, MB_EXC_ILLEGAL_DATA_ADDRESS);   //0x02
    }
    //the request is a valid request, we process it
    //we format the requested values into the array data
    word_t data[quantityRegisters];
    for(int i=0; i<quantityRegisters; i++){
        data[i] = (((word_t) mb_pdu[6 + i*2] ) << 8) + mb_pdu[6 + i*2 + 1];
    }
    //we write the values
    if(! _memory->writeMultipleRegisters(startingAdd,quantityRegisters,data)){
        //Something went wrong while reading the data
        return mb_exception(mb_rsp, MB_EXC_SLAVE_FAILURE);          //0x04
    }
    //format the response (must be the same as the request)
    for(int i=1; i< 5; i++){
        mb_rsp[i] = mb_pdu[i];
    }
    return 5;
}
