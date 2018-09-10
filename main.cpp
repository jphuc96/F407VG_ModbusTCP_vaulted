#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include "Modbus.h"
#include "ModbusTCP.h"

EthernetInterface eth;
ModbusTCP MB(&eth);

DigitalOut LedRed(PD_12);
DigitalOut LedBlue(PD_13);
DigitalOut LedGreen(PD_14);
DigitalOut LedYellow(PD_15);;

Serial pc(PA_9,PA_10,115200);
uint8_t buffer[64];
int main()
{
    LedBlue = 1;
    wait_ms(100);
    LedBlue = 0;

    pc.printf("=======================\r\r\n");
    pc.printf("====mbed Modbus TCP====\r\r\n");
    pc.printf("=======================\r\r\n");

    eth.connect();
    pc.printf("MAC: %s\r\r\n",eth.get_mac_address());
    pc.printf("IP: %s\r\r\n",eth.get_ip_address());

    MB.server_open(MODBUSTCP_PORT);

    while (true)
    {
        pc.printf("Waiting...\r\r\n");
        MB.server_run();
    }
    return 1;
}
