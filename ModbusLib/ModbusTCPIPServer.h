#ifndef MODBUSTCPIPSERVER
#define MODBUSTCPIPSERVER

#include "ModbusServer.h"

class ModbusTCPIPServer : public ModbusServer{
 public:
  ModbusTCPIPServer(MemInterface* mem);

  unsigned int processModbusRequest(byte_t* mb_req, unsigned int len, byte_t* mb_rep = 0);
 private:
};


#endif

