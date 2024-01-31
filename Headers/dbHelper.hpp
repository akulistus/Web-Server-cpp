#ifndef DBHELPER_H
#define DBHELPER_H
#include <iostream>
#include <string>
#include <list>
#include <variant>
#include <filesystem>
#include <ctime>
#include "sqlite3.h"
#include "nlohmann/json.hpp"

class DBHelper
{
public:
    DBHelper();
    ~DBHelper();
    nlohmann::json getAll();
    nlohmann::json getFree();
    nlohmann::json getAllDates();
    nlohmann::json getDay(std::string day);
    bool add(std::string date, std::string time);
    bool changeState(std::string name,
                     std::string surname,
                     std::string patronymic,
                     std::string type,
                     std::string date,
                     std::string time,
                     std::string forw);
    bool deleteObs(std::string date, std::string time);
    bool deleteTime(std::string date, std::string time);
    bool isValidDateTime(const std::string& date, const std::string& time);

private:

protected:
    sqlite3* db;
    bool checkDateTime(std::string date, std::string time);
    //    inline void swap(QJsonValueRef& v1, QJsonValueRef& v2){
    //        QJsonValue t(v1);
    //        v1 = QJsonValue(v2);
    //        v2 = t;
    //    }
    // static bool compare(const std::variant &v1, const std::variant &v2)
    // {
    //     if (v1.toMap().find("time")->toTime().hour() < v2.toMap().find("time")->toTime().hour())
    //         return true;
    //     else
    //         return false;
    // }
};

#endif // DBHELPER_H
