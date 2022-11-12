[![Build Status](https://travis-ci.org/devsim/devsim.svg?branch=master)](https://travis-ci.org/devsim/devsim)
[![Build status](https://ci.appveyor.com/api/projects/status/github/devsim/devsim?branch=master&svg=true)](https://ci.appveyor.com/project/devsim/devsim)

# DEVSIM

## Introduction:
**DEVSIM** - TCAD Device Simulator

**DEVSIM** is a tool for TCAD Device Simulation, using finite volume methods.  The source code is provided by DEVSIM LLC.

## Website:

The official website is here:

[https://devsim.org](https://devsim.org)

## Installation:

Binary releases are available at [Zenodo](https://doi.org/10.5281/zenodo.1186952).  Please see [INSTALL.md](INSTALL.md) for installation instructions from binary or source.

## Citing This Work:

Please see [CITATION.md](CITATION.md).  *Please do not cite this Github Repository as it will be moving in the future.*

## License:

DEVSIM is licensed under the [Apache 2.0 License](https://www.apache.org/licenses/LICENSE-2.0.html).  Example scripts are also provided under this license.  Other files are subject to the license terms of their copyright owners.
Please see [LICENSE](LICENSE) and [NOTICE](NOTICE) for license terms.

## Documentation:

The PDF documentation is located in ``doc/devsim.pdf``.  An online version of the documentation is available at [https://devsim.net](https://devsim.net).  A list of documentation resources is available [online](https://devsim.org/introduction.html#documentation).

The repository for the documentation is at [https://github.com/devsim/devsim_documentation](https://github.com/devsim/devsim_documentation).

## Supported Platforms:
* macOS 10.13 (High Sierra)
* Microsoft Windows (64 bit)
* Red Hat 7 (Centos Compatible)


## Software Features:
* Python scripting 
* DC, small-signal AC, impedance field method, transient
* User specified partial differential equations (PDE).
* 1D, 2D, and 3D simulation
* 1D, 2D mesher
* Import 3D meshes.
* 2D cylindrical coordinate simulation
* ASCII file format with PDE embedded.

## Support:

For support and general discussion, please join our forum:
[https://forum.devsim.org](https://forum.devsim.org)

## Contributing:

Please see:
[Contribution guidelines for this project](CONTRIBUTING.md)

## Testing

If you wish to see simulation results, or run the regression tests yourself.  They are available from:

| Name | Description |
| --- | --- |
| [devsim_tests_msys](https://github.com/devsim/devsim_tests_msys) | Windows MSYS Build |
| [devsim_tests_win64](https://github.com/devsim/devsim_tests_win64) | Windows Visual Studio 2022 Build |
| [devsim_tests_macos_gcc_x86_64](https://github.com/devsim/devsim_tests_macos_gcc_x86_64) | macOS Build |
| [devsim_tests_linux_x86_64](https://github.com/devsim/devsim_tests_linux_x86_64) | Linux |

*The results are platform dependent due to differences in the compiler, operating system, and math libraries used on each platform.*

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

