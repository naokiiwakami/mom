#include <stdint.h>
#include <cstdio>

#include "Processor.hxx"
#include "Session.hxx"
#include "I2CDriver.hxx"
#include "ApplicationException.hxx"
#include "BadUsageException.hxx"
#include "Util.hxx"

using namespace std;

class PingProcessor : public Processor
{
public:
    PingProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("ping");
    }

    string getShortDescription() {
        return string("Ping SPI packets");
    }

    void myUsage() {
        printf("Usage: %s <address>\n", getCommand().c_str());
    }

    void process(const vector<string>& argv, Session* session) {

        size_t ia = 1;
        if (ia >= argv.size()) {
            throw BadUsageException();
        }

        // make the byte array to ping
        uint8_t address = 0;
        if (! Util::parseData(&address, argv[ia])) {
            throw ApplicationException("address syntax error");
        }

        bool result = session->getI2C()->ping(address);

        if (result) {
            printf("0x%x (%d) responded\n", address, address);
        }
        else {
            printf("No response at 0x%x (%d)\n", address, address);
        }
    }
}
pingProcessor;
