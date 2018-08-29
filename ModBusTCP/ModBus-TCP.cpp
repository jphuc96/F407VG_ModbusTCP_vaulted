/* Copyright (C) 2013 Hiroshi Suga, MIT License
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
 */

#include "ModBus-TCP.h"

Modbus * Modbus::_inst;

Modbus::Modbus () {
    _inst = this;
    memset(_state, 0, sizeof(_state));
    _handler_count = 0;
}

int Modbus::start (EthernetInterface *ns, int port) {
    int i;

    m_ns = ns;
    
    for (i = 0; i < MODBUS_MAX_CLIENTS; i ++) {
       // _state[i].buf = new CircBuffer<char>(MODBUS_BUF_SIZE);
        _state[i].thread = new Thread(osPriorityNormal, MODBUS_STACK_SIZE);
        _state[i].client = new TCPSocket();
        _state[i].thread->start(callback(child, (void*)i));
    }
    
#ifdef MODBUS_ENABLE_CLOSER
    _state[MODBUS_MAX_CLIENTS].thread = new Thread(closer, (void*)MODBUS_MAX_CLIENTS, osPriorityNormal, 128);
    _state[MODBUS_MAX_CLIENTS].client = new TCPSocket(m_ns);
#endif

    _server.open(m_ns);
    _server.bind(port);
    _server.set_blocking(true);
    _server.listen(MODBUS_MAX_CLIENTS);
    _daemon = new Thread(osPriorityNormal, MODBUS_STACK_SIZE);
    _daemon->start(Modbus::daemon);
    return 0;
}

void Modbus::daemon () {
    Modbus *modbus = Modbus::getInstance();
    int i, t = 0;

    
    for (;;) {
        //DBG("Wait for new connection... child %i",t);
        if (t >= 0) {
            if (modbus->_server.accept(modbus->_state[t].client) == 0) {
                INFO("accept %d\r\n", t);
                modbus->_state[t].thread->signal_set(0x1);
            }
        } else {
#ifdef HTTPD_ENABLE_CLOSER
            if (modbus->_server.accept(modbus->_state[MODBUS_MAX_CLIENTS].client) == 0) {
                INFO("accept x\r\n");
                modbus->_state[MODBUS_MAX_CLIENTS].thread->signal_set(0x1);
            }
#endif
        }

        t = -1;
        for (i = 0; i < MODBUS_MAX_CLIENTS; i ++) {
            //DBG("Child %i in State : %u", i, modbus->_state[i].thread->get_state());
            if ( modbus->_state[i].thread->get_state() == Thread::WaitingThreadFlag) {
                if (t < 0) t = i; // next empty thread
            }
        }
    }
}

void Modbus::child (void const *arg) {
    Modbus *modbus = Modbus::getInstance();
    int id = (int)arg;
    int i, n;
    char buf[MODBUS_BUF_SIZE];

    for (;;) {
        //DBG("child %i waiting for connection",id);
        Thread::signal_wait(0x1);
        
       

        INFO("Connection from client on child %i", id);
//      INFO("Connection from %s\r\n", httpd->_state[id].client->get_ip_address());

        modbus->_state[id].client->set_blocking(true);
        modbus->_state[id].client->set_timeout(15000);
        time_t t1 = time(NULL);
        for (;;) {
            //if (! httpd->_state[id].client->is_connected()) break;
            modbus->_state[id].client->set_blocking(true);
            modbus->_state[id].client->set_timeout(15000);
            n = modbus->_state[id].client->recv(buf, sizeof(buf));
            
            if (n < 0 ) {
                printf("Modbus::child breaking n = %d\r\n", n);
                break;
            }
            
            if( n > 0 ) { // We received something
                
                t1 = time(NULL);
            
                //DBG("Recv %d bytes Content: %x", n, buf)
            
                modbus->recvData(id, buf, n);
            }
            
            if(abs((int)(time(NULL) - t1))> 15) {
                DBG("Timeout in child %i",id);
                break;    
            }
            
        }

        modbus->_state[id].client->close(); // Needs to bere moved 
        INFO("Closed client connection");
        //INFO("Close %s\r\n", httpd->_state[id].client->get_ip_address());
    }
}

void Modbus::closer (void const *arg) {
    Modbus *modbus = Modbus::getInstance();
    int id = (int)arg;

    for (;;) {
        Thread::signal_wait(1);

        modbus->_state[id].client->close();
        INFO("Closed client connection\r\n");
        //INFO("Close %s\r\n", httpd->_state[id].client->get_ip_address());
    }
}

