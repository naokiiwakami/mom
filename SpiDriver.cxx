#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <wiringPi.h>

#include <sstream>

#include "SpiDriver.hxx"
#include "Session.hxx"
#include "ApplicationException.hxx"

using namespace std;

const string SpiDriver::DEVICE = "device";
const string SpiDriver::MODE = "mode";
const string SpiDriver::SPEED = "speed";
const string SpiDriver::BITS_PER_WORD = "bitsperword";

SpiDriver::SpiDriver(Session* session)
    : m_session(session), m_fd(-1)
{
    // set default values
    m_device = "/dev/spidev0.0";
    m_session->setEnvVar(DEVICE, m_device);


    m_mode = SPI_MODE_0; // CPOL = 0, CPHA = 0
    stringstream ssm;
    ssm << (int) m_mode;
    m_session->setEnvVar(MODE, ssm.str());

    stringstream sss;
    m_speed = 500000; // 500 kHz
    sss << m_speed;
    m_session->setEnvVar(SPEED, sss.str());

    stringstream ssb;
    m_bitsPerWord = 8;
    ssb << (int) m_bitsPerWord;
    m_session->setEnvVar(BITS_PER_WORD, ssb.str());

    // This workarounds my Raspberry Pi SPI SS pins mulfunction
    // by using GPIO 23 as an alternative SS0.
    if (wiringPiSetupGpio() < 0) {
        throw ApplicationException("failed to initialize wiring pi");
    }

    m_pin_SS = 23;
    pinMode(m_pin_SS, OUTPUT); // TODO: revert back to previous state on shutdown
    digitalWrite(m_pin_SS, HIGH);
}

#define _enableSS(x) digitalWrite(m_pin_SS, LOW)
#define _disableSS(x) digitalWrite(m_pin_SS, HIGH)

void
SpiDriver::send(const std::vector<uint8_t>& data)
{
    checkSession();

    size_t size = data.size();
    const uint8_t* tx = &data[0];
    uint8_t* rx = new uint8_t[size];

    struct spi_ioc_transfer tr;
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = size;
    tr.delay_usecs = 0;
    tr.speed_hz = m_speed;
    tr.bits_per_word = m_bitsPerWord;

    _enableSS(); // to workaround HW mulfunction

    int ret = ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0) {
        throw ApplicationException("SPI data exchange failed");
    }

    _disableSS(); // to workaround HW mulfunction

    delete[] rx;
}

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
