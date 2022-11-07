A test for diode

R1 1 2 10
D1 2 0 diode
V1 1 0 5

.dc v1 0 2 0.005
.plot dc v(2)
.end