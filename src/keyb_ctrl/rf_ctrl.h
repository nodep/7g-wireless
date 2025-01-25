#pragma once

// stat counters
extern uint32_t plos_total, arc_total, rf_packets_total;

void rf_ctrl_init(void);

// LED status will be set to LED_STATUS_NOT_RECEIVED if no status has been received
#define LED_STATUS_NOT_RECEIVED	0xff

bool rf_ctrl_send_message(const void* buff, const uint8_t num_bytes);

uint8_t rf_ctrl_read_ack_payload(void* buff, const uint8_t buff_size);

bool rf_ctrl_process_ack_payloads(uint8_t* msg_buff_free, uint8_t* msg_buff_capacity);

void rf_set_addr(const uint8_t* addr);