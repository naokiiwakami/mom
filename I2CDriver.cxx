#include <cstdio>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>

#include <sstream>

#include "I2CDriver.hxx"
#include "Session.hxx"
#include "ApplicationException.hxx"

using namespace std;

const string I2CDriver::DEVICE = "device";

I2CDriver::I2CDriver(Session* session)
    : m_session(session)
{
    // set default values
    m_device = "/dev/i2c-1";
    m_session->setEnvVar(DEVICE, m_device);

    m_fd = open(m_device.c_str(), O_RDWR);
    if (m_fd < 0) {
        std::string message = "device " + m_device + " open failed.";
        throw ApplicationException(message);
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

#if 0

void
SpiDriver::checkSession()
{
    stringstream errorStream;

    // Check if we need reconnect
    if (checkString(DEVICE, m_device)) {
        if (m_fd >= 0) {
            close(m_fd);
        }
        m_fd = -1;
    }

    bool reopened = false;
    if (m_fd < 0) {
        m_fd = open(m_device.c_str(), O_RDWR);
        if (m_fd < 0) {
            std::string message = "device " + m_device + " open failed.";
            throw ApplicationException(message);
        }
        reopened = true;
    }

    try {
        if (checkInt(MODE, m_mode) || reopened) {
            int ret = ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode);
            if (ret < 0) {
                errorStream << "failed to set mode " << m_mode;
                throw ApplicationException(errorStream.str());
            }
        }

        if (checkInt(SPEED, m_speed) || reopened) {
            int ret = ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_speed);
            if (ret < 0) {
                errorStream << "failed to set speed to " << m_speed;
                throw ApplicationException(errorStream.str());
            }
        }

        if (checkInt(BITS_PER_WORD, m_bitsPerWord) || reopened) {
            int ret = ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &m_bitsPerWord);
            if (ret < 0) {
                errorStream << "failed to get bits per word to " << m_bitsPerWord;
                throw ApplicationException(errorStream.str());
            }
        }
    }
    catch (const ApplicationException& ex) {
        if (m_fd >= 0) {
            close(m_fd);
            m_fd = -1;
        }
        throw;
    }
}

/**
 * Check particular value to environmental variable.
 * If there's an difference, set the env value to
 * the given value and return true.
 */
bool
SpiDriver::checkString(const string& key, string& value)
{
    const string& envValue = m_session->getEnvVar(key);
    if (key.empty()) {
        return false;
    }

    if (envValue != value) {
        value = envValue;
        return true;
    }
    
    return false;
}

/**
 * Check particular value to environmental variable.
 * If there's an difference, set the env value to
 * the given value and return true.
 */
template <typename T>
bool
SpiDriver::checkInt(const string& key, T& value)
{
    const string& strValue = m_session->getEnvVar(key);
    if (key.empty()) {
        return false;
    }

    stringstream ss;
    ss << strValue;

    int out;
    ss >> out;
    T envValue = out;

    if (envValue != value) {
        value = envValue;
        return true;
    }

    return false;
}
#endif
