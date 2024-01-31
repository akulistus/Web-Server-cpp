#include "dbHelper.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

DBHelper::DBHelper()
{
    if (!fs::exists("../data"))
        fs::create_directories("../data");

    int rc = sqlite3_open("../data/calendar.sqlite", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    rc = sqlite3_exec(db,   "CREATE TABLE IF NOT EXISTS calendar ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "state INT NOT NULL, "
                            "date TEXT NOT NULL, "
                            "time TEXT NOT NULL, "
                            "name TEXT, "
                            "surname TEXT, "
                            "patronymic TEXT, "
                            "type TEXT, "
                            "forwarded TEXT);", 0,0,0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error creating table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    sqlite3_close(db);
    std::cout << "Table created successfully!" << std::endl;
}

DBHelper::~DBHelper()
{
    sqlite3_close(db);
}

// Переделать. Прошедшие записи не удаляются из базы...
// Или не нужно
bool DBHelper::isValidDateTime(const std::string& date, const std::string& time) {

    
    std::tm dateStruct = {};

    std::sscanf(date.c_str(), "%d.%d.%d", &dateStruct.tm_mday, &dateStruct.tm_mon, &dateStruct.tm_year);

    dateStruct.tm_year -= 1900;
    dateStruct.tm_mon -= 1;

    // Convert the struct tm to time_t
    std::time_t tmptime = std::mktime(&dateStruct);
    dateStruct.tm_yday = std::localtime(&tmptime)->tm_yday;
    std::cout << dateStruct.tm_yday <<std::endl;

    
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    
    // std::cout << tmDate.tm_year << std::endl;
    // std::cout << std::localtime(&currentDate)->tm_year << std::endl;
    // std::cout << tmTime.tm_hour << std::endl;
    if ( (localTime->tm_yday - dateStruct.tm_yday < 0) || (dateStruct.tm_year > localTime->tm_year) ) {

    }
    return 1;
}

bool DBHelper::checkDateTime(std::string date, std::string time)
{
    std::list<std::string> list = {"09:00",
                           "10:00",
                           "11:00",
                           "12:00",
                           "13:00",
                           "14:00",
                           "15:00",
                           "16:00",
                           "17:00",
                           "18:00",
                           "19:00",
                           "20:00"};
    return (std::find(list.begin(), list.end(), time) != list.end());
}

nlohmann::json DBHelper::getAll()
{
    if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    sqlite3_stmt* stmt;

    std::string sqlRequest = "SELECT * FROM calendar";
    json arr;

    if(sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int fieldState = sqlite3_column_int(stmt, 1);
            const char* fieldDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* fieldTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* fieldName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* fieldSurname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            const char* fieldPatr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            const char* fieldType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            const char* fieldForw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

            if (isValidDateTime(fieldDate, fieldTime)) {
                arr.push_back({
                    {"state", fieldState},
                    {"date", fieldDate},
                    {"time", fieldTime},
                    {"name", fieldName},
                    {"surname", fieldSurname},
                    {"patronymic", fieldPatr},
                    {"type", fieldType},
                    {"forwarded", fieldForw}
                });
            }
        }

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    return arr;
}

bool DBHelper::add(std::string date, std::string time)
{
    bool success;
    if (checkDateTime(date, time)) {
        if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
            std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }


        std::string sqlRequest = "SELECT * FROM calendar WHERE date = '" + date + "' AND time = '" + time + "';";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) != SQLITE_ROW) {
                sqlite3_finalize(stmt);

                std::string insertSql = "INSERT INTO calendar (state, date, time) VALUES (0, '" + date + "', '" + time + "');";

                if (sqlite3_exec(db, insertSql.c_str(), NULL, NULL, NULL) == SQLITE_OK) {
                    success = true;
                }
            } else {
                success = false;
            }
            sqlite3_finalize(stmt);
        }

        sqlite3_close(db);
    }

    return success;
}

