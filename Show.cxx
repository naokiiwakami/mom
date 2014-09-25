#include <cstdio>
#include <iostream>

#include "Session.hxx"
#include "Processor.hxx"

using namespace std;

class ShowProcessor : public Processor
{
public:
    ShowProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("show");
    }

    string getShortDescription() {
        return string("Show environment variables");
    }

    void myUsage() {
        printf("Usage: %s [name]\n", getCommand().c_str());
        printf("  where\n");
        printf("    name: environment variable name\n");
    }

    void process(const vector<string>& argv, Session* session) {

        if (argv.size() < 2) {
            vector<string> keys = session->getEnvironmentVariableNames();
            for (size_t i = 0; i < keys.size(); ++i) {
                string& name = keys[i];
                cout << name << "=" << session->getEnvVar(name)
                     << endl;
            }
        }
        else {
            const string& name = argv[1];
            cout << session->getEnvVar(name)
                 << endl;
        }
    }
}
showProcessor;
