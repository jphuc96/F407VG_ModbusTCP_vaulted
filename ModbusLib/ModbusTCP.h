/*
    ModbusTCP.h - Header for Modbus IP Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "mbed.h"
#include "rtos.h"
#include "Modbus.h"
#include "NetworkInterface.h"
#include "CBuffer.h"

#ifndef MODBUSTCP_H
#define MODBUSTCP_H

#define MODBUSTCP_PORT 	  502
#define MODBUSTCP_MAXFRAME 200
#define MODBUSTCP_BUFFER_SIZE 64
#define MODBUS_MAX_CLIENTS 3
#define MODBUS_STACK_SIZE 1024*6

//#define TCP_KEEP_ALIVE

class ModbusTCP : public Modbus {

    public:
        typedef struct
        {
            Thread* thread;
            TCPSocket* socket;
        } MBstate;

        ModbusTCP(NetworkInterface* _net);
        static MBstate _state[MODBUS_MAX_CLIENTS];
        void server_start(uint16_t port);
        static void daemon ();
        static void child (void const *arg);

        static ModbusTCP * getInstance() {
        return _instance;
    };

    private:
        NetworkInterface* network;
        static ModbusTCP *_instance;
        static TCPServer* _server;
        static TCPSocket* _socket[MODBUS_MAX_CLIENTS];
        static SocketAddress* _address;
        Thread* _daemon;
        byte _recv_buffer[MODBUSTCP_BUFFER_SIZE];
        byte* _send_buffer;
        byte _MBAP[7];

        
};

#endif //MODBUSIP_H

