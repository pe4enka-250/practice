#include <iostream>
#include <windows.h>

int* arr;
int arr_size;
int min_idx = 0;
int max_idx = 0;
double average_val = 0;

DWORD WINAPI min_max(LPVOID lpParam) {
    int min_val = arr[0];
    int max_val = arr[0];

    for (int i = 1; i < arr_size; ++i) {
        if (arr[i] < min_val) {
            min_val = arr[i];
            min_idx = i;
        }
        Sleep(7);

        if (arr[i] > max_val) {
            max_val = arr[i];
            max_idx = i;
        }
        Sleep(7);
    }

    std::cout << "Минимальный элемент: " << min_val << "\n";
    std::cout << "Максимальный элемент: " << max_val << "\n";

    return 0;
}

DWORD WINAPI average(LPVOID lpParam) {
    int sum = 0;

    for (int i = 0; i < arr_size; ++i) {
        sum += arr[i];
        Sleep(12);
    }

    average_val = (double)sum / arr_size;

    std::cout << "Среднее арифметическое: " << average_val << "\n";

    return 0;
}

int main() {
    std::cout << "Введите размерность массива: ";
    std::cin >> arr_size;

    arr = new int[arr_size];

    std::cout << "Введите элементы массива: ";
    for (int i = 0; i < arr_size; ++i) {
        std::cin >> arr[i];
    }

    HANDLE hMinMax, hAverage;

    hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    arr[min_idx] = (int)average_val;
    arr[max_idx] = (int)average_val;

    std::cout << "Массив после замены элементов:\n";
    for (int i = 0; i < arr_size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";

    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete[] arr;

    return 0;
}