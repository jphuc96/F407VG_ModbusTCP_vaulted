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
// uint8_t buffer[64];

int main()
{
    LedBlue = 1;
    wait_ms(100);
    LedBlue = 0;

    pc.printf("=======================\r\r\r\r\n");
    pc.printf("====mbed Modbus TCP====\r\r\r\r\n");
    pc.printf("=======================\r\r\r\r\n");

    eth.connect();
    pc.printf("MAC: %s\r\r\r\r\n",eth.get_mac_address());
    pc.printf("IP: %s\r\r\r\r\n",eth.get_ip_address());

    MB.server_open(MODBUSTCP_PORT);

    MB.addHreg(0,100);
    MB.addHreg(1,200);
    MB.addHreg(2,300);
    MB.addHreg(3,400);

    MB.server_run();

    return 1;
}