#include <print>
#include <iostream>
#include <thread>
#include "boost/asio.hpp"

enum class Peripheral {
    NO_PERIPHERAL,
    BUS_SERVO,
    BUZZER,
    LED,
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

auto receive_packet(boost::asio::serial_port& serial) -> void {
    // Read data from the serial port
    while (true) {
        boost::asio::streambuf response;
        boost::asio::read_until(serial, response, "\n");

        std::istream is(&response);
        std::string line;
        std::getline(is, line);

        std::cout << "Received: " << line << std::endl;
    }
}

auto main(int argc, char** argv) -> int {
    // Initialize the Asio I/O context
    boost::asio::io_context io;
    
    // Create serial port
    boost::asio::serial_port serial(io);

    // Open the serial port
    try {
        serial.open("COM3");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Configure serial port
    serial.set_option(boost::asio::serial_port_base::baud_rate(115200));

    // Create a thread
    std::thread rx_th(receive_packet, std::ref(serial));

    // Sending packet
    std::cout << "Sending..." << std::endl;
    const auto& bytes_sent = boost::asio::write(serial, boost::asio::buffer(blink_led));
    std::cout << "Successfully sent " << bytes_sent << " bytes." << std::endl;

    // Make sub-thread join
    rx_th.join();

    return 0;
}