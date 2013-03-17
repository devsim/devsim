
dna_t = 0.02e-4;
dna_r = 0.001e-4;
dna_lc = dna_r / 20;
Point(0) = {      0, -dna_t/2, 0, dna_lc};
Point(1) = {      0, +dna_t/2, 0, dna_lc};
Point(2) = {  dna_r, -dna_t/2, 0, dna_lc};
Point(3) = {  dna_r, +dna_t/2, 0, dna_lc};

disk_t  = 0.5e-4;
disk_r  = 0.3e-4;
disk_w  = 2.2e-4;
disk_r2 = disk_r + disk_w;
disk_lc = disk_t/25;
disk_lc2 = disk_w/25;

Point(4) = {      0, -disk_t/2,  0, disk_lc};
Point(5) = {      0, +disk_t/2,  0, disk_lc};
Point(6) = { disk_r, -disk_t/2,  0, disk_lc};
Point(7) = { disk_r, +disk_t/2,  0, disk_lc};
Point(8) = { disk_r2, -disk_t/2, 0, disk_lc2};
Point(9) = { disk_r2, +disk_t/2, 0, disk_lc2};


who_t = 2.5e-4;
who_r = disk_r2;
who_lc = who_t/25;

Point(10) = {      0, -who_t/2, 0, who_lc};
Point(11) = {      0, +who_t/2, 0, who_lc};
Point(12) = {  who_r, -who_t/2, 0, who_lc};
Point(13) = {  who_r, +who_t/2, 0, who_lc};
Line(1) = {0, 1};
Line(2) = {1, 3};
Line(3) = {3, 2};
Line(4) = {0, 2};
Line(6) = {7, 6};
Line(8) = {1, 5};
Line(9) = {4, 0};
Line(10) = {4, 10};
Line(11) = {10, 12};
Line(12) = {12, 8};
Line(13) = {8, 9};
Line(14) = {9, 13};
Line(15) = {13, 11};
Line(16) = {11, 5};
Line(17) = {7, 9};
Line(18) = {6, 8};
Line Loop(19) = {1, 2, 3, -4};
Plane Surface(20) = {19};
Line Loop(21) = {8, -16, -15, -14, -17, 6, 18, -12, -11, -10, 9, 4, -3, -2};
Plane Surface(22) = {21};
Line Loop(23) = {17, -13, -18, -6};
Plane Surface(24) = {23};

Physical Line("top") = {15};
Physical Line("bot") = {11};
Physical Line("dielectric_solution") = {17, 6, 18};
Physical Line("dna_solution") = {2, 3, 4};
Physical Surface("solution") = {22};
Physical Surface("dielectric") = {24};
Physical Surface("dna") = {20};
