#include <iostream>
#include <fstream>
#include <cstdlib>
#include "employee.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Использование: Reporter.exe <исходный_файл> <файл_отчета> <оплата_в_час>\n";
        return 1;
    }

    const char* binFilename = argv[1];
    const char* reportFilename = argv[2];
    double payPerHour = std::atof(argv[3]);

    std::ifstream in(binFilename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Ошибка: не удалось открыть бинарный файл.\n";
        return 1;
    }

    std::ofstream out(reportFilename);
    if (!out.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл отчета.\n";
        return 1;
    }

    out << "Отчет по файлу «" << binFilename << "»\n";
    out << "Номер сотрудника, имя сотрудника, часы, зарплата\n";

    employee emp{};
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        double salary = emp.hours * payPerHour;
        out << emp.num << ", " << emp.name << ", " << emp.hours << ", " << salary << "\n";
    }

    in.close();
    out.close();
    return 0;
}