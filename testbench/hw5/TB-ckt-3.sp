TA-session-5-ckt-3
* TestBench 3 for Homework 5, 2022 EDA course.
* Author    : Hao Limin
* Date      : 2022/10/25
* Info      : RC charge.

R1 1 2 1k
C1 1 0 100u
V1 2 0 PULSE (0 1 1 1 1 1 10)

.tran 0.01 3
.plot TRAN v(1)
.end
