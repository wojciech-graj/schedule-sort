# Schedule-Sort

Sorting algorithms are infamous for being difficult to understand and implement, which is why Schedule-Sort™\* delegates all of this difficult work to Linux's FIFO process scheduler, yielding an O(n) integer sorting algorithm \*\* \*\*\*. The algorithm works by creating a new thread for each integer in the list, and assigning said thread a priority equal to its value. Once all the threads have been created, they are run in order of decreasing priority, with each thread appending its value to the sorted list.

![Figure 1](post.png)

*Figure 1: The post that inspired this project*

- \* Trademark pending.
- \*\* Assuming the FIFO scheduler is O(n), which I am too lazy to check.
- \*\*\* Only integers in range [1,99] can be sorted.

## Running

This program can only be compiled on Linux, and must be run as root after enabling realtime scheduling: `sysctl -w kernel.sched_rt_runtime_us=-1`.

If the program does not work, you likely did not enable realtime scheduling, but because I do enough error handling at work to be bothered to add it to a gag program, you might have to experiment for yourself.

The compilation and invocation can be performed as follows:
```
$ cc sort.c -o sort -lpthread
# ./sort 1 2 3
```

## License
```
Copyright (c) 2024 Wojciech Graj

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```
