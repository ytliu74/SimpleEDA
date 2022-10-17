TestBench0 for Homework4 - Stamp Example
* 2022 EDA course testbench netlist

R1 1 2 200
R2 2 0 100

.op
.end

*       MNA Matrix
*   [ 0.01   0.    -0.01 ]
*   [ 0.     0.005 -0.005]
*   [-0.01  -0.005  0.015]
*
*       RHS Vector
*           [0.]
*           [0.]
*           [0.]
