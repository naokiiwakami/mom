#ifndef _i2cdriver_hxx_
#define _i2cdriver_hxx_

#include <stdint.h>
#include <pthread.h>

#include <string>
#include <vector>

class Session;
namespace boost {
    class thread;
}

class I2CDriver
{
public:
    I2CDriver(Session* session);

    I2CDriver* getInstance() {
        return s_instance;
    }

    // parameter names
    static const std::string DEVICE;

    bool ping(int address);

    void send(int address, const std::vector<uint8_t>& data);

    void interrupt();

private:
    void setAddress(int address);

    // ReadReady pin listener related
    static void launch(I2CDriver* driver);
    static void ISR_ReadReady();
    void threadmain();

private:
    Session* m_session;
    
    int m_fd;

    // cached I2C parameters
    std::string m_device;

    // GPIO pin number to listen on ReadReady
    int m_pin_ReadReady;

    boost::thread* m_thread;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

    static I2CDriver* s_instance; // singleton
};

#endif
