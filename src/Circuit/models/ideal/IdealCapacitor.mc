# Need to make self-documenting with node description
# external_node va "anode";
title IdealCapacitor;
external_node vtop;
external_node vbot;

# need to set valid bounds
# need to set nodefault (Error if not given)
parameter C  "Capacitance" 1.0;

model iq C*(vtop-vbot);

equation evtop  0 ddt +iq;
equation evbot  0 ddt -iq;

link evtop vtop;
link evbot vbot;

