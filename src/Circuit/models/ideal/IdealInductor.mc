# Need to make self-documenting with node description
# external_node va "anode";
title IdealInductor;
external_node vtop;
external_node vbot;
internal_node I;

# need to set valid bounds
# need to set nodefault (Error if not given)
parameter L  "Inductance" 1.0;

model vl L*I;
model vdiff vbot-vtop;

equation evtop  I ddt 0;
equation evbot -I ddt 0;
equation eI     vdiff ddt vl; 

link evtop vtop;
link evbot vbot;
link eI   I;

