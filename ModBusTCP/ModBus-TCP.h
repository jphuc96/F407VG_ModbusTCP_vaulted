/* Copyright (C) 2017 Christian Gulde, MIT License
 *
 * ModBus-TCP includes code from HTTPD(Hiroshi Suga), which is licensed under the MIT license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP

#include "mbed.h"
#include "rtos.h"
#include "NetworkStack.h"
#include "EthernetInterface.h"
//#include "CBuffer.h"

//#define DEBUG

#define MODBUS_TCP_PORT 502
#define MODBUS_MAX_CLIENTS 3
#define MODBUS_KEEPALIVE 10
#define MODBUS_TIMEOUT 50000
#define MODBUS_MAX_HANDLES 2
 
#define MODBUS_MAX_REGISTER 256
 
 
#define MODBUS_CMD_SIZE 100
#define MODBUS_BUF_SIZE 128
#define MODBUS_STACK_SIZE (1024 * 6)
//efine HTTPD_ENABLE_CLOSER

//Debug is disabled by default
#if defined(DEBUG) and (!defined(TARGET_LPC11U24))
#define DBG(x, ...) std::printf("[DBG]" x "\r\n", ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[WARN]" x "\r\n", ##__VA_ARGS__);
#define ERR(x, ...) std::printf("[ERR]" x "\r\n", ##__VA_ARGS__);
#define INFO(x, ...) std::printf("[INFO]" x "\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#define INFO(x, ...)
#endif


class Modbus {
public:

    struct STATE {
        Thread *thread;
        TCPSocket *client;
        //volatile Request req;
        //CircBuffer <char>*buf;
        
        char start_message[8];

    };

    Modbus ();
    
    int start (EthernetInterface *ns, int port = MODBUS_TCP_PORT);

    // --- HTTPD_req.cpp ---
    void httpdError (int id, int err);

   
    // --- HTTPD_util.cpp ---
    TCPSocket *getClientSocket(int id) {
        if (id >= MODBUS_MAX_CLIENTS) return NULL;
        return _state[id].client;
    }
    int send (int id, unsigned short buffer[], int len);

    int attachRead (void (*funcCgi)(int id, unsigned short start_address, unsigned short length), int startRegister = 0);

    static Modbus * getInstance() {
        return _inst;
    };

private :
    static Modbus *_inst;
    Thread *_daemon;
    TCPServer _server;
    
    EthernetInterface *m_ns;

#ifdef MODBUS_ENABLE_CLOSER
    struct STATE _state[MODBUS_MAX_CLIENTS + 1];
#else
    struct STATE _state[MODBUS_MAX_CLIENTS];
#endif

    struct HANDLER {
        int startRegister;
        void (*funcCgiRead)(int id, unsigned short start_address, unsigned short length);
        void (*funcCgiWrite)(int id);
    } _handler[MODBUS_MAX_HANDLES];

    int _handler_count;

    static void daemon ();
    static void child (void const *arg);
    static void closer (void const *arg);

    // --- HTTPD_req.cpp ---
    
    void recvData (int id, char buf[], int size);
    int mb_exception_handler(int id, char data_in[], unsigned short length, char error);



    // --- HTTPD_util.cpp ---
    int getHandler (int startRegister);
    int sendToClient(int id, char buffer[], unsigned short len);
    int from_hex (int ch);
    int to_hex (int code);
};

#endif
