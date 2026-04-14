# Notes for myself

## Ensure that FIFO is interupt safe

there are a couple of details you should be aware of:  

1. I only modify the head index in the add function, and I only modify the tail index in the remove function.
2. I only modify the index after reading/writing the data in the buffer.

By doing both of the above, I have successfully ensured that this is an interrupt-safe, lock-free ring buffer (if there is a single consumer and a single producer)