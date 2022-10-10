TestBench 4 for Parser
* Try to find out all errors of the netlist.
R1 node_1 node_2 1K
R2 node_2 node_3
C1 node_2 node_3 10p
C1 node_3 GND 1p
L1 node_3 GND 1m

VIN gnd node_1 DC 5

.DC V1 0 10 1
.print V(node_1)

.end