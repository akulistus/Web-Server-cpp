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
    std::sscanf(time.c_str(), "%d:%d", &dateStruct.tm_hour, &dateStruct.tm_min);

    dateStruct.tm_year -= 1900;
    dateStruct.tm_mon -= 1;

    std::time_t tmptime = std::mktime(&dateStruct);
    dateStruct.tm_yday = std::localtime(&tmptime)->tm_yday;

    
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    
    if ( (dateStruct.tm_yday - localTime->tm_yday >= 0) || (dateStruct.tm_year > localTime->tm_year) ) {
        if (!(dateStruct.tm_hour - localTime->tm_hour <= 0 && dateStruct.tm_yday == localTime->tm_yday)) {
            std::cout << "here" << std::endl;
            return true;
        }
    }
    return false;
}

bool DBHelper::isValidDate(const std::string& date, const int state) {
    std::tm dateStruct = {};

    std::sscanf(date.c_str(), "%d.%d.%d", &dateStruct.tm_mday, &dateStruct.tm_mon, &dateStruct.tm_year);

    dateStruct.tm_year -= 1900;
    dateStruct.tm_mon -= 1;

    std::time_t tmptime = std::mktime(&dateStruct);
    dateStruct.tm_yday = std::localtime(&tmptime)->tm_yday;

    
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    if ((dateStruct.tm_yday - localTime->tm_yday >= 0) || (dateStruct.tm_year > localTime->tm_year) || (state == 1) ){
        return true;
    }
    return false;
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
    json arr = json::array();

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

            //Почемну не могу вернуть nullptr
            if (isValidDateTime(fieldDate, fieldTime)) {
                arr.push_back({
                    {"state", fieldState},
                    {"date", fieldDate},
                    {"time", fieldTime},
                    {"name", fieldName ? fieldName : ""},
                    {"surname", fieldSurname ? fieldSurname : ""},
                    {"patronymic", fieldPatr ? fieldPatr : ""},
                    {"type", fieldType ? fieldType : ""},
                    {"forwarded", fieldForw ? fieldForw : ""}
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

                std::string insertSql = "INSERT INTO calendar (state, date, time) VALUES (0, '" + date + "', '" + time + "');";

                if (sqlite3_exec(db, insertSql.c_str(), NULL, NULL, NULL) == SQLITE_OK) {
                    success = true;
                }
            } else {
                success = false;
            }
        }
        sqlite3_finalize(stmt);
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

nlohmann::json DBHelper::getAllDates()
{
    if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    sqlite3_stmt* stmt;

    std::string sqlRequest = "SELECT * FROM calendar";

    std::unordered_map<std::string, int> hash;

    if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int fieldState = sqlite3_column_int(stmt, 1);
            const char* fieldDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            if (isValidDate(fieldDate, fieldState)){
                hash[fieldDate] = fieldState;
            }
        }

        sqlite3_finalize(stmt);
    }

    nlohmann::json qjo = json({});
    for (const auto &entry : hash) {
        qjo[entry.first] = entry.second;
    }

    sqlite3_close(db);

    return qjo;
}

nlohmann::json DBHelper::getFree()
{
    if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    sqlite3_stmt* stmt;

    std::string sqlRequest = "SELECT * FROM calendar";
    std::unordered_map<std::string, std::multiset<std::string>> hash;
    if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int fieldState = sqlite3_column_int(stmt, 1);
            if (fieldState == 0)
            {
                const char* fieldDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                const char* fieldTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

                if (isValidDateTime(fieldDate, fieldTime)){
                    hash[fieldDate].insert(fieldTime);
                }
            }
        }

        sqlite3_finalize(stmt);
    }

    nlohmann::json qjo = json({});
    for (const auto &entry : hash) {
        json arr = json::array();
        for (const auto &timestmp : entry.second) {
            arr.push_back(timestmp);
        }
        qjo[entry.first] = arr;
    }

    sqlite3_close(db);

    return qjo;
}

nlohmann::json DBHelper::getDay(std::string date)
{

    if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    }

    sqlite3_stmt* stmt;

    std::string sqlRequest = "SELECT * FROM calendar WHERE date = '" + date + "';";

    json arr = json::array();

    if(sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int fieldState = sqlite3_column_int(stmt, 1);
            const char* fieldTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* fieldName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* fieldSurname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            const char* fieldPatr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            const char* fieldType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            const char* fieldForw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

            //Почемну не могу вернуть nullptr
            //Если просто нул, то осущь. запись
            if (isValidDateTime(date, fieldTime) || fieldState == 1) {
                arr.push_back({
                    {"state", fieldState},
                    {"time", fieldTime},
                    {"name", fieldName ? fieldName : ""},
                    {"surname", fieldSurname ? fieldSurname : ""},
                    {"patronymic", fieldPatr ? fieldPatr : ""},
                    {"type", fieldType ? fieldType : ""},
                    {"forwarded", fieldForw ? fieldForw : ""}
                });
            }
        }

        sqlite3_finalize(stmt);
    }

    // Починить datetime и compare
    // И ещё DateTime в функции выше, которое удалил
    // std::sort(arr.begin(), arr.end());
    json result = {
        {"datetime", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()},
        {"data", arr}
    };

    sqlite3_close(db);
    return result;
}

bool DBHelper::deleteObs(std::string date, std::string time)
{
    bool success = false;
    if (checkDateTime(date, time)) {

        if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
            std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }

        sqlite3_stmt* stmt;

        std::string sqlRequest = "SELECT * FROM calendar WHERE date = '" + date + "' AND time = '" + time + "' AND state = 1;";

        if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                sqlite3_finalize(stmt);

                std::string updateSql = "UPDATE calendar SET name = NULL, surname = NULL, patronymic = NULL, type = NULL, "
                                        "forwarded = NULL, state=0 WHERE date= '" + date + "' AND time= '" + time + "' AND state=1;";

                if (sqlite3_exec(db, updateSql.c_str(), NULL, NULL, NULL) == SQLITE_OK) {
                    success = true;
                }
            }
        }
        
        sqlite3_close(db);
    }
    return success;   
}

// Если сделать add и сразу delete, то работает через раз.
bool DBHelper::deleteTime(std::string date, std::string time)
{
    bool success = false;
    if (checkDateTime(date, time)) {

        if (sqlite3_open("../data/calendar.sqlite", &db) != SQLITE_OK) {
            std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }

        sqlite3_stmt* stmt;

        std::string sqlRequest = "SELECT * FROM calendar WHERE date = '" + date + "' AND time = '" + time + "' AND state = 0;";

        if (sqlite3_prepare_v2(db, sqlRequest.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                sqlite3_finalize(stmt);

                std::string deleteSql = "DELETE FROM calendar WHERE date = '" + date + "' AND time = '" + time + "' AND state = 0;";

                if (sqlite3_exec(db, deleteSql.c_str(), NULL, NULL, NULL) == SQLITE_OK) {
                    success = true;
                }
            }
        }
        
        sqlite3_close(db);
    }
    return success;
}
