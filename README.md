# Mark And Sweep Garbage Collector (C - Linux Systems Programming)
Mark and Sweep Garbage Collector (C) - CS 361

Live Demo: https://replit.com/@fabiantorrestec/MarkAndSweepGarbageCollector

Example: <img width="555" alt="image" src="https://github.com/fabiantorrestech/MarkAndSweepGarbageCollector/assets/46858378/224ed203-4d2c-4647-a666-3a21e320de2b">

Criteria: https://www.cs.uic.edu/~ckanich/cs361/f19/homework4.html

This "Mark and Sweep" algorithm garbage collector will run a series of tests through this file to ensure that the garbage collection works correctly. The garbage collector operates by freeing memory from the heap.

It must run in a Linux environment for this work, as it utilizes Linux System calls.

------------
To Compile:

Run make in the terminal, or execute the following commands contained in the MakeFile:

    gcc -O0 -g memlib.c mm.c hw4.c main.c -o hw4 --std=gnu99

-----------
To Run:

Simply run this command:

    ./hw4
