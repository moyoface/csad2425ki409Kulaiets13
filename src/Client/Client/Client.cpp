#include <windows.h>
#include <iostream>

int main() {
    HANDLE hSerial = CreateFileW(
        L"\\\\.\\COM3",
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
    return 0;
}
