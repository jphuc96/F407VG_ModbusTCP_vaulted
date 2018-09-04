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

#ifndef MODBUSSERVER_H
#define MODBUSSERVER_H

//declaration of all the considered Modbus functions
#define MB_FCT_READ_DSCR_OUTPUTS        0x01
#define MB_FCT_READ_DSCR_INPUTS         0x02
#define MB_FCT_READ_HOLDING_REG         0x03
#define MB_FCT_READ_INPUT_REG           0x04
#define MB_FCT_WRITE_SING_OUTPUT        0x05
#define MB_FCT_WRITE_SING_REG           0x06
#define MB_FCT_WRITE_MULT_COILS         0x0F
#define MB_FCT_WRITE_MULT_REGS          0x10

//declaration of all the considered Modbus exceptions
#define MB_EXC_ILLEGAL_FUNCTION         0x01
#define MB_EXC_ILLEGAL_DATA_ADDRESS     0x02
#define MB_EXC_ILLEGAL_DATA_VALUE       0x03
#define MB_EXC_SLAVE_FAILURE            0x04

#include "MemInterface.h"               //to deal with the considered memory


/***
 * The ModbusServer class allows to deal with Modbus pdu (protocol data unit).
 * This implementation is based on the Modbus_Application_Protocol_V1_1b.pdf
 * document (http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf).
 * It requires a memory (an implementation of the MemInterface class).
 * Note that the bit_t, byte_t, word_t types are defined in the MemInterface
 * interface
 ***/
class ModbusServer
{
    public:
        //a memory is needed in the constructor
        //MemInterface* memory : an implementation of the MemInterface interface
        //  (meminterface.h)
        ModbusServer(MemInterface* memory);
        ~ModbusServer();

        //function that processes the request modbus PDU
        //byte_t* mb_pdu :          the modbus request
        //unsigned int pdu_len :    the size of the modbus request (in bytes)
        //byte_t* mb_rsp :       the modbus response (if not defined, the modbus 
        //                              request is used to store the response)
        //return the number of bytes of the modbus response.
        unsigned int processPDU(byte_t* mb_pdu, unsigned int pdu_len,
                                byte_t* mb_rsp = 0);
    protected:
        //function 0x01
        unsigned int readDiscreteOutputs(byte_t* mb_pdu, unsigned int pdu_len,
                                            byte_t* mb_rsp);
        //function 0x02
        unsigned int readDiscreteInputs(byte_t* mb_pdu, unsigned int pdu_len,
                                        byte_t* mb_rsp);
        //function 0x03
        unsigned int readHoldingRegisters(byte_t* mb_pdu, unsigned int pdu_len,
                                          byte_t* mb_rsp);
        //function 0x04
        unsigned int readInputRegisters(byte_t* mb_pdu, unsigned int pdu_len,
                                        byte_t* mb_rsp);
        //function 0x05
        unsigned int writeSingleOutput(byte_t* mb_pdu, unsigned int pdu_len,
                                        byte_t* mb_rsp);
        //function 0x06
        unsigned int writeSingleRegister(byte_t* mb_pdu, unsigned int pdu_len,
                                        byte_t* mb_rsp);
        //function 0x0F
        unsigned int writeMultipleOutputs(byte_t* mb_pdu, unsigned int pdu_len,
                                            byte_t* mb_rsp);
        //function 0x10
        unsigned int writeMultipleRegisters(byte_t* mb_pdu,unsigned int pdu_len,
                                            byte_t* mb_rsp);
        //handeling the exception responses
        unsigned int mb_exception(byte_t* mb_rsp, byte_t code);
        //To deal with the memory
        MemInterface* _memory;
};

#endif // MODBUSSERVER_H
