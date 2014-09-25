#ifndef _i2cdriver_hxx_
#define _i2cdriver_hxx_

#include <stdint.h>

#include <string>
#include <vector>

class Session;

class I2CDriver
{
public:
    I2CDriver(Session* session);

    // parameter names
    static const std::string DEVICE;
    /*
    static const std::string MODE;
    static const std::string SPEED;
    static const std::string BITS_PER_WORD;
    */

    bool ping(int address);

    void send(int address, const std::vector<uint8_t>& data);

private:
    /*
    void checkSession();

    bool checkString(const std::string& key, std::string& value);

    template <typename T>
    bool checkInt(const std::string& key, T& value);
    */
    void setAddress(int address);

private:
    Session* m_session;
    
    int m_fd;

    // cached I2C parameters
    std::string m_device;
    /*
    uint8_t m_mode;
    uint32_t m_speed; // Hz
    uint8_t m_bitsPerWord;
    */
};

#endif
