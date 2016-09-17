#pragma once

void rf_dngl_init(void);
uint8_t rf_dngl_recv(__xdata void* buff, uint8_t buff_size);

void rf_dngl_queue_ack_payload(__xdata void* buff, uint8_t num_bytes);