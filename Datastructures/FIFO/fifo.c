#include "fifo.h"


fifo_status_t fifo_init(fifo_t *fifo, uint8_t *buffer, size_t size) {
    // TODO: Check that fifo is uninitialized (e.g. buffer == NULL) before initializing
    if (fifo == NULL || buffer == NULL) return FIFO_NULPTR;
    if (size == 0) return FIFO_BADARG;
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->head = 0;
    fifo->tail = 0;
    return FIFO_OK;
}

/* Slightly optimized by ommitting overflow check */
static inline void _fifo_push_byte(fifo_t *fifo, uint8_t data) {
    fifo->buffer[fifo->head] = data;                // Write data to head
    fifo->head = (fifo->head + 1) % fifo->size;     // Move head forward, wrap around if necessary
}

static inline void _fifo_push_byte_force(fifo_t *fifo, uint8_t data) {
    _fifo_push_byte(fifo, data);
    if (fifo->head == fifo->tail)                   // Buffer is full
        fifo->tail = (fifo->tail + 1) % fifo->size; // Move tail forward; Drop oldest data
}

fifo_status_t fifo_push8(fifo_t *fifo, uint8_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    size_t next_head = (fifo->head + 1) % fifo->size; // Calculate next head position
    if (next_head == fifo->tail) return FIFO_FULL;    // Buffer is full        
    _fifo_push_byte(fifo, data);
    return FIFO_OK;
}

fifo_status_t fifo_push16(fifo_t *fifo, uint16_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    uint8_t* _data = (uint8_t*)&data;
    if (fifo->size - fifo_count(fifo) < 2) return FIFO_FULL; // Not enough space to push 2 bytes
    _fifo_push_byte(fifo, _data[1]); // Push high byte
    _fifo_push_byte(fifo, _data[0]); // Push low byte
    return FIFO_OK;
}

fifo_status_t fifo_push32(fifo_t *fifo, uint32_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    uint8_t* _data = (uint8_t*)&data;
    if (fifo->size - fifo_count(fifo) < 4) return FIFO_FULL; // Not enough space to push 4 bytes
    fifo_push8(fifo, _data[3]); // Push highest byte
    fifo_push8(fifo, _data[2]);
    fifo_push8(fifo, _data[1]);
    fifo_push8(fifo, _data[0]); // Push lowest byte
    return FIFO_OK;
}

/**
 * Doesn't use _fifo_push_byte_force() which allows for returning
 * the overflow status within the if statment rather than adding
 * addition cycles/ram to calculate it another time before calling
 * _fifo_push_byte_force()
 **/
fifo_status_t fifo_push8_force(fifo_t *fifo, uint8_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    fifo->buffer[fifo->head] = data;                // Write data to head
    fifo->head = (fifo->head + 1) % fifo->size;     // Move head forward, wrap around if necessary
    if (fifo->head == fifo->tail) {                 // Buffer is full
        fifo->tail = (fifo->tail + 1) % fifo->size; // Move tail forward; Drop oldest data
        return FIFO_OVRFLW;
    }
    return FIFO_OK;
}

fifo_status_t fifo_push16_force(fifo_t *fifo, uint16_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    fifo_status_t status = (fifo->size - fifo_count(fifo)) >= 2 ? FIFO_OK : FIFO_OVRFLW;
    _fifo_push_byte_force(fifo, (data >> 0) & 0xFF); // Little endian: Push lowest byte first
    _fifo_push_byte_force(fifo, (data >> 8) & 0xFF);
    return status;
}

fifo_status_t fifo_push32_force(fifo_t *fifo, uint32_t data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    fifo_status_t status = (fifo->size - fifo_count(fifo)) >= 4 ? FIFO_OK : FIFO_OVRFLW;
    _fifo_push_byte_force(fifo, (data >> 0 ) & 0xFF); // Little endian: Push lowest byte first
    _fifo_push_byte_force(fifo, (data >> 8 ) & 0xFF);
    _fifo_push_byte_force(fifo, (data >> 16) & 0xFF);
    _fifo_push_byte_force(fifo, (data >> 24) & 0xFF);
    return status;
}


static inline void _fifo_pop_byte(fifo_t *fifo, uint8_t *data) {
    *data = fifo->buffer[fifo->tail];
    fifo->tail = (fifo->tail + 1) % fifo->size;
}


fifo_status_t fifo_pop8(fifo_t *fifo, uint8_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    _fifo_pop_byte(fifo, data);
    return FIFO_OK;
}

fifo_status_t fifo_pop16(fifo_t *fifo, uint16_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    // Make sure we have at least 2 bytes available
    if (fifo_count(fifo) < 2) return FIFO_UNDRFLW;
    uint8_t* _data = (uint8_t *)data;
    _fifo_pop_byte(fifo, &(_data[0])); // Little endian: Pop lowest byte first
    _fifo_pop_byte(fifo, &(_data[1]));
    return FIFO_OK;
}

