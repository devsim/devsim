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

`DEVSIM` is a technology computer-aided design (TCAD) software for semiconductor device simulation.  By solving the equations for electric fields and current flow, it simulates the electrical behavior of semiconductor devices, such as transistors.  This capability is useful for calibration to model existing fabricated devices.  It is also possible to explore novel device structures and exotic materials, saving the number of costly and time consuming manufacturing iterations.

# Statement of need

`DEVSIM` is TCAD device simulation package written in C++, with a Python front end.  It is capable of simulating 1-D, 2-D and 3-D structures with models describing advanced physical effects [@devsim:home].  Software with TCAD simulation capabilities have existed in commercial and academic research tools for quite some time.  The number of open source offerings, meeting the OSI definition, has been growing (cf. [@cogenda; @charon; @solcore]).  Due to the expanse of TCAD simulation algorithms and models, each simulator would need to be evaluated for its desired application.

A unique feature of the software is its scripting model interface.  It symbolically evaluates expressions and their derivatives.  These expressions are evaluated on the mesh structure by the C++ engine.  This symbolic engine is tailored toward modeling advanced TCAD simulation models, and is directly compatible with the finite volume methods employed in the equation discretization [@sanchez:ieee].  Researchers are able to implement their models using a Python scripting interface, avoiding the need to recompile the software, enabling rapid development of new physical models.

While `DEVSIM` has limited capabilities for the creation of 1-D and 2-D meshes, the Python interface allows the import of mesh structures from any format using a triangular representation (in 2-D) or a tetrahedral representation (in 3-D).  This makes it possible for the user to utilize high quality open source meshing solutions [@Gmsh:2009; @tetgen].

This software framework has been utilized to investigate device behavior in ferroelectric transistors [@chen:pssa].  The open source nature of `DEVSIM` has made it possible for researchers to incorporate its functionality into their own solution frameworks [@hulbert:thesis; @hulbert:software].

# References
