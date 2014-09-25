#ifndef _util_h_
#define _util_h_

#include <stdint.h>
#include <string>

class Util
{
public:
    static bool parseData(uint8_t* data,
                          const std::string& input);
};

#endif
