#include <boost/lexical_cast.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

#include "Util.hxx"
#include "ApplicationException.hxx"

using namespace std;

bool
Util::parseData(uint8_t* data,
                const string& input)
{
    string error;
    try {
        unsigned int temp;

        bool isHex = false;
        bool isDec = false;

        // Determine if this is hex or decimal
        if (boost::starts_with(input, "0x") || boost::starts_with(input, "0X")) {
            string payload = input.substr(2);
            isHex = boost::algorithm::all_of(payload.begin(), payload.end(),
                                             boost::is_any_of("0123456789aAbBcCdDeEfF"));
        }
        else {
            isDec = boost::algorithm::all_of(input.begin(), input.end(), boost::is_digit());
        }

        if (isHex) {
            stringstream ss;
            ss << hex << input;
            ss >> temp;
        }
        else if (isDec) {
            temp = boost::lexical_cast<int>(input);
        }
        else {
            return false;
        }

        if (temp > 0xff) {
            error = "data out of range (must be [0..255])";
            throw boost::bad_lexical_cast();
        }

        *data = temp;
    }
    catch (const boost::bad_lexical_cast& ex) {
        throw ApplicationException(error);
    }

    return data;
}
