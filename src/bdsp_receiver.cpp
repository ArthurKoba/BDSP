#include "bdsp_receiver.h"

BDSPReceiver::BDSPReceiver() {
    decoder = nullptr;
    raw_packet = nullptr;
}

BDSPReceiver::~BDSPReceiver() {
    delete raw_packet;
}

bdsp_set_config_status BDSPReceiver::set_config(cobs_config_t config, packet_handler_t handler) {
    if (decoder) return CONFIG_ALREADY_INSTALLED;
    packet_handler = handler;

    cobs_reader_data_callback_t callback = [] (uint8_t character, cobs_read_state read_state, void *context) {
        BDSPReceiver &self = *reinterpret_cast<BDSPReceiver*>(context);
        self.parse_packet_byte(character, read_state);
    };

    decoder = new COBSDecoder(config, callback, this);
    return CONFIG_INSTALLED;
}

void BDSPReceiver::parse(uint8_t *data_ptr, size_t size) {
    decoder->parse(data_ptr, size);
}

void BDSPReceiver::parse_packet_byte(uint8_t character, cobs_read_state read_state) {
    if (read_state == ERROR) {
        return reset();
    }

    if (parse_state not_eq PACKET_CHECKSUM and parse_state not_eq PACKET_ID) {
        packet_checksum = crc8(&character, 1, packet_checksum);
    }

    switch (parse_state) {
        case PACKET_ID:
            raw_packet = new Packet(character, 0);
            packet_checksum = crc8(&character, 1);
            parse_state = SIZE_A;
            break;
        case SIZE_A:
            raw_packet->size = character;
            parse_state = SIZE_B;
            break;
        case SIZE_B:
            raw_packet->size += character << 8;
            if (raw_packet->size > max_packet_size or raw_packet->size < 2) {
                // Packet size error.
                return reset();
            }
            raw_packet->create_buffer();
            if (not raw_packet->data) {
                // Error creating packet buffer.
                return reset();
            }
            parse_state = PACKET_DATA;
            break;
        case PACKET_DATA:
            raw_packet->data[received_data_byte] = character;
            received_data_byte += 1;
            if (received_data_byte == raw_packet->size) {
                parse_state = PACKET_CHECKSUM;
            }
            break;
        case PACKET_CHECKSUM:
            if (packet_checksum not_eq character) return reset();
            parse_state = WAIT_END;
            break;
        case WAIT_END:
            if (read_state == END) {
                packet_handler(*raw_packet);
            }
            reset();
            break;
    }
}

void BDSPReceiver::reset() {
    decoder->reset(true);
    parse_state = PACKET_ID;
    received_data_byte = 0;
    if (raw_packet) {
        delete raw_packet;
        raw_packet = nullptr;
    }
}


