#include "mbed.h"
#include "EthernetInterface.h"

#include "Modbus-TCP.h"
#include "ModbusMaster.h"

EthernetInterface eth;
ModbusTCP modbustcp_s;

DigitalOut LedRed(PD_12);
DigitalOut LedBlue(PD_13);
DigitalOut LedGreen(PD_14);
DigitalOut LedYellow(PD_15);

Serial pc(PA_9,PA_10,115200);

BufferedSerial buffered_serial(PD_5,PA_3);
ModbusMaster modbusrtu_m_s1, modbusrtu_m_s2;

/*Thread declares*/
Thread thread_modbusrtu_routine(osPriorityNormal,1024,NULL,"modbusrtu_routine");
Thread thread_update_registers(osPriorityNormal,OS_STACK_SIZE,NULL,"update_registers");
void task_update_registers();
void task_modbusrtu_routine();

int main()
{
    LedBlue = 1;
    wait_ms(100);
    LedBlue = 0;

    pc.printf("===========================\r\r\n");
    pc.printf("====mbed Modbus TCP-RTU====\r\r\n");
    pc.printf("===========================\r\r\n");

    eth.connect();
    pc.printf("MAC: %s\r\r\n",eth.get_mac_address());
    pc.printf("IP: %s\r\r\n",eth.get_ip_address());

    buffered_serial.baud(57600);
    buffered_serial.format(8,Serial::None,2);
    pc.printf("Initialized Modbus RTU master.\r\r\n");

    modbustcp_s.start(&eth,502);
    pc.printf("ModbusTCP server started\r\r\n");

    thread_update_registers.start(task_update_registers);
    thread_modbusrtu_routine.start(task_modbusrtu_routine);

    while(1)
    {
        // LedGreen = 0;
        Thread::wait(500);
        // LedGreen = 1;
        Thread::wait(500);
    }
    return 1;
}

void task_update_registers()
{
    //update registers here, need to check Mutex
    modbustcp_s.addHreg(0,100);
    modbustcp_s.addHreg(1,200);
    modbustcp_s.addHreg(2,300);
    modbustcp_s.addHreg(3,400);

    AnalogIn cpu_temp(ADC_TEMP);
    AnalogIn vbat(ADC_VBAT);
    for(;;)
    {
        modbustcp_s.Hreg(0,cpu_temp.read_u16());
        modbustcp_s.Hreg(1,vbat.read_u16());
        modbustcp_s.Hreg(2,rand());
        modbustcp_s.Hreg(3,rand());

        Thread::wait(500);
    }
}

void task_modbusrtu_routine()
{
    modbusrtu_m_s1.begin(1,buffered_serial);
    modbusrtu_m_s1.begin(2,buffered_serial);
    for(;;)
    {

    }
}