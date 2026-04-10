#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>


typedef struct {
    uint8_t* buffer;
    size_t size;
    size_t head;
    size_t tail;
} fifo_t;

typedef enum {
    FIFO_OK = 0, // Operation successful
    FIFO_NULPTR, // Null pointer passed as argument
    FIFO_UNINIT, // FIFO not initialized
    FIFO_BADARG, // Invalid argument (e.g. size == 0 or not divisible by data size)
    FIFO_EMPTY,  // FIFO empty; Pop operation aborted
    FIFO_FULL,   // FIFO full; Push operation aborted without modifying FIFO
    FIFO_OVRFLW, // FIFO full; Oldest data overwritten
    FIFO_UNDRFLW // FIFO length less than requested pop count, but not empty; Pop operation aborted without modifying FIFO
} fifo_status_t;


/** Initializes the FIFO with the given user buffer and size */
fifo_status_t fifo_init(fifo_t *fifo, uint8_t *buffer, size_t size);


/**
 * Pushes a 8-bit element onto the FIFO
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_FULL if the FIFO is full (i.e. no free space)
 * In this case, the FIFO will not be modified (i.e. no bytes will be pushed)
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push8(fifo_t *fifo, uint8_t data);
/**
 * Pushes a 16-bit element onto the FIFO using little endian format (i.e. lower byte is pushed first)
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_FULL if there is not enough space to push the entire 16-bit data (i.e. less than 2 bytes of free space)
 * In this case, the FIFO will not be modified (i.e. no bytes will be pushed)
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push16(fifo_t *fifo, uint16_t data);
/**
 * Pushes a 32-bit element onto the FIFO using little endian format (i.e. lower byte is pushed first)
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_FULL if there is not enough space to push the entire 32-bit data (i.e. less than 4 bytes of free space)
 * In this case, the FIFO will not be modified (i.e. no bytes will be pushed)
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push32(fifo_t *fifo, uint32_t data);


/**
 * Pushes an 8-bit element onto the FIFO, overwriting the oldest element if necessary
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_OVRFLW if the FIFO is full and the oldest element was overwritten
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push8_force(fifo_t *fifo, uint8_t data);
/**
 * Pushes a 16-bit element onto the FIFO, overwriting the oldest element(s) if necessary
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_OVRFLW if the FIFO does not have enough free space to push the entire 16-bit data (i.e. less than 2 bytes of free space), and one or more elements were overwritten
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push16_force(fifo_t *fifo, uint16_t data);
/**
 * Pushes a 32-bit element onto the FIFO, overwriting the oldest element(s) if necessary
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_OVRFLW if the FIFO does not have enough free space to push the entire 32-bit data (i.e. less than 4 bytes of free space), and one or more elements were overwritten
 * Returns FIFO_OK if the data was successfully pushed onto the FIFO
 */
fifo_status_t fifo_push32_force(fifo_t *fifo, uint32_t data);


/**
 * Peaks at the next 8-bit element in the FIFO without modifying the FIFO
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_OK if the data was successfully peeked at
 */
fifo_status_t fifo_peak8(fifo_t *fifo, uint8_t *data);
/**
 * Peaks at the next 16-bit element in the FIFO without modifying the FIFO
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to peak a full 16-bit element (i.e. less than 2 bytes of data)
 * Returns FIFO_OK if the data was successfully peeked at
 */
fifo_status_t fifo_peak16(fifo_t *fifo, uint16_t *data);
/**
 * Peaks at the next 32-bit element in the FIFO without modifying the FIFO
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to peak a full 32-bit element (i.e. less than 4 bytes of data)
 * Returns FIFO_OK if the data was successfully peeked at
 */
fifo_status_t fifo_peak32(fifo_t *fifo, uint32_t *data);


/**
 * Pops the next 8-bit element from the FIFO into the provided data pointer
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop8(fifo_t *fifo, uint8_t *data);
/**
 * Pops the next 16-bit element from the FIFO into the provided data pointer, using little endian format (i.e. lower byte is popped first)
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to pop a full 16-bit element (i.e. less than 2 bytes of data)
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop16(fifo_t *fifo, uint16_t *data);
/**
 * Pops the next 32-bit element from the FIFO into the provided data pointer, using little endian format (i.e. lower byte is popped first)
 * Returns FIFO_NULPTR if fifo pointer or data pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to pop a full 32-bit element (i.e. less than 4 bytes of data)
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop32(fifo_t *fifo, uint32_t *data);


/**
 * Pops the next 8-bit element from the FIFO and discards it
 * Returns FIFO_NULPTR if fifo pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop8(fifo_t *fifo);
/**
 * Pops the next 16-bit element from the FIFO and discards it
 * Returns FIFO_NULPTR if fifo pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to pop a full 16-bit element (i.e. less than 2 bytes of data)
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop16(fifo_t *fifo);
/**
 * Pops the next 32-bit element from the FIFO and discards it
 * Returns FIFO_NULPTR if fifo pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_EMPTY if the FIFO is empty
 * Returns FIFO_UNDRFLW if the FIFO does not have enough data to pop a full 32-bit element (i.e. less than 4 bytes of data)
 * Returns FIFO_OK if the data was successfully popped
 */
fifo_status_t fifo_pop32(fifo_t *fifo);


// /* Pops <count> elements of type uint8_t from the FIFO into the provided buffer */
// fifo_status_t fifo_pop8_bulk(fifo_t *fifo, uint8_t *data, size_t count);
// /* Pops <count> elements of type uint16_t from the FIFO into the provided buffer */
// fifo_status_t fifo_pop16_bulk(fifo_t *fifo, uint16_t *data, size_t count);
// /* Pops <count> elements of type uint32_t from the FIFO into the provided buffer */
// fifo_status_t fifo_pop32_bulk(fifo_t *fifo, uint32_t *data, size_t count);


/**
 * Returns the size of the FIFO buffer in bytes
 * Returns 0 if fifo pointer is NULL
 * Undefined behavior if fifo is not initialized
 */
size_t fifo_size(fifo_t *fifo);
/**
 * Returns the size of the FIFO buffer in bytes
 * Returns 0 if fifo pointer is NULL
 * Undefined behavior if fifo is not initialized
 */
inline size_t fifo_size_inline(fifo_t *fifo);


/**
 * Returns the number of elements in the FIFO (in bytes)
 * Returns 0 if fifo pointer is NULL or fifo buffer pointer is NULL
 * Undefined behavior if fifo is not initialized
 */
size_t fifo_count(fifo_t *fifo);


/**
 * Clears the FIFO by resetting head and tail indices and zeroing out the buffer
 * Returns FIFO_NULPTR if fifo pointer is NULL
 * Returns FIFO_UNINIT if fifo buffer pointer is NULL
 * Returns FIFO_OK if the FIFO was successfully cleared
 */
fifo_status_t fifo_clear(fifo_t *fifo);


#endif // FIFO_H