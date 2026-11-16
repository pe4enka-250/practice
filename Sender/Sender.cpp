#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

struct Message {
    char text[20];
};

struct FileHeader {
    int head;
    int tail;
    int capacity;
};

int main(int argc, char* argv[]) {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    if (argc < 2) {
        std::cerr << "[Sender] Ошибка: имя файла не передано через командную строку.\n";
        system("pause");
        return 1;
    }

    std::string filename = argv[1];

    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "Local\\QueueMutex");
    HANDLE hSemEmpty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "Local\\SemEmpty");
    HANDLE hSemFull = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "Local\\SemFull");
    HANDLE hSemReady = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "Local\\SemReady");

    if (!hMutex || !hSemEmpty || !hSemFull || !hSemReady) {
        std::cerr << "[Sender] Ошибка подключения к объектам синхронизации. Убедитесь, что запущен Receiver. Код: " << GetLastError() << std::endl;
        system("pause");
        return 1;
    }

    ReleaseSemaphore(hSemReady, 1, NULL);
    std::cout << "[Sender] Процесс готов к работе. Используемый файл: " << filename << "\n";

    while (true) {
        std::cout << "\n--- Меню Sender ---\n"
            << "1. Отправить сообщение процессу Receiver\n"
            << "2. Прекратить свою работу\n"
            << "Выберите действие (1 или 2): ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "[Sender] Введите текст сообщения (строго менее 20 символов): ";
            std::string inputStr;
            std::cin >> inputStr;

            if (inputStr.length() >= 20) {
                std::cout << "[Sender] Ошибка: длина сообщения должна быть меньше 20 символов!\n";
                continue;
            }

            Message msg;
            strncpy(msg.text, inputStr.c_str(), sizeof(msg.text) - 1);
            msg.text[sizeof(msg.text) - 1] = '\0';

            std::cout << "[Sender] Ожидание свободного слота в очереди...\n";
            WaitForSingleObject(hSemEmpty, INFINITE);

            WaitForSingleObject(hMutex, INFINITE);

            std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "[Sender] Ошибка открытия файла для записи.\n";
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hSemEmpty, 1, NULL);
                continue;
            }

            FileHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(header));

            file.seekp(sizeof(FileHeader) + header.tail * sizeof(Message), std::ios::beg);
            file.write(reinterpret_cast<char*>(&msg), sizeof(msg));

            std::cout << "[Sender] Отправлено сообщение: \"" << msg.text << "\" (в позицию " << header.tail << ")\n";

            header.tail = (header.tail + 1) % header.capacity;

            file.seekp(0, std::ios::beg);
            file.write(reinterpret_cast<char*>(&header), sizeof(header));
            file.close();

            ReleaseMutex(hMutex);

            ReleaseSemaphore(hSemFull, 1, NULL);

        }
        else if (choice == 2) {
            std::cout << "[Sender] Завершение работы Sender.\n";
            break;
        }
        else {
            std::cout << "[Sender] Неверный выбор. Повторите попытку.\n";
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hSemEmpty);
    CloseHandle(hSemFull);
    CloseHandle(hSemReady);

    return 0;
}