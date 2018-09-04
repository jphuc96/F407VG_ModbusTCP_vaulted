#include "ModbusTCPIPServer.h"

ModbusTCPIPServer::ModbusTCPIPServer(MemInterface* mem):ModbusServer(mem)
{

}
unsigned int ModbusTCPIPServer::processModbusRequest(byte_t* mb_req,
				       unsigned int len, byte_t* mb_rep){
  int retval=0;
  if(len < 8){
    //this is not a valid ModBus TCPIP request
    return retval;
  }

  // Transaction Identifier
  // mb_req[0]   Identification of a MODBUS Request/
  // mb_req[1]   Response transaction

  // Protocol Identifier
  if(mb_req[2] != 0 || mb_req[3] != 0){
    //this is not a valid ModBus TCPIP request
    return retval;
  }

  // Lenght
  word_t len_in_rq = (mb_req[4] << 8) + (mb_req[5]);
  if(len_in_rq != (len-6)){
    //this is not a valid ModBus TCPIP request
    return retval;
  }

  if(mb_rep == 0){
    mb_rep = mb_req;
  }else{
    mb_rep[0] = mb_req[0];
    mb_rep[1] = mb_req[1];
    mb_rep[2] = 0;
    mb_rep[3] = 0;
    mb_rep[6] = mb_req[6];
  }

  int len_rep = processPDU(mb_req+7, len_in_rq-1 ,mb_rep+7);

  // +1 because of the unit identifier
  mb_rep[4] = (((len_rep+1) & 0xFF00) >> 8);
  mb_rep[5] = ((len_rep+1) & 0x00FF);

  return len_rep+7;
}
