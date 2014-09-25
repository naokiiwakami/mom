#include <cstdio>
#include <iostream>

#include "Session.hxx"
#include "Processor.hxx"
#include "BadUsageException.hxx"

using namespace std;

class SetProcessor : public Processor
{
public:
    SetProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("set");
    }

    string getShortDescription() {
        return string("Set an environment variable");
    }

    void myUsage() {
        printf("Usage: %s [name [value]]\n", getCommand().c_str());
        printf("  where\n");
        printf("    name: environment variable name\n");
        printf("    value: environment variable value\n");
    }

    void process(const vector<string>& argv, Session* session) {
        if (argv.size() < 3) {
            Processor* delegate = find("show");
            delegate->process(argv, session);
        }
        else {
            session->setEnvVar(argv[1], argv[2]);
        }
    }
}
setProcessor;
