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
    setup_uart(hal.serial(2), 115200);
    _kart_port_ready = true;
}

void Rover::send_kart_message(void)
{
    if (_kart_port_ready)
    {
        Vector3f attitude;
        // ahrs.get_secondary_attitude(attitude);
        attitude.x = ahrs.get_roll();
        attitude.y = ahrs.get_pitch();
        attitude.z = ahrs.get_yaw();
        if (attitude.z < 0)
        {
            attitude.z += M_2PI;
        }
        // Location loc;
        // ahrs.get_secondary_position(loc);
        uint8_t fix = static_cast<uint8_t>(rover.gps.status(1));
        _ubx.packVehicleUpdateMessage(attitude, current_loc, ground_speed, fix);
        _ubx.writeMessage(hal.serial(2));

        if (++_low_freq_count == kLowFreqCountMax)
        {
            _low_freq_count = 0;
            float h_acc_m = 0;
            float v_acc_m = 0;
            rover.gps.horizontal_accuracy(h_acc_m);
            rover.gps.vertical_accuracy(v_acc_m);
            _ubx.packGpsStatusMessage(h_acc_m, v_acc_m, rover.gps.num_sats());
            _ubx.writeMessage(hal.serial(2));
        }
        // hal.console->printf("yaw: %.1f\r\n", attitude.z * 180.f / M_PI);
    }
}