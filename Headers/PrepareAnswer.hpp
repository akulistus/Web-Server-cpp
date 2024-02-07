#ifndef PEPAREANSWER_H
#define PEPAREANSWER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ParseQuerry.hpp"
#include "dbHelper.hpp"

namespace Network = Incart::Network;

struct Response {
    std::string res;
    std::string hdr;
    bool flg;

    void insert(const std::string& str) { hdr += str + "\r\n"; }

    void begin(const std::string& code) {
        res.clear();
        hdr = "HTTP/1.1 " + code + "\r\n";
        flg = true;
    }

    void end(const std::string& body) {
        hdr += "\r\n";
        res.append(hdr);
        res.append(body);
    }

    void add(const std::string& str) { res.append(str); }
};

class exeGet;
class makeCmdDev;
class PrepareAnswer {
public:
    PrepareAnswer();
    Response* exe(const std::string& data);
    DBHelper *db = new DBHelper();
    Response* getResponse() { return &m_resp; };
    Incart::Network::Uri* getUri() { return &m_uri; };
    makeCmdDev* getCmdDev() { return m_makeCmdDev; }

protected:
    Incart::Network::Uri m_uri;
    Response m_resp;
    makeCmdDev* m_makeCmdDev;

    std::map<std::string, exeGet*> m_exeGet;
    std::map<std::string, std::string> m_ext;

public:
    void htmlRet(const std::string& body);
    void jsonRet(const std::string& body);
    void extRet(const std::string& body);
    void errRet(const std::string& err);
    void downloadRet(const std::string& filePath);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class exeGet {
public:
    exeGet(PrepareAnswer* ans)
        : m_ans(ans)
        , m_uri(ans->getUri())
        , m_rsp(ans->getResponse())
    {}
    virtual ~exeGet() {}
    virtual void exe();

protected:
    PrepareAnswer *m_ans;
    Incart::Network::Uri *m_uri;
    Response *m_rsp;
};

class exeGetIndex : public exeGet {
public:
    exeGetIndex(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;
};

class exeGetFile : public exeGet {
public:
    exeGetFile(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;
};

// class exeGetDir : public exeGet {
// public:
//     exeGetDir(PrepareAnswer* ans)
//         : exeGet(ans)
//     {}
//     void exe() override;
// };

class exeGetObservation : public exeGet {
public:
    exeGetObservation(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeAddObservation : public exeGet {
public:
    exeAddObservation(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeGetObsState : public exeGet {
public:
    exeGetObsState(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeGetDate : public exeGet {
public:
    exeGetDate(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeGetFree : public exeGet {
public:
    exeGetFree(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeGetAllDates : public exeGet {
public:
    exeGetAllDates(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeChangeDay : public exeGet {
public:
    exeChangeDay(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeDeleteObs : public exeGet {
public:
    exeDeleteObs(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

class exeDeleteTime : public exeGet {
public:
    exeDeleteTime(PrepareAnswer* ans)
        : exeGet(ans)
    {}
    void exe() override;

private:
};

//-----------------------------------------------------------------------------
#endif // PEPAREANSWER_H
