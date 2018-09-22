/*
    ModbusTCP.cpp - Source for Modbus IP Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "ModbusTCP.h"
extern Serial pc;
extern ModbusTCP MB;

ModbusTCP * ModbusTCP::_instance;
ModbusTCP::ModbusTCP(NetworkInterface* _net)
{
    _instance = this;
    memset(_state, 0, sizeof(_state));
    network = _net;
    _server = new TCPServer();    
}

void ModbusTCP::server_start(uint16_t port)
{
    for(int i=0; i<MODBUS_MAX_CLIENTS; i++ )
    {
        _state[i].socket = new TCPSocket();
        _state[i].thread = new Thread(osPriorityNormal, MODBUS_STACK_SIZE);
        _state[i].thread->start(callback(child,(void*)i));
    }

    _server->open(network);
    _server->bind(network->get_ip_address(),port);
    _server->listen(MODBUS_MAX_CLIENTS);
    _daemon = new Thread(osPriorityNormal, MODBUS_STACK_SIZE);
    _daemon->start(daemon);
}

void ModbusTCP::daemon()
{
    // nsapi_size_or_error_t _ret;
    ModbusTCP *modbus_tcp = ModbusTCP::getInstance();
    int t=0;
    for (;;)
    {
        if( t>= 0)
        {
            modbus_tcp->_server->accept(_state[t].socket,_address);
            pc.printf("Accept: %s:%d\r\r\n",_address->get_ip_address(),_address->get_port());
            modbus_tcp->_state[t].thread->signal_set(0x1);
        }
        t = -1;
        for (int i=0 ; i<MODBUS_MAX_CLIENTS ;i++)
        {
            if(modbus_tcp->_state[i].thread->get_state() == Thread::WaitingThreadFlag)
            {
                if(t < 0) t=i;
            }
        }
    
    }
}

void ModbusTCP::child(void const *arg)
{
    
    // while(true)
    // {
    //     int _i;
    //     _socket->set_blocking(true);
    //     _socket->set_timeout(15000);
    //     _ret = _socket->recv(_recv_buffer,MODBUSTCP_BUFFER_SIZE);
    //     _recv_buffer[_ret] = '\0';

    //     if(_ret < 0) {
    //         //Failed
    //         pc.printf("Modbus breaking r = %d\r\r\n", _ret);
    //         this->server_run();
    //         break;
    //     }
    //     if(_ret > 0) {
    //         //Successed
    //         // pc.printf("Recv: ");
    //         // for(int i=0;i<_ret;i++) pc.printf("%.2x  ",_recv_buffer[i]);
    //         // pc.printf("\r\r\n");

    //         for(_i=0;_i<7;_i++) _MBAP[_i] = _recv_buffer[_i];

    //         _len = _MBAP[4] << 8 | _MBAP[5];
    //         _len--;  // Do not count with last byte from MBAP
    //     }
    //     if (_MBAP[2] !=0 || _MBAP[3] !=0) return;   //Not a MODBUSIP packet
    //     if (_len > MODBUSTCP_MAXFRAME) return;      //Length is over MODBUSTCP_MAXFRAME

    //     _recv_frame = (byte*)malloc(_len);
    //     for(_i=0;_i<_len;_i++) _recv_frame[_i] = _recv_buffer[_i+7];

    //     this->receivePDU(_recv_frame);
    //     if (_reply != MB_REPLY_OFF) {
    //         //MBAP
    //         _MBAP[4] = (_len+1) >> 8;     //_len+1 for last byte from MBAP
    //         _MBAP[5] = (_len+1) & 0x00FF;

    //         _send_buffer = (byte*) malloc(7 + _len);

    //         //Write MBAP
    //         for (_i = 0 ; _i < 7 ; _i++) _send_buffer[_i] = _MBAP[_i];
    //         //Write PDU
    //         for (_i = 0 ; _i < _len ; _i++) _send_buffer[_i+7] = _recv_frame[_i];
    //         //Send buffer
    //         _socket->send(_send_buffer,_len + 7);

    //         // pc.printf("Sent: ");
    //         // for(int i=0;i<_len + 7;i++) pc.printf("%.2x  ",_send_buffer[i]);
    //         // pc.printf("\r\r\n");
    //     }
    //     free(_recv_frame);
    //     free(_send_buffer);
    //     _len = 0;

    //     MB.Hreg(0,rand());
    //     MB.Hreg(1,rand());
    //     MB.Hreg(2,rand());
    //     MB.Hreg(3,rand());

    // }
}
