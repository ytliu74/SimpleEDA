TA-session-5-ckt-1
* TestBench 1 for Homework 5, 2022 EDA course.
* Author    : Hao Limin
* Date      : 2022/10/25
* Info      : RES lade.

R03 0 3 3k
R23 2 3 1k
R02 0 2 2k
R12 1 2 1k
R01 0 1 2k
Vin 3 0 DC 5

.DC Vin 5 15 0.5
.plot DC v(1)
.end
