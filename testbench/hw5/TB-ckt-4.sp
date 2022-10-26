TA-session-5-ckt-4
* TestBench 4 for Homework 5, 2022 EDA course. 
* Author    : Hao Limin
* Date      : 2022/10/25
* Info      : LC oscillation.

C1 1 0 1
L1 1 0 1
R1 1 2 1
V1 2 0 PULSE (0 1 1 1 1 1 15)

.tran 0.1 30
.plot TRAN v(1)
.end
