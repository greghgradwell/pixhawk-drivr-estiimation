#include "UbxInterpreter.h"
#include "definitions.h"

constexpr float kRad2DegE2 = 5729.57795f;

UbxInterpreter::UbxInterpreter()
{
    tx_buffer_[0] = START_BYTE_1;
    tx_buffer_[1] = START_BYTE_2;
}

void UbxInterpreter::packPixhawkMessage()
{
    setHeaderValues(0x11, 0x00, 15);
    Vector3f attitude;
    getEulerAngles(Vector3f & attitude);
    Location loc;
    getLLH(&loc);

    _ubx.packValue(static_cast<int16_t>(attitude.x * kRad2DegE2), 0);
    _ubx.packValue(static_cast<int16_t>(attitude.y * kRad2DegE2), 2);
    _ubx.packValue(static_cast<uint16_t>(attitude.z * kRad2DegE2), 4);
    _ubx.packValue(loc.lat, 6);
    _ubx.packValue(loc.lon, 10);
    _ubx.packValue(3, 14);
}

void UbxInterpreter::setHeaderValues(uint8_t msg_class, uint8_t msg_id, uint16_t payload_length)
{
    tx_buffer_[2] = msg_class;
    tx_buffer_[3] = msg_id;
    tx_buffer_[4] = payload_length & 0xFF;
    tx_buffer_[5] = (payload_length >> 8) & 0xFF;
    tx_payload_length_ = payload_length;
    tx_buffer_write_length_ = payload_length + PAYLOAD_LENGTH_ADDED_BYTES;
}

void UbxInterpreter::prepareMessage()
{
    uint8_t chka, chkb;
    calculateChecksum(&tx_buffer_[2], tx_payload_length_ + 4, chka, chkb);
    tx_buffer_[tx_payload_length_ + 6] = chka;
    tx_buffer_[tx_payload_length_ + 7] = chkb;
}

int UbxInterpreter::writeMessage(AP_HAL::UARTDriver *uart)
{
    return (uart->write(tx_buffer_, tx_buffer_write_length_));
}

int UbxInterpreter::messageBuffer(uint8_t msg_buffer[])
{
    memcpy(msg_buffer, tx_buffer_, tx_buffer_write_length_);
    return tx_buffer_write_length_;
}

void UbxInterpreter::printWriteBuffer()
{
    int i = 0;
    for (; i < tx_buffer_write_length_ - 1; i++)
    {
        hal.console->print(tx_buffer_[i], output_type);
        hal.console->print(",");
    }
    hal.console->print(tx_buffer_[i]);
}
void UbxInterpreter::calculateChecksum(uint8_t *payload, int payload_length, uint8_t &chka, uint8_t &chkb)
{
    chka = 0;
    chkb = 0;
    for (int i = 0; i < payload_length; ++i)
    {
        chka = (chka + payload[i]) & 0xFF;
        chkb = (chkb + chka) & 0xFF;
    }
}