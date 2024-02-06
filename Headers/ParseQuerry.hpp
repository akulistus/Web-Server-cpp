#include <vector>
#include <sstream>
#include <string>
#include "Uri.hpp"

namespace M_Uri 
{
    Incart::Network::Uri ParseRequest(const std::string request);
    // static std::string getMethod() { return m_method; };
    // static std::string findHostHeader(const std::vector<std::string>& lines);
}