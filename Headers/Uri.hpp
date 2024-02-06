#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <array>
#include <algorithm>
#include "split.hpp"

namespace Incart::Network
{

struct Uri
{
  // URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
  // authority = [userinfo "@"] host [":" port]
  // query = [parameter1=value2][(;|&)parameter2=value2]...
  // userinfo = username[:password]
  std::string scheme;
  std::string userName;
  std::string password;
  std::string host;
  uint16_t port;
  std::string path;
  std::string extention;
  std::unordered_map<std::string, std::string> query;
  std::string fragment;
  bool hasScheme = false;
  bool hasAuthority = false;
  bool hasQuery = false;
  bool hasFragment = false;
  bool hasUserInfo = false;
  bool hasPort = false;
  bool hasPassword = false;
  std::string fullUri;
  bool isValid = true;
  bool isRelative = false;

  Uri() = default;

  Uri(const std::string& scheme_, const std::string& userName_,
    const std::string& password_, const std::string& host_,
    uint16_t port_, const std::string& path_, const std::string extention_,
    const std::unordered_map<std::string, std::string>& query_,
    const std::string& fragment_,
    const std::array<bool, 9>& flags_,
    const std::string& fullUri_
  )
    : scheme(scheme_)
    , userName(userName_)
    , password(password_)
    , host(host_)
    , port(port_)
    , path(path_)
    , extention(extention_)
    , query(query_)
    , fragment(fragment_)
    , hasScheme(flags_[0])
    , hasAuthority(flags_[1])
    , hasQuery(flags_[2])
    , hasFragment(flags_[3])
    , hasUserInfo(flags_[4])
    , hasPort(flags_[5])
    , hasPassword(flags_[6])
    , fullUri(fullUri_)
    , isValid(flags_[7])
    , isRelative(flags_[8])
  {
  }

  Uri(const std::string& uri_) : Uri(Uri::parse(uri_))
  {
  }

  static Uri createNotValidUri()
  {
    return Uri("", "", "", "", 0, "", "", std::unordered_map<std::string, std::string>(), "", {
      false, false, false,
      false, false, false,
      false, false, false
    }, "");
  }

