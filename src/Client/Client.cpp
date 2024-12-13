#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

std::vector<std::wstring> getAvailableCOMPorts() {
    std::vector<std::wstring> ports;
    for (int i = 1; i <= 256; ++i) {
        std::wstring portName = L"\\\\.\\COM" + std::to_wstring(i);
        HANDLE hTest = CreateFileW(
            portName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hTest != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(hTest);
        }
    }
    return ports;
}

int main() {
    while (true) {
        std::vector<std::wstring> comPorts = getAvailableCOMPorts();

        if (comPorts.empty()) {
            std::cerr << "No COM ports available.\n";
            return 1;
        }

        std::wcout << L"Available COM Ports:\n";
        for (size_t i = 0; i < comPorts.size(); ++i) {
            std::wcout << i + 1 << L": " << comPorts[i].substr(4) << L"\n";
        }

        size_t portIndex;
        std::wcout << L"Select COM port (enter number): ";
        std::wcin >> portIndex;

        if (portIndex < 1 || portIndex > comPorts.size()) {
            std::cerr << "Invalid selection.\n";
            return 1;
        }

        std::wstring selectedPort = comPorts[portIndex - 1];

        HANDLE hSerial = CreateFileW(
            selectedPort.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening COM port.\n";
            return 1;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error getting COM port state.\n";
            CloseHandle(hSerial);
            return 1;
        }

        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error setting COM port state.\n";
            CloseHandle(hSerial);
            return 1;
        }

        const char* data = "Hello, Server\n";
        DWORD bytesWritten;
        if (!WriteFile(hSerial, data, strlen(data), &bytesWritten, NULL)) {
            std::cerr << "Error writing to COM port.\n";
            CloseHandle(hSerial);
            return 1;
        }
        std::cout << "Sent: " << data << std::endl;

        char buffer[256];
        DWORD bytesRead;
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            std::cout << "Received: " << buffer << std::endl;
        }
        else {
            std::cerr << "Error reading from COM port.\n";
        }

        CloseHandle(hSerial);

        char choice;
        std::cout << "Do you want to send message again? (y/n): ";
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }

    return 0;
}
