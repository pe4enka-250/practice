#include <iostream>
#include <vector>
#include <windows.h>
#include <cstdlib>

using namespace std;

int* arr = nullptr;
int arraySize = 0;
int markerCount = 0;

CRITICAL_SECTION cs;

HANDLE* startEvents = nullptr;       // Сигнал на начало работы потоков marker
HANDLE* continueEvents = nullptr;    // Сигнал на продолжение работы конкретному потоку
HANDLE* terminateEvents = nullptr;   // Сигнал на завершение работы конкретному потоку
HANDLE mainReadyEvent;               // Сигнал потоку main о невозможности продолжения работы marker'а

struct ThreadData {
    int id;
    bool isTerminated;
    bool isWaitingForMain;
    int markedCount;
    int failedIndex;
};

vector<ThreadData> threadDataList;
vector<HANDLE> hThreads;

DWORD WINAPI MarkerThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    int id = data->id;

    WaitForSingleObject(startEvents[id - 1], INFINITE);

    srand(id);

    while (true) {
        int index = rand() % arraySize;

        EnterCriticalSection(&cs);

        if (arr[index] == 0) {
            LeaveCriticalSection(&cs);
            Sleep(5);
            EnterCriticalSection(&cs);

            if (arr[index] == 0) {
                arr[index] = id;
                data->markedCount++;
                LeaveCriticalSection(&cs);

                Sleep(5);
                continue; 
            }
        }

        data->failedIndex = index;

        cout << "Marker " << id << ": marked elements count = " << data->markedCount
            << ", failed index = " << index << endl;

        LeaveCriticalSection(&cs);

        data->isWaitingForMain = true;
        SetEvent(mainReadyEvent);

        HANDLE waitHandles[2] = { continueEvents[id - 1], terminateEvents[id - 1] };
        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0 + 1) {
            EnterCriticalSection(&cs);
            for (int i = 0; i < arraySize; ++i) {
                if (arr[i] == id) {
                    arr[i] = 0;
                }
            }
            LeaveCriticalSection(&cs);
            break;
        }
        else if (waitResult == WAIT_OBJECT_0) {
            data->isWaitingForMain = false;
        }
    }

    return 0;
}

int main() {
    cout << "Enter array size: ";
    cin >> arraySize;

    arr = new int[arraySize];

    for (int i = 0; i < arraySize; ++i) {
        arr[i] = 0;
    }

    cout << "Enter number of marker threads: ";
    cin >> cinCount = 0;
    cin >> markerCount;

    InitializeCriticalSection(&cs);

    startEvents = new HANDLE[markerCount];
    continueEvents = new HANDLE[markerCount];
    terminateEvents = new HANDLE[markerCount];

    threadDataList.resize(markerCount);
    hThreads.resize(markerCount);

    mainReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < markerCount; ++i) {
        startEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        continueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        terminateEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL); 

        threadDataList[i].id = i + 1;
        threadDataList[i].isTerminated = false;
        threadDataList[i].isWaitingForMain = false;
        threadDataList[i].markedCount = 0;
        threadDataList[i].failedIndex = -1;

        hThreads[i] = CreateThread(NULL, 0, MarkerThread, &threadDataList[i], 0, NULL);
    }

    for (int i = 0; i < markerCount; ++i) {
        SetEvent(startEvents[i]);
    }

    int activeThreads = markerCount;

    while (activeThreads > 0) {
        WaitForSingleObject(mainReadyEvent, INFINITE);

        bool allWaiting = true;
        for (int i = 0; i < markerCount; ++i) {
            if (!threadDataList[i].isTerminated && threadDataList[i].isWaitingForMain) {
            }
        }

        EnterCriticalSection(&cs);
        cout << "\nCurrent array state:\n";
        for (int i = 0; i < arraySize; ++i) {
            cout << arr[i] << " ";
        }
        cout << "\n\n";
        LeaveCriticalSection(&cs);

        int targetId;
        cout << "Enter marker thread ID to terminate (1 to " << markerCount << "): ";
        cin >> targetId;

        SetEvent(terminateEvents[targetId - 1]);
        threadDataList[targetId - 1].isTerminated = true;

        WaitForSingleObject(hThreads[targetId - 1], INFINITE);

        EnterCriticalSection(&cs);
        cout << "Array state after terminating marker " << targetId << ":\n";
        for (int i = 0; i < arraySize; ++i) {
            cout << arr[i] << " ";
        }
        cout << "\n\n";
        LeaveCriticalSection(&cs);

        activeThreads--;
        if (activeThreads == 0) {
            break;
        }

        for (int i = 0; i < markerCount; ++i) {
            if (!threadDataList[i].isTerminated) {
                SetEvent(continueEvents[i]);
            }
        }
    }

    cout << "All marker threads terminated. Main thread exiting." << endl;

    DeleteCriticalSection(&cs);
    CloseHandle(mainReadyEvent);
    for (int i = 0; i < markerCount; ++i) {
        CloseHandle(startEvents[i]);
        CloseHandle(continueEvents[i]);
        CloseHandle(terminateEvents[i]);
        CloseHandle(hThreads[i]);
    }

    delete[] arr;
    delete[] startEvents;
    delete[] continueEvents;
    delete[] terminateEvents;

    return 0;
}