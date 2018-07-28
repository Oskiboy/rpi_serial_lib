#ifndef I2C_HPP
#define I2C_HPP

#include <gpio.hpp>
#include <chrono>
#include <vector>
using namespace serial;
using gpio::HIGH;
using gpio::LOW;

#define READ  0x1
#define WRITE 0x0

namespace serial {
/*! The I2C class is an implementation of the I2C serial interface.
*  it requires two GPIO pins and sends or receives information from a slave unit.
*  There is possible to have multiple slaves and masters on the same network.
*
*  To start a transfer of data the master sends a start signal and then 8 bits of data.
*  This initial data contains the adress to the slave it selects as well if it is a read
*  or write operation. After this initial bit string, the master leaves the data line floating and the
*  slave pulls it up or down to acknowledge if the transfer is possible or not.
*  Then 8 bits of data is sent plus a start or stop signal, on a stop signal the transfer is done, but if another start bit is sent
*  then another 8 bits of data is sent, until a stop signal is sent.
*/

class I2C {
    public:
        I2C(int sda_pin, int scl_pin):
            sda_pin(sda_pin), scl_pin(scl_pin),
            sda(sda_pin, gpio::iof::OUT), scl(scl_pin, gpio::iof::OUT),
            clock_delay(500)
        {
                sda.write(HIGH);
                scl.write(HIGH);
        }

        /// Write data to the i2c bus.
        void writeData(char* msg, size_t msg_size, int address);
        /// Read data from the i2c bus.
        std::vector<char> readData(int address, size_t msg_size);


    private:
        void sendStartBit(void);
        void sendStopBit(void);
        void sendAddressByte(int address);
        bool readAckBit(void);
        void writeDataByte(char byte);
        char readDataByte(void);

        int sda_pin, scl_pin;
        gpio::GPIO sda, scl;
        std::chrono::milliseconds clock_delay;
        
};


} //namespace serial
#endif //I2C_HPP

