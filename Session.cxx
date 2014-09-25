#include <iostream>

#include "Processor.hxx"
#include "Session.hxx"
#include "I2CDriver.hxx"
#include "ApplicationException.hxx"
#include "BadUsageException.hxx"

using namespace std;

Session::Session()
{
    // default device name
    m_i2c = new I2CDriver(this);
    m_prompt = "mom> ";
}

bool
Session::processCommand(const vector<string>& argv)
{
    if (argv.empty()) {
        return false;
    }

    if (argv[0] == "quit") {
        return true;
    }

    Processor* processor = Processor::find(argv[0]);
    if (processor == NULL) {
        cout << argv[0] << ": Command not found. Type \"help\" to list supported commands." << endl;
    }
    else {
        try {
            processor->process(argv, this);
        }
        catch (const BadUsageException& ex) {
            if (ex.hasMessage()) {
                cout << ex.getMessage() << endl << endl;
            }

            processor->usage();
        }
        catch (const ApplicationException& ex) {
            cout << ex.what() << endl;
        }
    }

    return false;
}

vector<string>
Session::getEnvironmentVariableNames()
{
    vector<string> output;

    map<string, string>::iterator it = m_environment.begin();
    map<string, string>::iterator ed = m_environment.end();

    for (; it != ed; ++it) {
        output.push_back(it->first);
    }

    return output;
}

string
Session::getEnvVar(const string& name)
{
    string output;
    map<string, string>::iterator it = m_environment.find(name);
    if (it != m_environment.end()) {
        output = it->second;
    }
    return output;
}

void
Session::setEnvVar(const std::string& name, const std::string& value)
{
    m_environment[name] = value;
}
