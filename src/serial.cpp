#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <print>
#include <vector>

#include "serial/serial.h"

using namespace std::chrono_literals;

enum class Peripheral {
    NO_PERIPHERAL,
    BUS_SERVO,
    BUZZER,
    LED,
};

std::vector<uint8_t> query_servos_angle{
    0x55, // Header
    0x55, 
    static_cast<uint8_t>(Peripheral::BUS_SERVO), // Peripheral
    0x04, // Data Length
    0x1C, // Command
    0x06, // Servo Count
    0x01, // Servos Id
    0x02,
    0x03,
    0x04,
    0x05,
    0x06
};

std::vector<uint8_t> blink_led{
    0x55, // Header
    0x55, 
    static_cast<uint8_t>(Peripheral::LED), // Peripheral
    0x0C, // Data Length
    0x01, // LED Count
    0x00, // LED Id
    0xE8, // On
    0x03,
    0x00,
    0x00,
    0xE8, // Off
    0x03,
    0x00,
    0x00,
    0x05, // Repeat Count
    0x00,
};

std::vector<uint8_t> beep{
    0x55, // Header
    0x55, 
    static_cast<uint8_t>(Peripheral::BUZZER), // Peripheral
    0x0E, // Data Length
    0xDC, // Frequency
    0x05,
    0x00, 
    0x00, 
    0x64, // On Duration
    0x00, 
    0x00, 
    0x00, 
    0xf4, // Off Duration
    0x01, 
    0x00, 
    0x00, 
    0x02, // Repeat Count
    0x00 
};

void list_ports() {
    auto devices = serial::list_ports();
    for (const auto& device : devices) {
        std::cout << device.port.c_str() << " | "
                << device.description.c_str() << " | "
                << device.hardware_id.c_str()
                << std::endl;
    }
}

void receive_packet(serial::Serial& serial) {
    while (true) {
        // auto msg = serial.read();
        // if (!msg.empty())
        //     std::cout << std::hex << std::showbase << static_cast<int>(*msg.begin()) << std::endl;

        auto msg = serial.readline();
        if (!msg.empty()) 
            std::cout << "Received:" << msg<< std::endl;
    }
}

int main() {
    serial::Serial serial = serial::Serial("", 115200, serial::Timeout::simpleTimeout(1000));
    serial.setBytesize(serial::eightbits);
    serial.setParity(serial::parity_none);
    serial.setStopbits(serial::stopbits_one);
    serial.setFlowcontrol(serial::flowcontrol_none);

    // Set serial port and open
    try {
        // serial.setPort("/dev/ttyUSB0");
        serial.setPort("COM6");
        serial.open();
    } catch (const serial::IOException& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    if (!serial.isOpen())
        std::cout << "Failed to open." << std::endl;

    std::thread rx_th(receive_packet, std::ref(serial));
    
    // Sending msg
    std::cout << "Start to post..." << std::endl;
    serial.write(query_servos_angle);

    rx_th.join();

    return 0;
}