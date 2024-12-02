#include <windows.h>
#include <iostream>
#include <cassert>

void test_open_com_port() {
    HANDLE hSerial = CreateFileW(
        L"\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    assert(hSerial != INVALID_HANDLE_VALUE);
    CloseHandle(hSerial);
}

void test_configure_com_port() {
    HANDLE hSerial = CreateFileW(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    BOOL result = GetCommState(hSerial, &dcbSerialParams);
    assert(result);

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    result = SetCommState(hSerial, &dcbSerialParams);
    assert(result);

    CloseHandle(hSerial);
}

void test_send_data() {
    HANDLE hSerial = CreateFileW(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    const char* data = "Test message\n";
    DWORD bytesWritten;
    BOOL result = WriteFile(hSerial, data, strlen(data), &bytesWritten, NULL);

    assert(result);
    assert(bytesWritten == strlen(data));

    CloseHandle(hSerial);
}

void test_receive_data() {
    HANDLE hSerial = CreateFileW(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    char buffer[256];
    DWORD bytesRead;
    BOOL result = ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

    assert(result);
    buffer[bytesRead] = '\0';
    std::cout << "Received: " << buffer << std::endl;

    CloseHandle(hSerial);
}

int main() {
    test_open_com_port();
    std::cout << "COM port opened successfully.\n";

    test_configure_com_port();
    std::cout << "COM port configured successfully.\n";

    test_send_data();
    std::cout << "Data sent successfully.\n";

    test_receive_data();
    std::cout << "Data received successfully.\n";

    return 0;
}
