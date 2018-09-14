/*
    ModbusTCP.h - Header for Modbus IP Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "mbed.h"
#include "Modbus.h"
#include "NetworkInterface.h"
#include "CBuffer.h"

#ifndef MODBUSTCP_H
#define MODBUSTCP_H

#define MODBUSTCP_PORT 	  502
#define MODBUSTCP_MAXFRAME 200
#define MODBUSTCP_BUFFER_SIZE 64

//#define TCP_KEEP_ALIVE

class ModbusTCP : public Modbus {
    private:
        NetworkInterface* network;
        TCPServer* _server;
        TCPSocket* _socket;
        SocketAddress* _address;
        byte _MBAP[7];

    public:
        ModbusTCP(NetworkInterface* _net);
        void server_open(uint16_t port);
        void server_run();
        void server_stop();
};

#endif //MODBUSIP_H

