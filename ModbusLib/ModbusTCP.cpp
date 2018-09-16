/*
    ModbusTCP.cpp - Source for Modbus IP Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "ModbusTCP.h"
extern Serial pc;
extern ModbusTCP MB;

ModbusTCP::ModbusTCP(NetworkInterface* _net) : network(_net)
{
    _server = new TCPServer();
    _socket = new TCPSocket();
}

void ModbusTCP::server_open(uint16_t port)
{
    _server->open(network);
    _server->bind(network->get_ip_address(),port);
    _server->listen(2);
}

void ModbusTCP::server_stop()
{
    _socket->close();
}

void ModbusTCP::server_run()
{
    nsapi_size_or_error_t _ret;
    pc.printf("Running...\r\r\n");
    _server->accept(_socket,_address);
    pc.printf("Accept: %s:%d\r\r\n",_address->get_ip_address(),_address->get_port());
    while(true)
    {
        int _i;
        _socket->set_blocking(true);
        _socket->set_timeout(15000);
        _ret = _socket->recv(_recv_buffer,MODBUSTCP_BUFFER_SIZE);
        _recv_buffer[_ret] = '\0';

        if(_ret < 0) {
            //Failed
            pc.printf("Modbus breaking r = %d\r\r\n", _ret);
            this->server_run();
            break;
        }
        if(_ret > 0) {
            //Successed
            pc.printf("Recv: ");
            for(int i=0;i<_ret;i++) pc.printf("%.2x  ",_recv_buffer[i]);
            pc.printf("\r\r\n");

            for(_i=0;_i<7;_i++) _MBAP[_i] = _recv_buffer[_i];

            _len = _MBAP[4] << 8 | _MBAP[5];
            _len--;  // Do not count with last byte from MBAP
        }
        if (_MBAP[2] !=0 || _MBAP[3] !=0) return;   //Not a MODBUSIP packet
        if (_len > MODBUSTCP_MAXFRAME) return;      //Length is over MODBUSTCP_MAXFRAME

        _recv_frame = (byte*)malloc(_len);
        for(_i=0;_i<_len;_i++) _recv_frame[_i] = _recv_buffer[_i+7];

        this->receivePDU(_recv_frame);
        if (_reply != MB_REPLY_OFF) {
            //MBAP
            _MBAP[4] = (_len+1) >> 8;     //_len+1 for last byte from MBAP
            _MBAP[5] = (_len+1) & 0x00FF;

            _send_buffer = (byte*) malloc(7 + _len);

            //Write MBAP
            for (_i = 0 ; _i < 7 ; _i++) _send_buffer[_i] = _MBAP[_i];
            //Write PDU
            for (_i = 0 ; _i < _len ; _i++) _send_buffer[_i+7] = _recv_frame[_i];
            //Send buffer
            _socket->send(_send_buffer,_len + 7);

            pc.printf("Sent: ");
            for(int i=0;i<_len + 7;i++) pc.printf("%.2x  ",_send_buffer[i]);
            pc.printf("\r\r\n");
        }
        free(_recv_frame);
        free(_send_buffer);
        _len = 0;

        if(_socket->)

        MB.Hreg(0,rand());
        MB.Hreg(1,rand());
        MB.Hreg(2,rand());
        MB.Hreg(3,rand());

    }
}
