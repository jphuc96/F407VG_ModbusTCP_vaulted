#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include "Modbus.h"
#include "Modbus-TCP.h"

EthernetInterface eth;
// ModbusTCP MB(&eth);
ModbusTCP modbustcp;

DigitalOut LedRed(PD_12);
DigitalOut LedBlue(PD_13);
DigitalOut LedGreen(PD_14);
DigitalOut LedYellow(PD_15);

Serial pc(PA_9,PA_10,115200);
// uint8_t buffer[64];

/*Thread declares*/
Thread thread_modbustcp(osPriorityNormal,OS_STACK_SIZE,NULL,"ModbusTCP");
void task_modbustcp();

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

    modbustcp.addHreg(0,100);
    modbustcp.addHreg(1,200);
    modbustcp.addHreg(2,300);
    modbustcp.addHreg(3,400);

    // MB.server_start(MODBUSTCP_PORT);

    modbustcp.start(&eth,502);

    while(1)
    {
        LedGreen = 0;
        Thread::wait(500);
        LedGreen = 1;
        Thread::wait(500);
    }
    return 1;
}
