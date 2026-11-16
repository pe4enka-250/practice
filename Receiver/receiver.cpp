#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Структура сообщения (максимальная длина текста < 20 символов)
struct Message {
    char text[20];
};

// Заголовок бинарного файла (кольцевая очередь)
struct FileHeader {
    int head;     // индекс для чтения
    int tail;     // индекс для записи
    int capacity; // максимальное количество записей
};

int main() {
    // Установка кодировки консоли для корректного отображения кириллицы
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::string filename;
    int capacity;
    int senderCount;

    std::cout << "[Receiver] Введите имя бинарного файла (например, messages.bin): ";
    std::cin >> filename;
    std::cout << "[Receiver] Введите количество записей в бинарном файле: ";
    std::cin >> capacity;
    std::cout << "[Receiver] Введите количество процессов Sender: ";
    std::cin >> senderCount;

    // 1. Создание бинарного файла для сообщений и инициализация заголовка
    {
        std::ofstream outFile(filename, std::ios::binary);
        FileHeader header = { 0, 0, capacity };
        outFile.write(reinterpret_cast<char*>(&header), sizeof(header));

        Message emptyMsg = { "" };
        for (int i = 0; i < capacity; ++i) {
            outFile.write(reinterpret_cast<char*>(&emptyMsg), sizeof(emptyMsg));
        }
    }

    // Создание именованных объектов синхронизации Win32 API
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Local\\QueueMutex");
    HANDLE hSemEmpty = CreateSemaphoreA(NULL, capacity, capacity, "Local\\SemEmpty");
    HANDLE hSemFull = CreateSemaphoreA(NULL, 0, capacity, "Local\\SemFull");
    HANDLE hSemReady = CreateSemaphoreA(NULL, 0, senderCount, "Local\\SemReady");

    if (!hMutex || !hSemEmpty || !hSemFull || !hSemReady) {
        std::cerr << "[Receiver] Ошибка создания объектов синхронизации. Код: " << GetLastError() << std::endl;
        return 1;
    }

    // 3. Запуск заданного количества процессов Sender через командную строку
    std::vector<HANDLE> senderProcessHandles;
    for (int i = 0; i < senderCount; ++i) {
        std::string cmdLine = "sender.exe " + filename;
        std::vector<char> writableCmdLine(cmdLine.begin(), cmdLine.end());
        writableCmdLine.push_back('\0');

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Запуск дочернего процесса в новом окне консоли
        if (!CreateProcessA(
            NULL,
            writableCmdLine.data(),
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            std::cerr << "[Receiver] Ошибка запуска Sender " << i + 1 << ". Код: " << GetLastError() << std::endl;
        }
        else {
            senderProcessHandles.push_back(pi.hProcess);
            CloseHandle(pi.hThread); // Закрываем ненужный дескриптор потока
        }
    }

    // 4. Ожидание сигнала готовности от всех процессов Sender
    std::cout << "[Receiver] Ожидание готовности всех процессов Sender...\n";
    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(hSemReady, INFINITE);
    }
    std::cout << "[Receiver] Все процессы Sender готовы к работе!\n";

    // 5. Цикл обработки консольных команд для Receiver
    while (true) {
        std::cout << "\n--- Меню Receiver ---\n"
            << "1. Читать сообщение из бинарного файла\n"
            << "2. Завершить работу\n"
            << "Выберите действие (1 или 2): ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            // Ожидание появления заполненного слота (если файл пуст, процесс засыпает)
            std::cout << "[Receiver] Ожидание сообщения в буфере...\n";
            WaitForSingleObject(hSemFull, INFINITE);

            // Захват мьютекса для эксклюзивного доступа к файлу
            WaitForSingleObject(hMutex, INFINITE);

            std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "[Receiver] Ошибка открытия файла.\n";
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hSemFull, 1, NULL);
                continue;
            }

            // Читаем заголовок
            FileHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(header));

            // Читаем сообщение из текущей позиции head
            file.seekg(sizeof(FileHeader) + header.head * sizeof(Message), std::ios::beg);
            Message msg;
            file.read(reinterpret_cast<char*>(&msg), sizeof(msg));

            std::cout << "[Receiver] ПРОЧИТАНО СООБЩЕНИЕ: \"" << msg.text << "\" (из позиции " << header.head << ")\n";

            // Сдвигаем head кольцевой очереди
            header.head = (header.head + 1) % header.capacity;

            // Перезаписываем обновленный заголовок
            file.seekp(0, std::ios::beg);
            file.write(reinterpret_cast<char*>(&header), sizeof(header));
            file.close();

            // Освобождаем мьютекс
            ReleaseMutex(hMutex);

            // Увеличиваем счетчик свободных мест
            ReleaseSemaphore(hSemEmpty, 1, NULL);

        }
        else if (choice == 2) {
            std::cout << "[Receiver] Завершение работы Receiver.\n";
            break;
        }
        else {
            std::cout << "[Receiver] Неверный выбор. Повторите попытку.\n";
        }
    }

    // Очистка ресурсов
    CloseHandle(hMutex);
    CloseHandle(hSemEmpty);
    CloseHandle(hSemFull);
    CloseHandle(hSemReady);

    for (HANDLE hProc : senderProcessHandles) {
        CloseHandle(hProc);
    }

    return 0;
}