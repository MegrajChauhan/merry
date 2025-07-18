# Overview

Merry implements a few different data structures that it uses for its internal functioning.
These are your typical data structures and nothing fancy.

- Stack: Uses Fix-sized buffers, stores a minimum and maximum of 8-bytes.
- List: Store n-bytes of data at once as specified at initialization time, has static buffer but can be resized.
- Dynamic List: Exactly the same as List but performs resizing automatically when needed.
- Static Queue: Uses linked list, can store only 8-bytes min and max so basically pointers only, cannot be resized.
- Dynamic Queue: Exactly as static queue but resizes according to the needs of the queue.
- Simple Queue: Implements an array based circular queue, can specify the size of each element to store, cannot resize. 
