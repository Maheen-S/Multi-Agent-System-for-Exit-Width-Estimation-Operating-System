# Multi-Agent-System-for-Exit-Width-Estimation-Operating-System

Develop an operating system for 50 robots in a 100x100 room to estimate exit width, using inter-process communication (IPC) and synchronization. Prevention of Race Conditions by Mutexes to prevent data inconsistencies

Code Structure:
- **Main.cpp**: Initializes shared memory, spawns robot processes, manages width estimate aggregation.
- **Child.cpp**: Contains logic for individual robot processes.

Execution Flow:
1. **Environment Setup**: Room and exit details initialization.
2. **Robot Placement**: Random positioning of robots.
3. **Child Process Creation**: Child processes spawned for each robot.
4. **Estimate Calculation**: Individual estimates based on distance from exit.
5. **Data Sharing**: IPC via shared memory for estimates.
6. **Parallel Calculation**: Threads for efficient computation.
7. **Synchronization**: Mutexes ensure orderly execution.
8. **Result Display**: Output of true exit width and estimates.
