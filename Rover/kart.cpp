#include "Rover.h"
#include "UbxInterpreter.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Vehicle/AP_Vehicle.h> // needed for AHRS build
#include <AP_GPS/AP_GPS.h>

static void setup_uart(AP_HAL::UARTDriver *uart, const uint32_t baud)
{
    if (uart == nullptr)
    {
        // that UART doesn't exist on this platform
        hal.console->printf("Serial not available");

        return;
    }
    hal.console->printf("setup uart\n");
    uart->begin(baud);
    // uart->set_flow_control(AP_HAL::UARTDriver::flow_control::FLOW_CONTROL_ENABLE);
}

void Rover::setup_telem()
{
    // hal.scheduler->delay(1000);
    setup_uart(hal.serial(2), 230400);
}

void Rover::send_kart_message(void)
{
    Vector3f attitude;
    ahrs.get_secondary_attitude(attitude);
    Location loc;
    ahrs.get_secondary_position(loc);
    _ubx.packPixhawkMessage(attitude, loc, static_cast<uint8_t>(rover.gps.status(1)));
    _ubx.writeMessage(hal.serial(2));
    hal.console->printf("send kart \n");
}