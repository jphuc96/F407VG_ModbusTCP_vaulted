#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"

Serial pc(PA_9,PA_10,115200);

int main()
{
    pc.printf("TCP server example\n");
    
    EthernetInterface eth;
    eth.connect();
    
    pc.printf("The Server IP address is '%s'\n", eth.get_ip_address());
    
    TCPServer srv;
    TCPSocket client_sock;
    SocketAddress client_addr;
    char *buffer = new char[256];
    
    /* Open the server on ethernet stack */
    srv.open(&eth);
    
    /* Bind the HTTP port (TCP 80) to the server */
    srv.bind(eth.get_ip_address(), 23);
    
    /* Can handle x simultaneous connections */
    srv.listen(1);
 
    srv.accept(&client_sock, &client_addr);
    pc.printf("Accepted %s:%d\n", client_addr.get_ip_address(), 
                    client_addr.get_port());
    strcpy(buffer, "Hello \n\r");
    client_sock.send(buffer, strlen(buffer));
    client_sock.recv(buffer, 256);
    
    client_sock.close();
    delete[] buffer;
}
