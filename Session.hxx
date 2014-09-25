#ifndef _session_hxx_
#define _session_hxx_

#include <string>
#include <map>
#include <vector>

class I2CDriver;

class Session
{
public:
    Session();
    bool processCommand(const std::vector<std::string>& argv);

    std::string getPrompt() {
        return m_prompt;
    }

    I2CDriver* getI2C() {
        return m_i2c;
    }

    std::vector<std::string> getEnvironmentVariableNames();
    std::string getEnvVar(const std::string& name);
    void setEnvVar(const std::string& name, const std::string& value);

private:
    std::map<std::string, std::string> m_environment;

    I2CDriver* m_i2c;

    std::string m_prompt;
};

#endif
