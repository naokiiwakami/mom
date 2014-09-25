#include <iostream>
#include "Processor.hxx"

using namespace std;

std::map<std::string, Processor*> Processor::s_processors;

Processor*
Processor::find(const std::string& command)
{
    std::map<std::string, Processor*>::iterator it;
    it = s_processors.find(command);

    return (it != s_processors.end()) ? it->second : NULL;
}

Processor::Processor(const std::string& name)
{
    s_processors[name] = this;
}

void
Processor::usage()
{
    cerr << getCommand() << " - " << getShortDescription()
         << endl;

    myUsage();
}
