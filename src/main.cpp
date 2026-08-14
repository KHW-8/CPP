#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <syncstream>
#include <print>

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
    0x1C,
    0x02,
    0x01, 
    0x02
};

std::vector<uint8_t> blink_led{
    0x55,  // Header
    0x55, 
    static_cast<uint8_t>(Peripheral::LED), // Peripheral
    0x0C, // Data Length
    0x01, // LED Count
    0x01, // LED Id
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
    0x55, 
    0x55, 
    static_cast<uint8_t>(Peripheral::BUZZER)
};

void list_ports() {
    auto devices = serial::list_ports();
    for (const auto& device : devices) {
        std::osyncstream(std::cout) << device.port.c_str() << " | "
                << device.description.c_str() << " | "
                << device.hardware_id.c_str()
                << std::endl;
    }
}

void receive_packet(serial::Serial& serial) {
    std::osyncstream(std::cout) << "Start to receive" << std::endl;

    while (true) {
        auto msg = serial.readline();
        if (!msg.empty()) 
            std::osyncstream(std::cout) << "Received:" << msg<< std::endl;
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
        std::osyncstream(std::cout) << "Failed to open." << std::endl;

    std::thread rx_th(receive_packet, std::ref(serial));
    
    // Sending msg
    // std::osyncstream(std::cout) << "Start to post..." << std::endl;
    // serial.write(blink_led);

    rx_th.join();

    return 0;
}