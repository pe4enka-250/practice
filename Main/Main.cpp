#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include "employee.h"

void printBinaryFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        return;
    }

    employee emp{};
    std::cout << "\n--- Содержимое бинарного файла ---\n";
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << "ID: " << emp.num << ", Имя: " << emp.name << ", Часы: " << emp.hours << "\n";
    }
    std::cout << "----------------------------------\n\n";
}

void printTextFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        return;
    }

    std::string line;
    std::cout << "\n--- Содержимое файла отчета ---\n";
    while (std::getline(in, line)) {
        std::cout << line << "\n";
    }
    std::cout << "-------------------------------\n";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    std::string binFilename, reportFilename;
    int recordsCount{};
    double payPerHour{};

    std::cout << "Введите имя создаваемого бинарного файла: ";
    std::cin >> binFilename;
    std::cout << "Введите количество записей: ";
    std::cin >> recordsCount;

    std::string creatorCmd = "Creator.exe " + binFilename + " " + std::to_string(recordsCount);
    std::vector<char> creatorBuffer(creatorCmd.begin(), creatorCmd.end());
    creatorBuffer.push_back('\0');

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessA(NULL, creatorBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else {
        std::cerr << "Не удалось запустить Creator.exe\n";
        return 1;
    }

    printBinaryFile(binFilename);

    std::cout << "Введите имя файла отчета: ";
    std::cin >> reportFilename;
    std::cout << "Введите оплату за час работы: ";
    std::cin >> payPerHour;

    std::string reporterCmd = "Reporter.exe " + binFilename + " " + reportFilename + " " + std::to_string(payPerHour);
    std::vector<char> reporterBuffer(reporterCmd.begin(), reporterCmd.end());
    reporterBuffer.push_back('\0');

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(NULL, reporterBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else {
        std::cerr << "Не удалось запустить Reporter.exe\n";
        return 1;
    }

    printTextFile(reportFilename);

    return 0;
}