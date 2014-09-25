#include <cstdio>
#include <iostream>
#include "Processor.hxx"

using namespace std;

class HelpProcessor : public Processor
{
public:
    HelpProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("help");
    }

    string getShortDescription() {
        return string("Print help");
    }

    void myUsage() {
        printf("Usage: %s [command]\n", getCommand().c_str());
        printf("  where\n");
        printf("    command: command name\n");
    }

    void process(const vector<string>& argv, Session* session) {
        if (argv.size() < 2) {
            cout << "Commands:" << endl;
            map<string, Processor*>::iterator it = s_processors.begin();
            map<string, Processor*>::iterator ed = s_processors.end();

            for (; it != ed; ++it) {
                cout << "    " << it->first << ": "
                     << it->second->getShortDescription()
                     << endl;
            }
        }
        else {
            const std::string& name = argv[1];
            Processor* processor = find(name);
            if (processor == NULL) {
                cout << argv[0] << ": Command not found." << endl;
            }
            else {
                processor->usage();
            }
        }
    }
}
helpProcessor;
