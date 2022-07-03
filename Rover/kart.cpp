#include "Rover.h"
#include <AP_HAL/AP_HAL.h>

// const AP_HAL::HAL &hal = AP_HAL::get_HAL();
void set_g(uint8_t buffer[], int start_value);

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
    // uart->set_unbuffered_writes(true);
    // uart->set_flow_control(AP_HAL::UARTDriver::flow_control::FLOW_CONTROL_ENABLE);
}

void Rover::setup_telem()
{

    hal.scheduler->delay(1000);
    // hal.console->printf("Setup telem\n");

    setup_uart(hal.serial(2), 230400);
}

void set_g(uint8_t buffer[], int start_value)
{
    for (int i = 0; i < 10; ++i)
    {
        buffer[i] = start_value;
    }
}

uint8_t g_count[10];
int g_i = 0;

void Rover::print_test(void)
{
    // if (hal.serial(5)->is_initialized())
    // {
    // hal.console->printf("pt\n");
    set_g(g_count, g_i);
    ++g_i;
    // hal.console->printf("gi\n");
    hal.serial(2)->write(&g_count[0], 10);
    // if (g_i == 200)
    // {
    // hal.console->printf("%d\n", g_count[0]);
    // hal.serial(5)->flush();
    // g_i = 0;
    // }
    // }
}