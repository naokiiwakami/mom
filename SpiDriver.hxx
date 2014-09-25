#ifndef _spidriver_hxx_
#define _spidriver_hxx_

#include <stdint.h>

#include <string>
#include <vector>

class Session;

class SpiDriver
{
public:
    SpiDriver(Session* session);

    // parameter names
    static const std::string DEVICE;
    static const std::string MODE;
    static const std::string SPEED;
    static const std::string BITS_PER_WORD;

    /**
     * Initialize by default.
     */
    void initialize();

    void send(const std::vector<uint8_t>& data);

private:
    void checkSession();

    bool checkString(const std::string& key, std::string& value);

    template <typename T>
    bool checkInt(const std::string& key, T& value);

private:
    Session* m_session;
    
    int m_fd;

    // cached SPI parameters
    std::string m_device;
    uint8_t m_mode;
    uint32_t m_speed; // Hz
    uint8_t m_bitsPerWord;

    // Temporary members to workaround RPi SPI SS pins mulfunction.
    int m_pin_SS;
};

#endif
