# Need to make self-documenting with node description
# external_node va "anode";
title mctest_user;
external_node va;
external_node vc;
internal_node vx;
declarefunc cos(x);
definefuncdiff sin(x), cos(x);
definefuncdiff cos(x), -sin(x);

# need to set valid bounds
# need to set nodefault (Error if not given)
parameter Is "sat current" 1.0;
parameter R  "series R" 1.0;
parameter n  "ideality" 1.0;
parameter VT "thermal voltage" 1.0;
parameter C "Capacitance" 1.0;

model id Is*cos((vx-vc)/(n*VT))-Is;
#model d_id_d_vx (id+Is)/(n*VT);
#model d_id_d_vc -d_id_d_vx;
model ir (va-vx)/R;

model qd C*(vx-vc);

equation evx id-ir ddt qd;
equation eva ir ddt 0;
equation evc -id ddt -qd;

link evx vx;
link eva va;
link evc vc;