  static Uri parse(const std::string& text_)
  {
    size_t searchIndex = text_.find(':');
    bool isRelative = (searchIndex == std::string::npos);
    std::string scheme = isRelative ? "" : text_.substr(0, searchIndex);
    bool hasScheme = !isRelative;
    bool hasAuthorityFlag = false;
    bool hasUserInfoFlag = false;
    bool hasPortFlag = false;
    bool hasPasswordFlag = false;
    bool hasFragmentFlag = false;
    bool hasQueryFlag = false;
    uint16_t port = 0;
    std::string userName = "";
    std::string password = "";
    std::string host = "";
    std::string fragment = "";
    std::string extention = "";
    std::unordered_map<std::string, std::string> query;

    bool isOk = true;
    searchIndex = isRelative ? 0 : searchIndex + 1;
    size_t pathStartIndex = searchIndex;
    // try parse source
    if ((text_.size() > searchIndex + 1) && (text_[searchIndex] == '/') && (text_[searchIndex + 1] == '/'))
    {
      // try parse authority
      hasAuthorityFlag = true;
      pathStartIndex = text_.find('/', searchIndex + 2);
      if (pathStartIndex == std::string::npos)
      {
        pathStartIndex = text_.size();
      }
      std::string authorityText = text_.substr(searchIndex + 2, pathStartIndex - searchIndex - 2);
      // try parse userInfo
      
      searchIndex = authorityText.find('@');
      size_t startSearchPortIndex = 0;
      if (searchIndex != std::string::npos)
      {
        startSearchPortIndex = searchIndex + 1;
        hasUserInfoFlag = true;
        std::string userInfoText = authorityText.substr(0, searchIndex);
        // try parse password
        size_t userNameEndIndex = userInfoText.size();
        searchIndex = userInfoText.find(':');
        if (searchIndex != std::string::npos)
        {
          hasPasswordFlag = true;
          userNameEndIndex = searchIndex;
          password = userInfoText.substr(searchIndex + 1);
        }
        userName = userInfoText.substr(0, userNameEndIndex);
      }
      // try parse port
      searchIndex = authorityText.find(':', startSearchPortIndex);
      size_t hostEndIndex = authorityText.size();
      if (searchIndex != std::string::npos)
      {
        hostEndIndex = searchIndex;
        std::string portText = authorityText.substr(searchIndex + 1);
        try
        {
          port = static_cast<uint16_t>(std::stoi(portText));
        }
        catch (std::exception&)
        {
          return createNotValidUri();
        }
        hasPortFlag = true;
      }
      host = authorityText.substr(startSearchPortIndex, hostEndIndex - startSearchPortIndex);
    }
    // try parse fragment
    size_t fragmentIndex = text_.find('#', pathStartIndex);
    size_t pathEndIndex = text_.size();
    if (fragmentIndex != std::string::npos)
    {
      pathEndIndex = fragmentIndex;
      hasFragmentFlag = true;
      fragment = text_.substr(fragmentIndex + 1);
    }
    // try parse query
    std::string path = text_.substr(pathStartIndex, pathEndIndex - pathStartIndex);
    if (path.size() > 2)
    {
      size_t queryIndex = path.find('?');
      if (queryIndex != std::string::npos)
      {
        pathEndIndex = queryIndex;
        std::string queryText = path.substr(queryIndex + 1);
        if (!parseQuery(queryText, query))
        {
          return createNotValidUri();
        }
        path = text_.substr(0, queryIndex);
        hasQueryFlag = true;
      }
    }

    // try parse extention

    std::vector<std::string> list = split(path, '.');
    if (list.size() > 2) 
    {
      extention = list[list.size() - 1]; 
    }
    
    return Uri(scheme, userName, password, host, port, path, extention, query, fragment, {
      hasScheme, hasAuthorityFlag, hasQueryFlag,
      hasFragmentFlag, hasUserInfoFlag, hasPortFlag,
      hasPasswordFlag, true, isRelative
      }, text_);
  }

private:
    static bool parseQuery(const std::string& text, std::unordered_map<std::string, std::string>& query)
    {
        int state = 0; // 0 - c != '='; c != ';'; c != '&' - start read parameter
        // 1 - c == '=' - start read value
        // 2 - c == ';' || c == '&' - start read next pair
        std::string currParameter;
        std::string currValue;
        int parameterStartIndex = 0;
        int parameterSymbolCount = 0;
        int valueStartIndex = 0;
        int valueSymbolCount = 0;
        for (int i = 0; i < text.size(); i++)
        {
            char symbol = static_cast<char>(text[i]);
            switch (state)
            {
            case 0:
                if ((i == text.size() - 1) || symbol == '=' || symbol == ';' || symbol == '&')
                {
                    return false;
                }
                state = 1;
                currParameter = "";
                parameterStartIndex = i;
                break;
            case 1:
                if (((i == text.size() - 1) || symbol == ';' || symbol == '&') && (symbol != '='))
                {
                  parameterSymbolCount = i - parameterStartIndex;
                  if (symbol != ';' && symbol != '&')
                  {
                    ++parameterSymbolCount;
                  }
                  if (parameterSymbolCount == 0)
                  {
                    return false;
                  }
                  currParameter = text.substr(parameterStartIndex, parameterSymbolCount);
                  query.insert({ currParameter, "true"});
                  state = 0;
                }
                else if (symbol == '=')
                {
                    parameterSymbolCount = i - parameterStartIndex;
                    if (parameterSymbolCount == 0)
                    {
                      return false;
                    }
                    currParameter = text.substr(parameterStartIndex, parameterSymbolCount);
                    valueStartIndex = i + 1;
                    state = 2;
                }
                break;
             case 2:
                if ((i == text.size() - 1) || symbol == ';' || symbol == '&')
                {
                    valueSymbolCount = i - valueStartIndex;
                    if (symbol != ';' && symbol != '&')
                    {
                      ++valueSymbolCount;
                    }
                    if (valueSymbolCount == 0)
                    {
                      return false;
                    }
                    currValue = text.substr(valueStartIndex, valueSymbolCount);
                    query.insert({currParameter, currValue});
                    state = 0;
                }
                break;
            }
        }
        return true;
    }
};

}
