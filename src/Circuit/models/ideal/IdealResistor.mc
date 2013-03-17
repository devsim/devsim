# Need to make self-documenting with node description
# external_node va "anode";
title IdealResistor;
external_node vtop;
external_node vbot;

# need to set valid bounds
# need to set nodefault (Error if not given)
parameter R  "Resistance" 1.0;

model G  (1/R);
model ir G*(vtop-vbot);

equation evtop  ir ddt 0;
equation evbot -ir ddt 0;

link evtop vtop;
link evbot vbot;

