[![Build Status](https://travis-ci.org/devsim/devsim.svg?branch=master)](https://travis-ci.org/devsim/m)
[![Build status](https://ci.appveyor.com/api/projects/status/github/devsim/devsim?branch=master&svg=ue)](https://ci.appveyor.com/project/devsim/devsim)
[![DOI](https://joss.theoj.org/papers/10.21105/joss.03898/status.svg)](https://doi.org/10.21105/joss.03898)

# DEVSIM

## Introduction:
**DEVSIM** - TCAD Device Simulator

**DEVSIM** is a tool for TCAD Device Simulation, using finite volume methods.  The source code is provided by DEVSIM LLC.

## Website:

The official website is here:

[https://devsim.org](https://devsim.org)

## Software Features:

* Python scripting 
* DC, small-signal AC, impedance field method, transient
* User specified partial differential equations (PDE).
* Extended floating point precision
* 1D, 2D, and 3D simulation
* 1D, 2D mesher
* Import 3D meshes.
* 2D cylindrical coordinate simulation
* ASCII file format with PDE embedded.

## Usage In Research:

Please see this [link](https://docs.google.com/spreadsheets/d/11TpoCrNzKwWDDmjKtP1d5bCiwn8WYNJkm1ZleJI0_k4/edit?usp=sharing) for a list of published research papers using or referring to the DEVSIM simulator.

## Installation:

Please see [INSTALL.md](INSTALL.md) for installation instructions.  Please see [BUILD.md](BUILD.md) for instructions to build from source.

## Citing This Work:

Please see [CITATION.md](CITATION.md).  *Please do not cite this Github Repository as it will be moving in the future.*

## License:

DEVSIM is licensed under the [Apache 2.0 License](https://www.apache.org/licenses/LICENSE-2.0.html).  Example scripts are also provided under this license.  Other files are subject to the license terms of their copyright owners.
Please see [LICENSE](LICENSE) and [NOTICE](NOTICE) for license terms.

## Documentation:

The PDF documentation is located in ``doc/devsim.pdf``.  An online version of the documentation is available at [https://devsim.net](https://devsim.net).  A list of documentation resources is available [online](https://devsim.org/introduction.html#documentation).  Recent changes are available in [CHANGES.md](CHANGES.md).

The repository for the documentation is at [https://github.com/devsim/devsim_documentation](https://github.com/devsim/devsim_documentation).

## Supported Platforms:

| OS | Version | Architecture |
| --- | --- | --- |
| macOS | Monterey 12.5 | `arm64`, `x86_64` |
| Microsoft Windows | Windows 10 | `x64` |
| Linux | Red Hat 7 (Centos Compatible) | `aarch64`, `x86_64` |

## Code of Conduct:

Please see [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## Support:

For support and general discussion, please join our forum:
[https://forum.devsim.org](https://forum.devsim.org)

## Contributing:

Please see:
[Contribution guidelines for this project](CONTRIBUTING.md)

## Testing

Please see [TEST.md](TEST.md).

## Related Projects

### Used directly by the simulator
| Name | Description |
| --- | --- |
| [symdiff](https://github.com/devsim/symdiff) | Symbolic differentiation engine for the simulator |
| [devsim_documentation](https://github.com/devsim/devsim_documentation) | Documentation for the simulator |

### Extended examples
| Name | Description |
| --- | --- |
| [devsim_bjt_example](https://github.com/devsim/devsim_bjt_example) | Bipolar Junction Transistor example |
| [devsim_density_gradient](https://github.com/devsim/devsim_density_gradient) | Quantum Corrections to Drift Diffusion simulation |
| [devsim_3dmos](https://github.com/devsim/devsim_3dmos) | 3D Mosfet example used in publication |
| [devsim_misc](https://github.com/devsim/devsim_misc) | Miscellaneous scripts |

