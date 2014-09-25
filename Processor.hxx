#ifndef _processor_h_
#define _processor_h_

#include <string>
#include <vector>
#include <map>

class Session;

class Processor
{
public:
    
    static Processor* find(const std::string& command);

    Processor(const std::string& name);

    void usage();

    virtual std::string getCommand() = 0;
    virtual std::string getShortDescription() = 0;
    virtual void myUsage() = 0;
    virtual void process(const std::vector<std::string>& argv, Session* session) = 0;

protected:
    static std::map<std::string, Processor*> s_processors;

};

#endif
