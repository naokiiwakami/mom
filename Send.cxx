#include <stdint.h>
#include <cstdio>

#include "Processor.hxx"
#include "Session.hxx"
#include "I2CDriver.hxx"
#include "ApplicationException.hxx"
#include "BadUsageException.hxx"
#include "Util.hxx"

using namespace std;

class SendProcessor : public Processor
{
public:
    SendProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("send");
    }

    string getShortDescription() {
        return string("Send SPI packets");
    }

    void myUsage() {
        printf("Usage: %s [options] <address> <data1> [<data2> [..]]\n", getCommand().c_str());
        printf("  Options:\n");
        printf("    -c: send as ASCII characters\n");
        printf("    -v: verbose\n");
    }

    void process(const vector<string>& argv, Session* session) {

        size_t ia = 1;
        // read options
        bool isAscii = false;
        bool isVerbose = false;
        while (ia < argv.size()) {
            const string& value = argv[ia];
            if (value == "-c") {
                isAscii = true;
            }
            else if (value == "-v") {
                isVerbose = true;
            }
            else {
                break;
            }
            ++ia;
        }

        if (ia >= argv.size()) {
            throw BadUsageException();
        }

        uint8_t address;
        if (!Util::parseData(&address, argv[ia++])) {
            throw ApplicationException("invalid address");
        }

        if (ia >= argv.size()) {
            throw BadUsageException();
        }

        // make the byte array to send
        vector<uint8_t> data;

        std::string error;
        for (; ia < argv.size(); ++ia) {
            const string& token = argv[ia];
            uint8_t item = 0;
            bool isThisAscii = isAscii;
            if (!isThisAscii) {
                isThisAscii = ! Util::parseData(&item, token);
            }
            if (isThisAscii) {
                for (size_t i = 0; i < token.size(); ++i) {
                    data.push_back(token[i]);
                }
            }
            else {
                data.push_back(item);
            }
        }

        if (isVerbose) {
            for (size_t i = 0; i < data.size(); ++i) {
                printf("data[%d] = 0x%x (%c)\n", i, data[i], data[i]);
            }
        }

        session->getI2C()->send(address, data);
    }
}
sendProcessor;
