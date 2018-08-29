

#include "ModBus-TCP.h"

void Modbus::recvData (int id, char buffer[], int size) {
    
    int bytes_rec;
    int returnLength;
    unsigned short transaction_number;
    unsigned short protokoll;
    unsigned short number_of_bytes;
    char adress;
    int port;
    
    transaction_number = buffer[0] + (buffer[1] << 8);
    protokoll = buffer[2] + (buffer[3] << 8);
    number_of_bytes = (buffer[4] << 8) + buffer[5] ;
    adress = buffer[6];
                  
    if(protokoll != 0x0000) {
         printf("No Modbus protkoll");    
    }
            
    #ifdef DEBUG
    //printf("tn: 0x%x, p: 0x%x, nb: %i, a: %i\r\n", transaction_number, protokoll, number_of_bytes, adress);
    #endif
    
    for(int i=0; i<8; i++) {
        _state[id].start_message[i] = buffer[i];
    }
    
    //returnLength = mb_handle_data(buffer, number_of_bytes);
    
    char fc; 
    fc = buffer[7];
    
    
    unsigned short start_adress;
    unsigned short number_of_registers;
   
    
    switch(fc) {
        case 0x04: //return mb_read_register(data_in, length); 
        {
            //Read Requet received
            // Getting Start Address and Length
            
            //DBG("Read Request received\r\n");
            
             start_adress = (buffer[8] << 8) + buffer[9];
             number_of_registers = (buffer[10] << 8) + buffer[11];
        
             
             int i = getHandler(0);
             //DBG("GetHandler %i",i);
             
             if(number_of_registers < 0x0001 || number_of_registers > 0x007d ) {
                mb_exception_handler(id, buffer, number_of_registers, 0x03);
            }
    
            if(start_adress >= MODBUS_MAX_REGISTER) {
                mb_exception_handler(id, buffer, number_of_registers, 0x02);            
            }
    
            if(number_of_registers < 0x0001 || number_of_registers + start_adress > MODBUS_MAX_REGISTER) {
                mb_exception_handler(id, buffer, number_of_registers, 0x04); 
            }
            
            
             
            _handler[i].funcCgiRead(id, start_adress, number_of_registers);
            break;
        }
        case 0x10: //return mb_write_register(data_in, length);
        {
            break;
        }      
        default:  mb_exception_handler(id, buffer, size, 0x01);
                
    }
}

int Modbus::mb_exception_handler(int id, char data_in[], unsigned short length, char error) {
    unsigned short number_of_bytes;
    
    data_in[7] += 0x80;
    data_in[8] += error;
    
    number_of_bytes = 3;
            
    data_in[4] = (char)(number_of_bytes >> 8);
    data_in[5] = (char)(number_of_bytes & 0x00ff);
    
    sendToClient(id, data_in, number_of_bytes + 7);
    
    return 0;
}


