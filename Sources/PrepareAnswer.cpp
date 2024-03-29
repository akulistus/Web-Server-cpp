#include "PrepareAnswer.hpp"

PrepareAnswer::PrepareAnswer()
{
    // распознаваемые значения path
    m_exeGet["/"] = new exeGetIndex(this);
    m_exeGet["__err"] = new exeGet(this);
    m_exeGet["__file"] = new exeGetFile(this);
    // m_exeGet["/getDir"] = new exeGetDir(this);
    m_exeGet["/getObservations"] = new exeGetObservation(this);
    m_exeGet["/addObservation"] = new exeAddObservation(this);
    m_exeGet["/changeObsState"] = new exeGetObsState(this);
    m_exeGet["/getFree"] = new exeGetFree(this);
    m_exeGet["/deleteObs"] = new exeDeleteObs(this);
    m_exeGet["/deleteTime"] = new exeDeleteTime(this);
    m_exeGet["/getDay"] = new exeGetDate(this);
    m_exeGet["/changeDay"] = new exeChangeDay(this);
    m_exeGet["/getAllDates"] = new exeGetAllDates(this);
    m_ext["html"] = "Content-Type: text/html; charset=utf8;";
    m_ext["json"] = "Content-Type: application/json; charset=utf8;";
    m_ext["js"] = "Content-Type: text/javascript; charset=utf8;";
    m_ext["css"] = "Content-Type: text/css; charset=utf8;";
    m_ext["txt"] = "Content-Type: text/plain; charset=utf8";
    m_ext["map"] = "Content-Type: application/x-navimap";
    m_ext["svg"] = "Content-Type: image/svg+xml";
    m_ext["png"] = "Content-type: image/png";
}

Response *PrepareAnswer::exe(const std::string& query)
{
    //Почему иногда приходит пустой запрос...
    if (query != "") {
        m_uri = M_Uri::ParseRequest(query);
    }

    if (true) {
        if (m_exeGet.contains(m_uri.path)) {
            m_exeGet[m_uri.path]->exe();
        } else {
            m_exeGet["__file"]->exe();
        }
    } else {
        m_exeGet["__err"]->exe();
    }

    return &m_resp;
}
//-----------------------------------------------------------------------------
void PrepareAnswer::htmlRet(const std::string& body)
{
    m_resp.begin("200 OK");
    m_resp.insert("Access-Control-Allow-Origin:*");
    m_resp.insert("Content-Type: text/html; charset=utf8");
    m_resp.end(body);
}

void PrepareAnswer::jsonRet(const std::string& body)
{
    m_resp.begin("200 OK");
    m_resp.insert("Access-Control-Allow-Origin:*");
    m_resp.insert("Content-Type: application/json");
    m_resp.end(body);
}

void PrepareAnswer::extRet(const std::string& body)
{
    m_resp.begin("200 OK");
    m_resp.insert("Access-Control-Allow-Origin:*");
    if (m_ext.contains(m_uri.extention))
        m_resp.insert(m_ext[m_uri.extention]);
    else
        m_resp.insert(m_ext["txt"]);
    m_resp.end(body);
}

// void PrepareAnswer::downloadRet(QFile f)
// {
//     f.open(QIODevice::ReadOnly);
//     m_resp.begin("200");
//     m_resp.insert("Access-Control-Allow-Origin:*");
//     m_resp.insert("Content-Type: application/octet-stream");
//     m_resp.insert("Content-Size: " + std::string::number(f.size()));
//     m_resp.insert("attachment; filename=" + f.fileName());
//     m_resp.insert("Accept-Ranges: bytes");
//     int64_t bytesRead = 64000;
//     m_resp.end(f.read(64000));
//     while (bytesRead < f.size()) {
//         f.seek(bytesRead);
//         m_resp.add(f.read(64000));
//         bytesRead += 64000;
//     }
//     f.close();
// }

void PrepareAnswer::errRet(const std::string& err)
{
    m_resp.begin("400");
    m_resp.insert("Access-Control-Allow-Origin:*");
    m_resp.insert("Content-Type: text/html; charset=utf8");
    m_resp.end(err);
}
//-----------------------------------------------------------------------------
void exeGet::exe()
{
    m_ans->errRet("501 Not Implemented");
}

void exeGetIndex::exe()
{
    std::ifstream file("index.html");
    if (file.is_open())
    {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        m_ans->htmlRet(buffer.str());
        file.close();
    }
    else 
    {
        std::cerr << "File not found: index.html" << std::endl;
        m_ans->errRet("404 Not Found");
    }
}

