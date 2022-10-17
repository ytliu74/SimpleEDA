TestBench2 for Homework4
* 2022 EDA course testbench netlist
* 2022-10-17

r2 2 1 2
v2 0 1 ac 10
l1 0 2 10
r1 1 3 1
c2 3 2 2


.ac dec 10 1 1000
*.plot ac vm(2) vm(3)
.END