fifo_status_t fifo_pop32(fifo_t *fifo, uint32_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    // Make sure we have at least 4 bytes available
    if (fifo_count(fifo) < 4) return FIFO_UNDRFLW;
    uint8_t* _data = (uint8_t *)data;
    _fifo_pop_byte(fifo, &(_data[0])); // Little endian: Pop lowest byte first
    _fifo_pop_byte(fifo, &(_data[1]));
    _fifo_pop_byte(fifo, &(_data[2]));
    _fifo_pop_byte(fifo, &(_data[3]));
    return FIFO_OK;
}


fifo_status_t fifo_pop8(fifo_t *fifo) {
    if (fifo == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    fifo->tail = (fifo->tail + 1) % fifo->size;
    return FIFO_OK;
}

fifo_status_t fifo_pop16(fifo_t *fifo) {
    if (fifo == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    // Make sure we have at least 2 bytes available
    if (fifo_count(fifo) < 2) return FIFO_UNDRFLW;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    return FIFO_OK;
}

fifo_status_t fifo_pop32(fifo_t *fifo) {
    if (fifo == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    // Make sure we have at least 4 bytes available
    if (fifo_count(fifo) < 4) return FIFO_UNDRFLW;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    return FIFO_OK;
}


// static inline void _fifo_peak_increment_byte(fifo_t *fifo, uint8_t *data) {
//     *data = fifo->buffer[fifo->tail];
// }

// static inline void _fifo_peak_byte_bulk(fifo_t *fifo, uint8_t *data, size_t len) {
//     if (fifo->size - fifo->tail >= len) {
//         // Data is contiguous from tail to tail+len, no wrap around
//         memcpy(data, fifo->buffer + fifo->tail, len);
//     } else {
//         // Data wraps around the end of the buffer
//         size_t first_part = fifo->size - fifo->tail;
//         memcpy(data, fifo->buffer + fifo->tail, first_part);
//         memcpy(data + first_part, fifo->buffer, len - first_part);
//     }
// }


fifo_status_t fifo_peak8(fifo_t *fifo, uint8_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    *data = fifo->buffer[fifo->tail];
    return FIFO_OK;
}

fifo_status_t fifo_peak16(fifo_t *fifo, uint16_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    if (fifo_count(fifo) < 2) return FIFO_UNDRFLW;   // Not enough data to peak
    ((uint8_t*)data)[0] = fifo->buffer[fifo->tail];  // Little endian: Peak lowest byte first
    ((uint8_t*)data)[1] = fifo->buffer[(fifo->tail + 1) % fifo->size];
    return FIFO_OK;
}

fifo_status_t fifo_peak32(fifo_t *fifo, uint32_t *data) {
    if (fifo == NULL || data == NULL) return FIFO_NULPTR;
    if (fifo->head == fifo->tail) return FIFO_EMPTY; // Buffer is empty
    if (fifo_count(fifo) < 4) return FIFO_UNDRFLW;   // Not enough data to peak
    ((uint8_t*)data)[0] = fifo->buffer[fifo->tail];  // Little endian: Peak lowest byte first
    ((uint8_t*)data)[1] = fifo->buffer[(fifo->tail + 1) % fifo->size];
    ((uint8_t*)data)[2] = fifo->buffer[(fifo->tail + 2) % fifo->size];
    ((uint8_t*)data)[3] = fifo->buffer[(fifo->tail + 3) % fifo->size];
    return FIFO_OK;
}

/**
 * Returns the total capacity of the FIFO buffer in bytes
 * Returns 0 if fifo pointer is NULL
 * Undefined behavior if fifo is not initialized
 */
inline size_t fifo_size_inline(fifo_t *fifo) {
    if (fifo == NULL) return 0;
    return fifo->size;
}

/**
 * Returns the total capacity of the FIFO buffer in bytes
 * Returns 0 if fifo pointer is NULL
 * Undefined behavior if fifo is not initialized
 */
size_t fifo_size(fifo_t *fifo) {
    if (fifo == NULL) return 0;
    return fifo->size;
}

size_t fifo_count(fifo_t *fifo) {
    if (fifo == NULL || fifo->buffer == NULL) return 0;
    if (fifo->head >= fifo->tail) {
        return fifo->head - fifo->tail;
    } else {
        return fifo->size - fifo->tail + fifo->head;
    }
}


fifo_status_t fifo_clear(fifo_t *fifo) {
    if (fifo == NULL) return FIFO_NULPTR;
    if (fifo->buffer == NULL) return FIFO_UNINIT;
    for (size_t i = 0; i < fifo->size; i++)
        fifo->buffer[i] = 0;
    fifo->head = 0;
    fifo->tail = 0;
    return FIFO_OK;
}
