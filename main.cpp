#include "dbHelper.hpp"

int main() {
    DBHelper db;
    std::string date = "01.02.2024";
    std::string time = "17:00";
    db.add(date, time);
    std::cout << db.deleteTime("01.02.2024", "17:00") << std::endl;
}