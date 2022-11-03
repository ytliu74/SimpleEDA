TA-session-6-ckt-2
* TestBench 2 for Homework 6, 2022 EDA course. 
* Author    : Hao Limin.
* Date      : 2022/11/03.

C1 2 0 1
D1 1 0 diode
D2 2 0 diode
R1 1 0 1000 
R2 1 2 1m
Is 0 1 1 0 tran const(1)

.TRAN 0.01 1 0
.plot tran v(1) v(2)
.END