#include "dbHelper.hpp"

int main() {
    DBHelper db;
    std::string date = "31.01.2024";
    std::string time = "19:00";
    db.add(date, time);
    std::cout << db.getAll().dump() << std::endl;
}