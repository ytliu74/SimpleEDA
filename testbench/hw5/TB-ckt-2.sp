TA-session-5-ckt-2
* TestBench 2 for Homework 5, 2022 EDA course.
* Author    : Hao Limin
* Date      : 2022/10/25.
* Info      : RLC filter.

R1 1 0 1k
C1 2 1 100u
R2 3 2 1
L1 4 3 100u
V1 4 0 AC 1

.AC DEC 10 1 100MEG
.plot AC vm(1)
.end
