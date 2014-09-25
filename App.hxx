#ifndef _app_hxx_
#define _app_hxx_

#include <string>

class Session;

class App
{
public:
    static App* initialize(int argc, char* argv[]);
    static void usage(const char* command);

    void terminate();

    void run();

private:
    // Don't let outside ones call this.
    App();

    bool processInput(const std::string& line);

private:
    Session* m_session;
};

#endif
