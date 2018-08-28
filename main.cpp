#include "mbed.h"
#include "EthernetInterface.h"

EthernetInterface eth;

Serial pc(PA_9,PA_10,115200);

int main()
{
    pc.printf("Modbus TCP test...\r\r\n");
    eth.connect();
    pc.printf("IP: &s\r\r\n",eth.get_ip_address());

}
