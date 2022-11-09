My Testbench

V1 1 0 TRAN sin (0 1 1 0 0)
R1 1 2 1
R2 2 0 1


.tran 0.01 4 0
.plot tran v(1) v(2)
.end