device_width =     1.0e-4;
gate_width =       1.0e-5;
diffusion_width =  0.4;

air_thickness =        1e-7;
oxide_thickness =      1e-5;
gate_thickness =       1e-5;
device_thickness =     1e-4;
diffusion_thickness =  1e-5;

y_channel_spacing =      1e-7;
y_diffusion_spacing =    1e-6;
y_gate_top_spacing =     1e-8;
y_gate_mid_spacing =     gate_thickness * 0.25;
y_gate_bot_spacing =     1e-8;
y_oxide_mid_spacing =    oxide_thickness * 0.25;
x_channel_spacing =    1e-6;
x_diffusion_spacing =  1e-5;
max_y_spacing =        1e-4;
max_x_spacing =        1e-2;
y_bulk_mid_spacing =  device_thickness * 0.25;
y_bulk_bot_spacing =  1e-8;

x_bulk_left =    0.0;
x_bulk_right =   x_bulk_left + device_width;
x_center =       0.5 * (x_bulk_left + x_bulk_right);
x_gate_left =    x_center - 0.5 * (gate_width);
x_gate_right =   x_center + 0.5 * (gate_width);
x_device_left =  x_bulk_left - air_thickness;
x_device_right = x_bulk_right + air_thickness;

y_bulk_top =       0.0;
y_oxide_top =      y_bulk_top - oxide_thickness;
y_oxide_mid =      0.5 * (y_oxide_top + y_bulk_top);
y_gate_top =       y_oxide_top - gate_thickness;
y_gate_mid =       0.5 * (y_gate_top + y_oxide_top);
y_device_top =     y_gate_top - air_thickness;
y_bulk_bot =    y_bulk_top + device_thickness;
y_bulk_mid =       0.5 * (y_bulk_top + y_bulk_bot);
y_device_bot =  y_bulk_bot + air_thickness;
y_diffusion =      y_bulk_top + diffusion_thickness;

z1 = -1e-5;
z2 = -0.5e-5;
z3 = 0.5e-5;
z4 = 1e-5;

//// Bulk
Point( 1) = {x_bulk_left,  y_bulk_top, z1, x_diffusion_spacing};
Point( 3) = {x_gate_left,  y_bulk_top, z1, x_diffusion_spacing};
Point( 4) = {x_gate_right, y_bulk_top, z1, x_diffusion_spacing};
Point( 6) = {x_bulk_right, y_bulk_top, z1, x_diffusion_spacing};
Point( 7) = {x_bulk_right, y_bulk_bot, z1, x_diffusion_spacing};
Point( 8) = {x_bulk_left,  y_bulk_bot, z1, x_diffusion_spacing};

Point( 9) = {x_gate_left,  y_gate_top,  z2, x_diffusion_spacing};
Point(10) = {x_gate_right, y_gate_top,  z2, x_diffusion_spacing};
Point(11) = {x_gate_right, y_oxide_top, z2, x_diffusion_spacing};
Point(12) = {x_gate_left,  y_oxide_top, z2, x_diffusion_spacing};

Point(15) = {x_gate_right, y_bulk_top, z2, x_diffusion_spacing};
Point(16) = {x_gate_left,  y_bulk_top, z2, x_diffusion_spacing};

Point(101) = {x_bulk_left,  y_bulk_top, z4, x_diffusion_spacing};
Point(103) = {x_gate_left,  y_bulk_top, z4, x_diffusion_spacing};
Point(104) = {x_gate_right, y_bulk_top, z4, x_diffusion_spacing};
Point(106) = {x_bulk_right, y_bulk_top, z4, x_diffusion_spacing};
Point(107) = {x_bulk_right, y_bulk_bot, z4, x_diffusion_spacing};
Point(108) = {x_bulk_left,  y_bulk_bot, z4, x_diffusion_spacing};

Point(109) = {x_gate_left,  y_gate_top,  z3, x_diffusion_spacing};
Point(110) = {x_gate_right, y_gate_top,  z3, x_diffusion_spacing};
Point(111) = {x_gate_right, y_oxide_top, z3, x_diffusion_spacing};
Point(112) = {x_gate_left,  y_oxide_top, z3, x_diffusion_spacing};

