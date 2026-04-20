# Notes for myself

## Performance

[https://www.downtowndougbrown.com/2013/01/microcontrollers-interrupt-safe-ring-buffers/]

modulo is pretty slow because it’s a divide operation. Division is a pretty slow operation on computers. It turns out when you have a power of two, you can do the equivalent of a modulo by doing a bitwise AND, which is a much quicker operation. It works because if you take a power of two and subtract one, you get a number which can be represented in binary as a string of all 1 bits. In the case of a queue of size 64, bitwise ANDing the head or tail with 63 will always keep the index between 0 and 63. So you can do the wrap-around like so:

```
ring_tail = (ring_tail + 1) & 63;
```

A good compiler will automatically convert the modulo to the faster AND operation if it’s a power of two, so you can just use my first example with the “% 64” since it makes the intent of the code clearer. I’ve been told I have “a lot of faith in compilers” but it’s true–if you compile with optimizations enabled, GCC will correctly optimize my first example into the assembly equivalent of my second example. Looking at the assembly code that your compiler generates is a very valuable tool for you to have available.



## Ensure that FIFO is interrupt safe

there are a couple of details you should be aware of:  

1. I only modify the head index in the add function, and I only modify the tail index in the remove function.
2. I only modify the index after reading/writing the data in the buffer.

By doing both of the above, I have successfully ensured that this is an interrupt-safe, lock-free ring buffer (if there is a single consumer and a single producer)