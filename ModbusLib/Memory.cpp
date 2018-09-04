/***
 * This file is part of the MemInterface interface implementation example
 * to use the ModbusServer class.
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

#include "Memory.h"

//constructor
//word_t nbCoils :      number of discrete inputs/outputs for the memory
//word_t nbRegisters :  number of inputs/outputs registers for the memory
Memory::Memory(){
    _min_address_coil = 0;
    _max_address_coil = NBCOILS;
    _min_address_reg = 0;
    _max_address_reg = NBREGISTERS;
    initMemory();   //to deal with tests while developping a client for instance
}

Memory::~Memory(){
    //dtor
}

//this function is meant to do some test while using this class
void Memory::initMemory(){
    for(unsigned int i=0; i< NBCOILS; i++){
        _coils[i] = false;
    }
    for(unsigned int i=0; i< NBREGISTERS; i++){
        _registers[i] = false;
    }
    pinMode(LEDY_PIN,OUTPUT); 
    pinMode(LEDG_PIN,OUTPUT); 
}

//update the memory according to the input
void Memory::updateInputs(){
    _registers[POT_MEM] = analogRead(POT_PIN);
}

//update the output according to the memory
void Memory::updateOutputs(){
  digitalWrite(LEDY_PIN, _coils[LEDY_MEM]); //set pin with the bit corresponding value
  digitalWrite(LEDG_PIN, _coils[LEDG_MEM]); //set pin with the bit corresponding value
  analogWrite(MOT_PIN, _registers[MOT_MEM]); //set analog output with the word corresponding value
}


//Test if the address corresponds to a correct discrete memory
//return true if the address is correct, false otherwise
//word_t i :    the address we want to test
bool Memory::isValidDiscreteOutput(word_t i) const{
    if(i>= _min_address_coil && i <=_max_address_coil){
        return true;
    }else{
        return false;
    }
}

//Test if the address corresponds to a correct register memory
//return true if the address is correct, false otherwise
//word_t i :    the address we want to test
bool Memory::isValidHoldingRegister(word_t i) const{
    if(i>= _min_address_reg && i <=_max_address_reg){
        return true;
    }else{
        return false;
    }
}

//Test if the address corresponds to a correct register memory
//return true if the address is correct, false otherwise
//word_t i :    the address we want to test
bool Memory::isValidInputRegister(word_t i) const{
    //in this test class, input and holding registers are the same
    return isValidHoldingRegister(i);
}

//Test if the address corresponds to a correct discrete memory
//return true if the address is correct, false otherwise
//word_t i :    the address we want to test
bool Memory::isValidDiscreteInput(word_t i) const{
    //in this test class, discrete inputs and outputs are the same
    return isValidDiscreteOutput(i);
}

//This function reads concecutives discrete outputs
//return true if the reading successed, false otherwise
//word_t startingAddress :  the address of the first bit we want to read
//word_t nbData :           the number of bits we want to read
//bit_t * data :            the array where we want to put the readed bits
bool Memory::readDiscreteOutputs(word_t startingAddress, word_t nbData,
                                     bit_t * data) const{
    for(int i=0; i<nbData; i++){
        data[i] = _coils[i+startingAddress];
    } 
    return true;
}

//This function reads concecutives discrete inputs
//return true if the reading successed, false otherwise
//word_t startingAddress :  the address of the first bit we want to read
//word_t nbData :           the number of bits we want to read
//bit_t * data :            the array where we want to put the readed bits
bool Memory::readDiscreteInputs(word_t startingAddress, word_t nbData,
                                    bit_t * data) const{
    //in this test class, discrete inputs and outputs are the same
    return readDiscreteOutputs(startingAddress, nbData, data);
}

//This function reads concecutives holding registers
//return true if the reading successed, false otherwise
//word_t startingAddress :  the address of the first register we want to read
//word_t nbData :           the number of registers we want to read
//word_t * data :           the array where we want to put the readed registers
bool Memory::readHoldingRegisters(word_t startingAddress, word_t nbData,
                                        word_t * data) const{
    for(int i=0; i<nbData; i++){
        data[i] = _registers[i+startingAddress];
    }
    return true;
}

//This function reads concecutives input registers
//return true if the reading successed, false otherwise
//word_t startingAddress :  the address of the first register we want to read
//word_t nbData :           the number of registers we want to read
//word_t * data :           the array where we want to put the readed registers
bool Memory::readInputRegisters(word_t startingAddress, word_t nbData,
                                    word_t * data) const{
    //in this test class, input and holding registers are the same
    return readHoldingRegisters(startingAddress, nbData, data);
}

//This function writes one discrete output
//return true if the writing successed, false otherwise
//word_t address :          the address of the bit we want to write
//bit_t value :             the value we want to write
bool Memory::writeSingleOutput(word_t address, bit_t value){
    _coils[address] = value;
    return true;
}

//This function writes one holding register
//return true if the writing successed, false otherwise
//word_t address :          the address of the register we want to write
//word_t value :            the value we want to write
bool Memory::writeHoldingRegister(word_t address, word_t value){
    _registers[address] = value;
    return true;
}

//This function writes concecutive discrete outputs
//return true if the writing successed, false otherwise
//word_t address :          the address of the first bit we want to write
//word_t nbData :           the number of bits we want to write
//bit_t* data :             the values we want to write
bool Memory::writeMultipleOutputs(word_t address, word_t nbData,
                                        bit_t* data){
    for(int i=0; i< nbData; i++){
        _coils[address+i] = data[i];
    }
    return true;
}

//This function writes concecutive holding registers
//return true if the writing successed, false otherwise
//word_t address :          the address of the first register we want to write
//word_t nbData :           the number of registers we want to write
//word_t* data :            the values we want to write
bool Memory::writeMultipleRegisters(word_t address, word_t nbData,
                                        word_t* data){
    for(int i=0; i< nbData; i++){
        _registers[address+i] = data[i];
    }
    return true;
}