bool DBHelper::changeState(std::string name,
                           std::string surname,
                           std::string patronymic,
                           std::string type,
                           std::string date,
                           std::string time,
                           std::string forw)
{
    bool success = false;
    if (checkDateTime(date, time)) {

        if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
            std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }

        std::string sqlRequest = "SELECT * FROM calendar WHERE date = '" + date +"' AND time = '" + time +"' AND state = 0;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {

                sqlite3_finalize(stmt);

                std::string updateSql = "UPDATE calendar SET name = '" + name + "', surname = '" + surname +
                                        "', patronymic = '" + patronymic + "', type = '" + type +
                                        "', forwarded = '" + forw + "', state = 1 WHERE date = '" + date + "' AND time = '" + time + "' AND state = 0;";
                if (sqlite3_exec(db, updateSql.c_str(), NULL, NULL, NULL) == SQLITE_OK) {
                    success = true;
                }
            }
        }

        sqlite3_close(db);
    }
    return success;
}

// nlohmann::json DBHelper::getAllDates()
// {
//     //db.setDatabaseName("./calendar.sqlite");
//     if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
//         std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
//         sqlite3_close(db);
//     }

//     sqlite3_stmt* stmt;

//     std::string sqlRequest = "SELECT * FROM calendar";

//     if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
//         while (sqlite3_step(stmt) == SQLITE_ROW) {

//         }
//     }
//     int fieldState = query.record().indexOf("state");
//     int fieldDate = query.record().indexOf("date");
//     QHash<std::string, std::string> hash;
//     while (query.next()) {
//         std::string state = query.value(fieldState).toString();
//         std::string date = query.value(fieldDate).toString();
//         if (!(QDate::fromString(date, "d.MM.yyyy").dayOfYear() - QDate::currentDate().dayOfYear()
//               < 0)
//             || (QDate::fromString(date, "d.MM.yyyy").year() > QDate::currentDate().year())
//             || state == "1") {
//             if (hash.keys().contains(date)) {
//                 if (state == "1")
//                     hash[date] = "1";
//             } else
//                 hash.insert(date, state);
//         }
//     }
//     nlohmann::json qjo;
//     foreach (std::string date, hash.keys())
//         qjo.insert(date, hash[date]);
//     db.close();
//     return QJsonDocument(qjo);
// }

// nlohmann::json DBHelper::getFree()
// {
//     //db.setDatabaseName("./calendar.sqlite");
//     db.open();
//     QSqlQuery query(db);
//     query.exec("SELECT * FROM calendar");
//     int fieldState = query.record().indexOf("state");
//     int fieldDate = query.record().indexOf("date");
//     int fieldTime = query.record().indexOf("time");
//     QMultiHash<std::string, std::string> hash;
//     while (query.next()) {
//         std::string state = query.value(fieldState).toString();
//         if (state.toInt() == 0) {
//             std::string date = query.value(fieldDate).toString();
//             std::string time = query.value(fieldTime).toString();
//             if (!(QDate::fromString(date, "d.MM.yyyy").dayOfYear() - QDate::currentDate().dayOfYear()
//                   < 0)
//                 || (QDate::fromString(date, "d.MM.yyyy").year() > QDate::currentDate().year()))
//                 if (!(QTime::fromString(time, "hh:mm").hour() - QTime::currentTime().hour() <= 0
//                       && QDate::fromString(date, "d.MM.yyyy").dayOfYear()
//                              == QDate::currentDate().dayOfYear()))
//                     hash.insert(date, time);
//         }
//     }
//     nlohmann::json qjo;
//     foreach (std::string date, hash.keys()) {
//         QList<std::string> values = hash.values(date);
//         QJsonArray arr;
//         values.sort();
//         foreach (std::string val, values) {
//             arr.append(val);
//         }
//         qjo.insert(date, arr);
//     }
//     db.close();
//     return QJsonDocument(qjo);
// }

