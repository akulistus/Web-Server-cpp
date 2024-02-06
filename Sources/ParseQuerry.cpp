#include "ParseQuerry.hpp"

Incart::Network::Uri M_Uri::ParseRequest(const std::string request)
{
  std::vector<std::string> elems = split(request, ' ');

  // m_method = elems[0];

  Incart::Network::Uri uri = Incart::Network::Uri::parse(elems[1]);
  return uri;
}
