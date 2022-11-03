TA-session-6-ckt-3
* TestBench 3 for Homework 6, 2022 EDA course. 
* Author    : Hao Limin.
* Date      : 2022/11/03.

.model diode diode1

V1 1 0 TRAN sin (0 1 1 0 0)
R1 1 2 1n
D1 2 0 diode

.tran 0.01 2 0
.plot tran v(1) v(2)
.end