
# Contributing

Thank you for your interest in this project.

TCAD simulation is a broad topic utilizing skills in:

* Programming
* Semiconductor Modeling
* Numerics
* Simulation Methods
* Meshing
* Extraction
* Visualization
* Documentation
* Testing

If you would like to participate in this project, please join our email forum:
https://forum.devsim.org
and let us know how you would like to help.

Please see the [Testing](README.md#Testing) and [Related Projects](README.md#Related-Projects) section in [README.md](README.md) for additional places where help is needed.

## Contributing on Github

The DEVSIM source code is currently hosted on Github at https://github.com/devsim/devsim. We accept pull requests.

Before contributing, you may want to explore the [Issues](https://github.com/devsim/devsim/issues) page to see if there is an issue you can help with. There is also active discussion on the [forum](https://forum.devsim.org).

If you are ready to contribute, fork the repository, make your changes, and submit a pull request. We use [pre-commit hooks](https://pre-commit.com/) to enforce formatting and style. Please install pre-commit using:
```
pip install pre-commit # or use a relevant system package manager
pre-commit install
```
The hooks (currently, only code formatting) will then run when you commit changes. You can also manually run the hooks using:
```
pre-commit run
```
When you make a pull request, the hooks will run server-side. The checks will fail if code is improperly formatted.

## Licensing

Note that we request all contributions to be licensed under the [Apache License Version 2.0, January 2004](https://apache.org/licenses/LICENSE-2.0). This is explained in the pull request template.