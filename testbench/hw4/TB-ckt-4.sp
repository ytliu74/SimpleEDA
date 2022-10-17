TestBench4 for Homework4
* 2022 EDA course testbench netlist
* 2022-10-17
* Origin: From Qixu Xie's Small Signal Model - AC Test

Vin in gnd 0 AC 1
Cin in gnd 1p

*first stage
Gm1 out1 gnd in gnd 160.6u
R1 out1 gnd 0.75Meg
C1 out1 gnd 619f

*compensation capacitor
Cc out1 out2 1p

*second stage
Gm2 out2 gnd out1 gnd 995u
R2 out2 gnd 99.47K
C2 out2 gnd 5p

.AC DEC 10 1 1000Meg
*.plot ac vdec(out2) vp(out2)
.end
