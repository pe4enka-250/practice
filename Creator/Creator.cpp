#include <iostream>
#include <fstream>
#include <cstdlib>
#include "employee.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: Creator.exe <имя_файла> <количество_записей>\n";
        return 1;
    }

    const char* filename = argv[1];
    int count = std::atoi(argv[2]);

    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл.\n";
        return 1;
    }

    for (int i = 0; i < count; ++i) {
        employee emp{};
        std::cout << "Введите ID, имя и количество часов " << i + 1 << " Сотрудника: ";
        std::cin >> emp.num >> emp.name >> emp.hours;
        out.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
    }

    out.close();
    return 0;
}