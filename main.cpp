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
Thread thread_update_registers(osPriorityNormal,OS_STACK_SIZE,NULL,"update_registers");
void task_update_registers();

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

    modbustcp.addHreg(0,100);
    modbustcp.addHreg(1,200);
    modbustcp.addHreg(2,300);
    modbustcp.addHreg(3,400);

    modbustcp.start(&eth,502);
    pc.printf("ModbusTCP started\r\r\n");

    thread_update_registers.start(task_update_registers);

    while(1)
    {
        LedGreen = 0;
        Thread::wait(500);
        LedGreen = 1;
        Thread::wait(500);
    }
    return 1;
}

void task_update_registers()
{
    //update register here, need to check Mutex
    for(;;)
    {
        modbustcp.Hreg(0,rand());
        modbustcp.Hreg(1,rand());
        modbustcp.Hreg(2,rand());
        modbustcp.Hreg(3,rand());

        Thread::wait(500);
    }
}