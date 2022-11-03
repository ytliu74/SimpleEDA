TA-session-6-ckt-1
* TestBench 1 for Homework 6, 2022 EDA course. 
* Author    : Hao Limin.
* Date      : 2022/11/03.

C1 2 0 1
R1 1 0 1000 
D1 1 2 diode
Is 0 1 1 const(1)

.tran 0.01 1 0
.plot tran v(2)
.END