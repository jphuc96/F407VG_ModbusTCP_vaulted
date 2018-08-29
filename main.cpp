#include "mbed.h"
#include "EthernetInterface.h"
#include "ModBus-TCP.h"

EthernetInterface eth;
Modbus ModbusTCP;

Serial pc(PA_9,PA_10,115200);

int main()
{
    pc.printf("=======================\r\r\n");
    pc.printf("====mbed Modbus TCP====\r\r\n");
    pc.printf("=======================\r\r\n");

    eth.connect();

    pc.printf("MAC: %s\r\r\n",eth.get_mac_address());
    pc.printf("IP: %s\r\r\n",eth.get_ip_address());

}