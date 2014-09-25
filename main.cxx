#include <stdio.h>
#include <libgen.h>

#include "App.hxx"

int main(int argc, char* argv[])
{
    App *app = App::initialize(argc, argv);
    if (app == NULL) {
        return 1;
    }

    app->run();

    app->terminate();
    
    return 0;
}
