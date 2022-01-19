---
title: 'DEVSIM: A TCAD Semiconductor Device Simulator'
tags:
  - Python
  - semiconductor
  - TCAD
  - simulation
authors:
  - name: Juan E. Sanchez #^[co-first author] # note this makes a footnote saying 'co-first author'
    orcid: 0000-0002-6783-0194
    affiliation: "1" # (Multiple affiliations must be quoted)
affiliations:
 - name: DEVSIM LLC, Austin, TX
   index: 1
date: 23 October 2021
bibliography: paper.bib

---

# Summary

`DEVSIM` is technology computer-aided design (TCAD) software for semiconductor device simulation.  By solving the equations for electric fields and current flow, it simulates the electrical behavior of semiconductor devices, such as transistors.  It can be used to model existing, fabricated devices for calibration purposes.  It is also possible to explore novel device structures and exotic materials, reducing the number of costly and time consuming manufacturing iterations.

# Statement of need

`DEVSIM` is a TCAD device simulation package written in C++, with a Python front end.  It is capable of simulating 1-D, 2-D and 3-D structures with models describing advanced physical effects [@devsim:home].  Software tools with TCAD simulation capabilities have existed in commercial and academic research tools for quite some time.  The number of open source offerings meeting the open source definition (OSD), as defined by the Open Source Initiative [@OSI:2007], has been growing [@cogenda; @charon; @solcore].

Due to the expanse of TCAD simulation algorithms and models, it is important to note that this software fits into the class of continuum PDE based solvers for drift-diffusion semiconductor simulation [@Selberherr].  `DEVSIM` is intended to compare directly with the commercial TCAD offerings, such as Sentaurus Device from Synopsys, or the Victory Device Simulator from Silvaco.  The identifying factors for this class of software are:

* Sharfetter-Gummel discretization of the electron and hole continuity equations
* DC, transient, small-signal AC, and noise solution algorithms
* Solution of 1D, 2D, and 3D unstructured meshes
* Advanced models for mobility and semiclassical approaches for quantum effects

While `DEVSIM` is not as complete as the commercial offerings, the project strives to fulfill the gaps by developing an open source community. To our knowledge the only OSD-licensed simulators having most of these features are Solcore, Genius Semiconductor Device Simulator, and Charon [@solcore;@cogenda;@charon].  There are other simulators which provide their source code under restrictive academic research licenses.

A unique feature of DEVSIM is its scripting model interface, which symbolically evaluates expressions and their derivatives. In this sense, DEVSIM is tailored toward scripting advanced TCAD simulation models.  The symbolic engine is maintained as a separate project, `SYMDIFF` [@symdiff].

These expressions are evaluated on the mesh structure by the C++ engine.
It is directly compatible with the finite volume methods employed in the equation discretization [@sanchez:ieee].  Researchers are able to implement their models using a Python scripting interface, avoiding the need to recompile the software, enabling rapid development of new physical models.

While `DEVSIM` has limited capabilities for the creation of 1-D and 2-D meshes, the Python interface allows the import of mesh structures from any format using a triangular representation (in 2-D) or a tetrahedral representation (in 3-D).  This makes it possible for the user to utilize high quality open source meshing solutions [@Gmsh:2009; @tetgen].

This software framework has been utilized to investigate device behavior in ferroelectric transistors [@chen:pssa].  The open source nature of `DEVSIM` has made it possible for researchers to incorporate its functionality into their own solution frameworks [@hulbert:thesis; @hulbert:software].  In addition, the simulator has been successful in simulating deep level transient spectra, beyond the measurement detection limit [@Lauwaert_2021].

# References
