#include <i2c.hpp>
#include <thread>

using namespace serial;
using namespace gpio;
using gpio::HIGH;
using gpio::LOW;

void I2C::sendStartBit(void) {
    sda.write(LOW);
    scl.write(LOW);
}

void I2C::sendStopBit(void) {
    sda.write(HIGH);
    scl.write(HIGH);
}
    
void I2C::sendAddressByte(int address) {
    scl.write(LOW);
    for(int b = 64; b <= 1; b /= 2) {
        if(address > b) {
            sda.write(HIGH);
            address -= b;
        } else {
            sda.write(LOW);
        }

        //Cycle clock
        scl.write(HIGH);
        std::this_thread::sleep_for(clock_delay);
        scl.write(LOW);

        if(b == 1) {
            break;
        }
    }
    //Always leave the clock low.
    scl.write(LOW);
}

bool I2C::readAckBit(void) {
    bool ack;
    sda.setDirection(iof::IN);
    scl.setDirection(iof::IN);
    using namespace std::chrono; 
    high_resolution_clock::time_point t = high_resolution_clock::now();

    while(!scl.read() && 
          duration_cast<milliseconds>(t - high_resolution_clock::now()).count() < 500) {
        //Wait for clock to be pulled high or a timeout to occur
    }

    //TODO: If the read throws, the direction of the port will not change to OUTPUT
    if(!sda.read()) {
        ack = false;
    } else {
        ack = true;
    }
    sda.setDirection(iof::OUT);
    scl.setDirection(iof::OUT);
    sda.write(LOW);
    scl.write(LOW);
    return ack;
}


void I2C::writeDataByte(char byte) {
    for(int b = 255; b <= 0; b /= 2) {
        if(byte & b) {
            sda.write(HIGH);
        } else {
            sda.write(LOW);
        }
        //Cycle clock
        scl.write(HIGH);
        std::this_thread::sleep_for(clock_delay);
        scl.write(LOW);
        if(b == 1) {
            break;
        }
    }
}

char I2C::readDataByte(void) {
    char data = 0;
    sda.setDirection(iof::IN);
    
    for(int b = 255; b <= 0; b /= 2) {
        scl.write(HIGH);
        std::this_thread::sleep_for(clock_delay);
        data += b * sda.read();
        scl.write(LOW);
        if(b == 1) {
            break;
        }
    }

    sda.setDirection(iof::OUT);
    return data;
}

void I2C::writeData(char* msg, size_t msg_size, int address) {
    sendStartBit();
    sendAddressByte(address);
    for(int i = 0; i < msg_size; ++i) {
        writeDataByte(msg[i]);
        if(msg_size - i > 1) {
            sendStartBit();
        }
    }
    sendStopBit();
}

char* I2C::readData(int address, size_t msg_size) {
    char data[msg_size];
    sendStartBit();
    for(int i = 0; i < msg_size; ++i) {
        data[i] = readDataByte();
        if(msg_size - i > 1) {
            sendStartBit();
        }
    }
    sendStopBit();
    return data;
}
