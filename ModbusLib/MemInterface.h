/***
 * This file is part of the ModbusServer (V0.1)
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

#ifndef MEMINTERFACE_H
#define MEMINTERFACE_H

/***
 * Here are redefined the types for:
 *  - bit (0 or 1),
 *  - byte (8 bits),
 *  - word (16 bits).
 * Regards to your system you may have to change those typedef in
 * order to be consistant with the previous values
 ***/

typedef bool bit_t;
typedef unsigned char byte_t;
typedef unsigned short word_t;

/***
 * This abstact class in an interface between the ModbusServer class and
 * the memory we want to deal with. You need to create your own Memory
 * class that derivates from MemInterface, by implementing all the
 * needed functions.
 ***/

class MemInterface
{
    public:
        MemInterface();
        ~MemInterface();

        //Return true if the i address is a valid address for the output
        //coils, false otherwise
        //word_t i:                 the address we want to test
        virtual bool isValidDiscreteOutput(word_t i) const = 0 ;

        //Return true if the i address is a valid address for the discrete
        //inputs, false otherwise
        //word_t i:                 the address we want to test
        virtual bool isValidDiscreteInput(word_t i) const = 0;

        //Return true if the i address is a valid address for the holding
        //registers, false otherwise
        //word_t i:                 the address we want to test
        virtual bool isValidHoldingRegister(word_t i) const = 0;

        //Return true if the i address is a valid address for the input
        //registers, false otherwise
        //word_t i:                 the address we want to test
        virtual bool isValidInputRegister(word_t i) const = 0;

        //This function reads nbData from the startingAddress in the discrete
        //outputs and put the reading result into data.
        //Return true if the reading is a success, false otherwise
        //word_t startingAddress:   first bit we want to read
        //word_t nbData:            number of concecutive bits we want to read
        //bit_t * data:             array where we want to save the readed bits
        virtual bool readDiscreteOutputs(word_t startingAddress, word_t nbData,
                                         bit_t * data) const = 0;

        //This function reads nbData from the startingAddress in the discrete
        //inputs and put the reading result into data.
        //Return true if the reading is a success, false otherwise
        //word_t startingAddress:   first bit we want to read
        //word_t nbData:            number of concecutive bits we want to read
        //bit_t * data:             array where we want to save the readed bits
        virtual bool readDiscreteInputs(word_t startingAddress, word_t nbData,
                                        bit_t * data) const = 0;

        //This function reads nbData from the startingAddress in the holding
        //registers and put the reading result into data.
        //Return true if the reading is a success, false otherwise
        //word_t startingAddress:   first register we want to read
        //word_t nbData:         number of concecutive registers we want to read
        //word_t * data:        array where we want to save the readed registers
        virtual bool readHoldingRegisters(word_t startingAddress, word_t nbData,
                                          word_t * data) const = 0;

        //This function reads nbData from the startingAddress in the inputs
        //registers and put the reading result into data.
        //Return true if the reading is a success, false otherwise
        //word_t startingAddress:   first register we want to read
        //word_t nbData:         number of concecutive registers we want to read
        //word_t * data:        array where we want to save the readed registers
        virtual bool readInputRegisters(word_t startingAddress, word_t nbData,
                                        word_t * data) const = 0;

        //This function writes one discrete output
        //Return true if the writing is a success, false otherwise
        //word_t address:           address of the bit we want to write
        //bit_t value:              value we want to write
        virtual bool writeSingleOutput(word_t address, bit_t value) = 0;

        //This function writes one holding register
        //Return true if the writing is a success, false otherwise
        //word_t address:           address of the register we want to write
        //word_t value:             value we want to write
        virtual bool writeHoldingRegister(word_t address, word_t value) = 0;

        //This function writes multiple concecutive discrete outputs
        //Return true if the writing is a success, false otherwise
        //word_t address:           address of the first output we want to write
        //word_t nbData:           number of concecutive output we want to write
        //bit_t* data:              the concecutive values we want to write
        virtual bool writeMultipleOutputs(word_t address, word_t nbData,
                                            bit_t* data) = 0;

        //This function writes multiple concecutive holding registers
        //Return true if the writing is a success, false otherwise
        //word_t address:         address of the first register we want to write
        //word_t nbData:         number of concecutive register we want to write
        //word_t* data:              the concecutive values we want to write
        virtual bool writeMultipleRegisters(word_t address, word_t nbData,
                                            word_t* data) = 0;
};

#endif // MEMINTERFACE_H