Point(115) = {x_gate_right, y_bulk_top, z3, x_diffusion_spacing};
Point(116) = {x_gate_left,  y_bulk_top, z3, x_diffusion_spacing};


delta = 1e-6;
source_w   = 5e-6;
x_source1 = x_bulk_left + delta;
x_source2 = x_gate_left - delta;
z_source1 = z1 + delta;
z_source2 = z4 - delta;
Point(120) = { x_source1, y_bulk_top, z_source1, x_diffusion_spacing};
Point(121) = { x_source1, y_bulk_top, z_source2, x_diffusion_spacing};
Point(122) = { x_source2, y_bulk_top, z_source1, x_diffusion_spacing};
Point(123) = { x_source2, y_bulk_top, z_source2, x_diffusion_spacing};
Point(124) = { x_source1, y_bulk_top, z1, x_diffusion_spacing};
Point(125) = { x_source1, y_bulk_top, z4, x_diffusion_spacing};
Point(126) = { x_source2, y_bulk_top, z1, x_diffusion_spacing};
Point(127) = { x_source2, y_bulk_top, z4, x_diffusion_spacing};

x_drain1 = x_bulk_right - delta;
x_drain2 = x_gate_right + delta;
z_drain1 = z1 + delta;
z_drain2 = z4 - delta;
Point(130) = { x_drain1, y_bulk_top, z_drain1, x_diffusion_spacing};
Point(131) = { x_drain1, y_bulk_top, z_drain2, x_diffusion_spacing};
Point(132) = { x_drain2, y_bulk_top, z_drain1, x_diffusion_spacing};
Point(133) = { x_drain2, y_bulk_top, z_drain2, x_diffusion_spacing};
Point(134) = { x_drain1, y_bulk_top, z1, x_diffusion_spacing};
Point(135) = { x_drain1, y_bulk_top, z4, x_diffusion_spacing};
Point(136) = { x_drain2, y_bulk_top, z1, x_diffusion_spacing};
Point(137) = { x_drain2, y_bulk_top, z4, x_diffusion_spacing};




Line(1) = {1, 101};
Line(3) = {106, 6};
Line(5) = {1, 8};
Line(6) = {108, 101};
Line(7) = {8, 7};
Line(8) = {108, 107};
Line(9) = {6, 7};
Line(10) = {107, 7};
Line(11) = {8, 108};
Line(12) = {106, 107};
Line(13) = {109, 112};
Line(14) = {112, 111};
Line(15) = {111, 11};
Line(16) = {110, 10};
Line(17) = {10, 9};
Line(18) = {109, 9};
Line(19) = {110, 109};
Line(20) = {11, 12};
Line(21) = {12, 9};
Line(22) = {12, 112};
Line(23) = {111, 110};
Line(24) = {10, 11};
Line(25) = {111, 115};
Line(26) = {15, 11};
Line(27) = {12, 16};
Line(28) = {112, 116};
Line(29) = {116, 16};
Line(30) = {115, 15};
Line(31) = {115, 116};
Line(32) = {15, 16};
Line(79) = {116, 103};
Line(80) = {104, 115};
Line(81) = {16, 3};
Line(82) = {4, 15};

