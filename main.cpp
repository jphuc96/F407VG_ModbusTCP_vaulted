#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"

EthernetInterface eth;
TCPServer srv;
TCPSocket clt_sock;
SocketAddress clt_addr;

Serial pc(PA_9,PA_10,115200);

int main()
{
    pc.printf("=======================\r\r\n");
    pc.printf("====mbed Modbus TCP====\r\r\n");
    pc.printf("=======================\r\r\n");

    eth.connect();
    pc.printf("MAC: %s\r\r\n",eth.get_mac_address());
    pc.printf("IP: %s\r\r\n",eth.get_ip_address());

    srv.open(&eth);
    srv.bind(eth.get_ip_address(), 502);
    srv.listen(5);

    while (true) {
        srv.accept(&clt_sock, &clt_addr);
        pc.printf("accept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
        char buffer[100] = "Hello World! \r\r\nModbus TCP\r\r\n";
        clt_sock.send(buffer,sizeof(buffer));
        char rbuffer[64];
        int rcount = clt_sock.recv(rbuffer, sizeof(rbuffer));
        pc.printf("recv %s | %d \r\r\n",rbuffer,rcount);
        delete[] rbuffer;
    }

    return 1;
}