void exeGetFile::exe()
{
    std::ifstream file(m_uri->path.substr(1), std::ios::binary);
    std::cerr << m_uri->path << std::endl;
    if (file.is_open())
    {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        m_ans->extRet(buffer.str());
        file.close();
    }
    else
    {
        std::cerr << "File not found: index.html" << std::endl;
        m_ans->errRet("404 Not Found");
    }
}

// void exeGetDir::exe()
// {
//     QDir dir;
//     dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//     dir.setSorting(QDir::Size | QDir::Reversed);
//     QFileInfoList list = dir.entryInfoList();
//     QJsonArray array;
//     for (int i = 0; i < list.size(); i++) {
//         QFileInfo fileInfo = list.at(i);
//         std::string filename = fileInfo.fileName();
//         QJsonObject obj = {{"filename", filename}};
//         array.append(obj);
//     }
//     QJsonDocument doc(array);
//     m_ans->jsonRet(doc.toJson());
// }

void exeGetObservation::exe()
{
    m_ans->jsonRet(m_ans->db->getAll().dump());
}

void exeGetFree::exe()
{
    m_ans->jsonRet(m_ans->db->getFree().dump());
}

void exeGetAllDates::exe()
{
    m_ans->jsonRet(m_ans->db->getAllDates().dump());
}

void exeGetDate::exe()
{
    std::unordered_map<std::string, std::string>& query = m_uri->query;

    auto it = query.find("date");
    if (it != query.end())
    {
        m_ans->jsonRet(m_ans->db->getDay(it->second).dump());
    }
    else
    {
        m_ans->errRet("wrong args");
    }
}
void exeChangeDay::exe()
{
    std::unordered_map<std::string, std::string>& query = m_uri->query;

    auto dateIt = query.find("date");
    auto timeIt = query.find("time");

    if (dateIt != query.end() && timeIt != query.end())
    {
        const std::string& date = dateIt->second;
        const std::vector<std::string>& timeList = split(timeIt->second, ',');

        std::vector<std::string> allTime = {"09:00", "10:00", "11:00", "12:00", "13:00", "14:00",
                                               "15:00", "16:00", "17:00", "18:00", "19:00", "20:00"};
        
        for (const auto& time: allTime)
        {
            if (std::find(timeList.begin(), timeList.end(), time) != timeList.end())
            {
                m_ans->db->add(date, time);
            }
            else
            {
                m_ans->db->deleteTime(date, time);
            }
        }
        m_ans->extRet("ok");
    }
    else
    {
        m_ans->errRet("wrong args");
    }
}

void exeAddObservation::exe()
{
    std::unordered_map<std::string, std::string>& query = m_uri->query;

    auto dateIt = query.find("date");
    auto timeIt = query.find("time");

    if (dateIt != query.end() && timeIt != query.end()) {
        bool success = m_ans->db->add(dateIt->second, timeIt->second);
        if (success)
            m_ans->extRet("ok");
        else
            m_ans->errRet("SQL Error");
    } else
        m_ans->errRet("wrong args");
}

void exeGetObsState::exe()
{
    std::unordered_map<std::string, std::string>& query = m_uri->query;

    auto dateIt = query.find("date");
    auto timeIt = query.find("time");
    auto nameIt = query.find("name");
    auto surnameIt = query.find("surname");
    auto patronymicIt = query.find("patronymic");
    auto typeIt = query.find("type");
    auto forwardedIt = query.find("forwarded");

    if (nameIt != query.end()) {
        bool success = m_ans->db->changeState(nameIt->second,
                                              surnameIt->second,
                                              patronymicIt->second,
                                              typeIt->second,
                                              dateIt->second,
                                              timeIt->second,
                                              forwardedIt->second);
        if (success)
            m_ans->extRet("ok");
        else
            m_ans->errRet("SQL Error");
    } else
        m_ans->errRet("wrong args");
}

void exeDeleteObs::exe()
{
    std::unordered_map<std::string, std::string> query = m_uri->query;

    auto dateIt = query.find("date");
    auto timeIt = query.find("time");

    if (dateIt != query.end() && timeIt != query.end()) {
        bool success = m_ans->db->deleteObs(dateIt->second, timeIt->second);
        if (success)
            m_ans->extRet("ok");
        else
            m_ans->errRet("SQL Error");
    } else
        m_ans->errRet("wrong args");
}

void exeDeleteTime::exe()
{
    std::unordered_map<std::string, std::string> query = m_uri->query;

    auto dateIt = query.find("date");
    auto timeIt = query.find("time");

    if (dateIt != query.end() && timeIt != query.end()) {
        bool success = m_ans->db->deleteTime(dateIt->second, timeIt->second);
        if (success)
            m_ans->extRet("ok");
        else
            m_ans->errRet("SQL Error");
    } else
        m_ans->errRet("wrong args");
}
