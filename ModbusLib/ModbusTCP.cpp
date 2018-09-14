/*
    ModbusTCP.cpp - Source for Modbus IP Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "ModbusTCP.h"

uint8_t buffer[MODBUSTCP_BUFFER_SIZE];
extern Serial pc;

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
    nsapi_size_or_error_t r;

    while(true)
    {
        _server->accept(_socket);
        pc.printf("Accept \r\r\n");
        // _socket->send("Hello !!\r\r\n",11);
        // pc.printf("Send \r\r\n");

        while (1) {
            _socket->set_blocking(true);
            _socket->set_timeout(15000);
            r = _socket->recv(buffer,sizeof(buffer));
            buffer[r] = '\0';
            if (r < 0 ) {
                pc.printf("Modbus breaking r = %d\r\r\n", r);
                break;
            }
            
            if( r > 0 ) { // We received something
                // pc.printf("Recv: %s Size: %d\r\r\n",buffer,r);
                for(int i=0;i<r;i++)
                {
                    pc.printf("%.2x  ",buffer[i]);
                }
                pc.printf("\r\r\n");
                //Send: 00  0D  00  00  00  0B  01  03  08  F1  AF  0F  DE  01  47  89  1D
                uint8_t send_buf[] = {0x00,0x0D,0x00,0x00,0x00,0x0B,0x01,0x03,0x08,0xF1,0xAF,0x0F,0xDE,0x01,0x47,0x89,0x1D};
                _socket->send(send_buf,sizeof(send_buf));
            }
            
        }

        _socket->close();
        pc.printf("Close \r\r\n");
    }

    // while(true)
    // {
    //     _server.accept(&_socket, &_address);
    //     int n = _socket.recv(buffer, sizeof(buffer));
    //     pc.printf("%s\r\r\n",buffer);
    //     delete[] buffer;
    // }
    


//     if (_server.accept(&_socket, &_address) == NSAPI_ERROR_OK) {
//         if (client.connected())
//         {
//             int i = 0;
//             while (client.available()){
//                 _MBAP[i] = client.read();
//                 i++;
//                 if (i==7) break;  //MBAP length has 7 bytes size
//             }
//             _len = _MBAP[4] << 8 | _MBAP[5];
//             _len--;  // Do not count with last byte from MBAP

//             if (_MBAP[2] !=0 || _MBAP[3] !=0) return;   //Not a MODBUSIP packet
//             if (_len > MODBUSIP_MAXFRAME) return;      //Length is over MODBUSIP_MAXFRAME

//             _frame = (byte*) malloc(_len);
//             i = 0;
//             while (client.available()){
//                 _frame[i] = client.read();
//                 i++;
//                 if (i==_len) break;
//             }

//             this->receivePDU(_frame);
//             if (_reply != MB_REPLY_OFF) {
//                 //MBAP
//                 _MBAP[4] = (_len+1) >> 8;     //_len+1 for last byte from MBAP
//                 _MBAP[5] = (_len+1) & 0x00FF;

//                 byte sendbuffer[7 + _len];

//                 for (i = 0 ; i < 7 ; i++) {
//                     sendbuffer[i] = _MBAP[i];
//                 }
//                 //PDU Frame
//                 for (i = 0 ; i < _len ; i++) {
//                     sendbuffer[i+7] = _frame[i];
//                 }
//                 client.write(sendbuffer, _len + 7);
//             }

// #ifndef TCP_KEEP_ALIVE
//             client.stop();
// #endif
//             free(_frame);
//             _len = 0;
//         }
//     }
}