// bulk oxide interface
Physical Surface(85) = {84};
Line(87) = {103, 104};
Line(90) = {4, 3};
Line(135) = {130, 132};
Line(136) = {133, 132};
Line(137) = {130, 131};
Line(138) = {133, 131};
Line(139) = {123, 121};
Line(140) = {120, 122};
Line(141) = {122, 123};
Line(142) = {120, 121};
Line(147) = {125, 121};
Line(148) = {120, 124};
Line(149) = {123, 127};
Line(150) = {122, 126};
Line(151) = {101, 125};
Line(152) = {125, 127};
Line(153) = {127, 103};
Line(154) = {1, 124};
Line(155) = {124, 126};
Line(156) = {126, 3};
Line(157) = {4, 136};
Line(158) = {136, 132};
Line(159) = {136, 134};
Line(160) = {134, 130};
Line(161) = {6, 134};
Line(162) = {131, 135};
Line(163) = {106, 135};
Line(164) = {104, 137};
Line(165) = {137, 135};
Line(166) = {133, 137};
Line Loop(167) = {29, 81, -156, -150, 141, 149, 153, -79};
Plane Surface(168) = {167};
Line Loop(169) = {82, -30, -80, 164, -166, 136, -158, -157};
Plane Surface(170) = {169};
Line Loop(171) = {32, 81, -90, 82};
Plane Surface(172) = {171};
Line Loop(173) = {32, -29, -31, 30};
Plane Surface(174) = {173};
Line Loop(175) = {31, 79, 87, 80};
Plane Surface(176) = {175};
Line Loop(177) = {25, 30, 26, -15};
Plane Surface(178) = {177};
Line Loop(179) = {25, 31, -28, 14};
Plane Surface(180) = {179};
Line Loop(181) = {22, 28, 29, -27};
Plane Surface(182) = {181};
Line Loop(183) = {20, 27, -32, 26};
Plane Surface(184) = {183};
Line Loop(185) = {14, 15, 20, 22};
Plane Surface(186) = {185};
Line Loop(187) = {16, 17, -18, -19};
Plane Surface(188) = {187};
Line Loop(189) = {19, 13, 14, 23};
Plane Surface(190) = {189};
Line Loop(191) = {18, -21, 22, -13};
Plane Surface(192) = {191};
Line Loop(193) = {17, -21, -20, -24};
Plane Surface(194) = {193};
Line Loop(195) = {24, -15, 23, 16};
Plane Surface(196) = {195};
Line Loop(197) = {140, 141, 139, -142};
Plane Surface(198) = {197};
Line Loop(199) = {1, 151, 147, -142, 148, -154};
Plane Surface(200) = {199};
Line Loop(201) = {155, -150, -140, 148};
Plane Surface(202) = {201};
Line Loop(203) = {139, -147, 152, -149};
Plane Surface(204) = {203};
Line Loop(205) = {136, -135, 137, -138};
Plane Surface(206) = {205};
Line Loop(207) = {135, -158, 159, 160};
Plane Surface(208) = {207};
Line Loop(209) = {137, 162, -163, 3, 161, 160};
Plane Surface(210) = {209};
Line Loop(211) = {138, 162, -165, -166};
Plane Surface(212) = {211};
Line Loop(213) = {8, 10, -7, 11};
Plane Surface(214) = {213};
Line Loop(215) = {6, -1, 5, 11};
Plane Surface(216) = {215};
Line Loop(217) = {12, 10, -9, -3};
Plane Surface(218) = {217};
Line Loop(219) = {151, 152, 153, 87, 164, 165, -163, 12, -8, 6};
Plane Surface(221) = {219};
Line Loop(222) = {155, 156, -90, 157, 159, -161, 9, -7, -5, 154};
Plane Surface(223) = {222};
Surface Loop(226) = {186, 188, 196, 194, 192, 190};
Volume(227) = {226};
Surface Loop(228) = {180, 178, 184, 182, 174, 186};
Volume(229) = {228};
Surface Loop(230) = {198, 202, 223, 168, 172, 170, 176, 221, 200, 216, 214, 218, 210, 206, 208, 212, 204, 174};
Volume(231) = {230};


// Box field to impose a step change in element sizes inside a box
Field[6] = Box;
Field[6].VIn = x_diffusion_spacing/5;
Field[6].VOut = x_diffusion_spacing/5;
Field[6].XMin = x_device_left; 
Field[6].XMax = x_device_right;
Field[6].YMax = y_bulk_bot;
Field[6].YMin = y_bulk_bot - diffusion_thickness;

// Use minimum of all the fields as the background field
Field[7] = Min;
Field[7].FieldsList = {6};
Background Field = 7;


/// source
Physical Surface("source_contact") = {198};
/// drain
Physical Surface("drain_contact") = {206};
/// gate contact
Physical Surface("gate_contact") = {188};
/// oxide bulk interface
Physical Surface("bulk_oxide_interface") = {174};
/// oxide gate interface
Physical Surface("gate_oxide_interface") = {186};
//// body contact
Physical Surface("body_contact") = {214};

Physical Volume("gate") = {227};
Physical Volume("oxide") = {229};
Physical Volume("bulk") = {231};
