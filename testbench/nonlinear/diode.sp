A test for diode

R1 1 2 1
D1 2 3 diode
R2 3 0 1
V1 1 0 5

.dc v1 0 0.5 0.001
.plot dc v(2)
.end