// nlohmann::json DBHelper::getDay(std::string date)
// {
//     //db.setDatabaseName("./calendar.sqlite");
//     db.open();
//     QSqlQuery query(db);
//     query.prepare("SELECT * FROM calendar WHERE date = ?");
//     query.bindValue(0, date);
//     query.exec();
//     QJsonArray arr;
//     int fieldState = query.record().indexOf("state");
//     int fieldTime = query.record().indexOf("time");
//     int fieldName = query.record().indexOf("name");
//     int fieldSurname = query.record().indexOf("surname");
//     int fieldPatr = query.record().indexOf("patronymic");
//     int fieldType = query.record().indexOf("type");
//     int fieldForw = query.record().indexOf("forwarded");
//     while (query.next()) {
//         std::string state = query.value(fieldState).toString();
//         std::string time = query.value(fieldTime).toString();
//         std::string name = query.value(fieldName).toString();
//         std::string surname = query.value(fieldSurname).toString();
//         std::string patronymic = query.value(fieldPatr).toString();
//         std::string type = query.value(fieldType).toString();
//         std::string forw = query.value(fieldForw).toString();
//         if (!(QDate::fromString(date, "d.MM.yyyy").dayOfYear() - QDate::currentDate().dayOfYear()
//               < 0)
//             || (QDate::fromString(date, "d.MM.yyyy").year() > QDate::currentDate().year())
//             || state == "1")
//             if (!(QTime::fromString(time, "hh:mm").hour() - QTime::currentTime().hour() <= 0
//                   && QDate::fromString(date, "d.MM.yyyy").dayOfYear()
//                          == QDate::currentDate().dayOfYear())
//                 || state == "1")
//                 arr.append(QJsonObject({{"state", state},
//                                         {"time", time},
//                                         {"name", name},
//                                         {"surname", surname},
//                                         {"patronymic", patronymic},
//                                         {"type", type},
//                                         {"forwarded", forw}}));
//     }
//     QVariantList list = arr.toVariantList();
//     std::sort(list.begin(), list.end(), compare);
//     arr = QJsonArray::fromVariantList(list);
//     db.close();
//     QJsonObject qjo = {{"datetime", QDateTime::currentDateTime().toMSecsSinceEpoch()},
//                        {"data", arr}};
//     return QJsonDocument(qjo);
// }

// bool DBHelper::deleteObs(std::string date, std::string time)
// {
//     bool success;
//     if (checkDateTime(date, time)) {
//         //db.setDatabaseName("./calendar.sqlite");
//         db.open();
//         QSqlQuery query(db);
//         query.prepare("SELECT * FROM calendar WHERE date = ? AND time = ? AND state = 1");
//         query.bindValue(0, date);
//         query.bindValue(1, time);
//         query.exec();
//         if (query.first()) {
//             QSqlQuery query1(db);
//             query1.prepare(
//                 "UPDATE calendar SET name = NULL, surname = NULL, patronymic = NULL, type = NULL, "
//                 "forwarded = NULL, state=0 WHERE date=? AND time=? AND state=1");
//             query1.bindValue(0, date);
//             query1.bindValue(1, time);
//             success = query1.exec();

//         } else
//             success = false;
//         db.close();
//     } else {
//         success = false;
//     }
//     return success;
// }

// bool DBHelper::deleteTime(std::string date, std::string time)
// {
//     bool success;
//     if (checkDateTime(date, time)) {
//         //db.setDatabaseName("./calendar.sqlite");
//         db.open();
//         QSqlQuery query(db);
//         query.prepare("SELECT * FROM calendar WHERE date = ? AND time = ? AND state=0");
//         query.bindValue(0, date);
//         query.bindValue(1, time);
//         query.exec();
//         if (query.first()) {
//             QSqlQuery query1(db);
//             query1.prepare("DELETE FROM calendar WHERE date = ? AND time = ? AND state=0");
//             query1.bindValue(0, date);
//             query1.bindValue(1, time);
//             success = query1.exec();
//         } else
//             success = false;
//         db.close();
//     } else {
//         success = false;
//     }
//     return success;
// }
