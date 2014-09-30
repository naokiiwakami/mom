#include <boost/thread/thread.hpp>
#include <cstdio>
#include <iostream>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <wiringPi.h>

#include "I2CDriver.hxx"
#include "Session.hxx"
#include "ApplicationException.hxx"


using namespace std;

const string I2CDriver::DEVICE = "device";
I2CDriver* I2CDriver::s_instance = NULL;

I2CDriver::I2CDriver(Session* session)
    : m_session(session)
{
    // This class is singleton
    if (s_instance != NULL) {
        throw ApplicationException("I2CDriver instance already exists.");
    }
    s_instance = this;

    // set default values
    m_device = "/dev/i2c-1";
    m_session->setEnvVar(DEVICE, m_device);

    m_fd = open(m_device.c_str(), O_RDWR);
    if (m_fd < 0) {
        std::string message = "device " + m_device + " open failed.";
        throw ApplicationException(message);
    }

    // This program monitors GPIO 23 to 
    if (wiringPiSetupGpio() < 0) {
        throw ApplicationException("failed to initialize wiring pi");
    }

    m_pin_ReadReady = 23;
    pinMode(m_pin_ReadReady, INPUT); // TODO: revert back to previous state on shutdown

    wiringPiISR(m_pin_ReadReady, INT_EDGE_BOTH, ISR_ReadReady);

    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);

    m_thread = new boost::thread(launch, this);
}

void
I2CDriver::launch(I2CDriver* driver)
{
    driver->threadmain();
}

void
I2CDriver::ISR_ReadReady()
{
    // cout << "\rinterrupt!" << endl;
    s_instance->interrupt();
}

void
I2CDriver::interrupt()
{
    pthread_mutex_lock(&m_mutex);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

void
I2CDriver::threadmain()
{
    // boost::chrono::milliseconds timeToSleep(10000);
    // boost::posix_time::time_duration timeout = boost::posix_time::seconds(1);
    // boost::mutex theMutex();
    // boost::condition_variable theCondition;

    struct timespec ts;
    struct timeval tv;

    while (true) {
        // boost::mutex::scoped_lock lock(theMutex);
        // theCondition.timed_wait(lock, timeout);
        // boost::this_thread::sleep_for(timeToSleep);
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;
        ts.tv_sec += 1;
        pthread_mutex_lock(&m_mutex);
        pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
        pthread_mutex_unlock(&m_mutex);
        if (digitalRead(m_pin_ReadReady)) {
            const int DataSize = 8;
            uint8_t buf[DataSize];
            setAddress(8);
            int result = read(m_fd, buf, 1);
            int size = buf[0];
            result = read(m_fd, buf, size);
            if (result < 0) {
                throw ApplicationException("I2C read failure");
            }
            printf("\r");
            for (int i = 0; i < result; ++i) {
                printf("data[%d] = 0x%x (%c)\n", i, buf[i], buf[i]);
            }
            cout << m_session->getPrompt() << flush;
        }
        /*
        cout << "\r";
        if (digitalRead(m_pin_ReadReady)) {
            cout << "HIGH";
        }
        else {
            cout << "LOW";
        }
        cout << endl;

        cout << m_session->getPrompt() << flush;
        */
    }
}

void
I2CDriver::setAddress(int address)
{
    // TODO: check address... assert 4 <= address < 128
    if (ioctl(m_fd, I2C_SLAVE, address) < 0) {
        throw ApplicationException("Setting slave address failed.");
    }
}

bool
I2CDriver::ping(int address)
{
    setAddress(address);

    int result = write(m_fd, NULL, 0);

    return (result == 0);
}

void
I2CDriver::send(int address, const vector<uint8_t>& data)
{
    setAddress(address);

    int result = write(m_fd, &data[0], data.size());
    if (result < 0) {
        // TODO: change exception type
        throw ApplicationException("I2C write failure");
    }
}
