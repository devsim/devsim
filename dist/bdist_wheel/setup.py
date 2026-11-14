import os
import re
from glob import glob
from itertools import chain

from setuptools import Distribution, setup

wheel_abi = os.environ.get("DEVSIM_WHEEL_ABI", "abi3")
if wheel_abi not in ("abi3", "abi3t"):
    raise ValueError(f"Unsupported DEVSIM_WHEEL_ABI: {wheel_abi}")

cmdclass = {}
if wheel_abi == "abi3t":
    from setuptools.command.bdist_wheel import bdist_wheel

    class Abi3tWheel(bdist_wheel):
        def get_tag(self):
            _, _, platform = super().get_tag()
            return "cp315", "abi3t", platform

    cmdclass["bdist_wheel"] = Abi3tWheel


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True

    def is_pure(self):
        return False


def generate_data_files():
    data_dirs = ("doc", "testing", "examples")
    root_files = [path for path in glob("*") if os.path.isfile(path)]
    data_files = [("devsim_data", root_files)]
    for path, _, files in chain.from_iterable(os.walk(data_dir) for data_dir in data_dirs):
        install_dir = os.path.join("devsim_data", path)
        data_files.append((install_dir, [os.path.join(path, name) for name in files]))
    return data_files


with open("README.md", encoding="utf-8") as readme_file:
    readme = readme_file.read()

readme_base_url = os.environ.get("README_BASE_URL")
if readme_base_url:
    if not readme_base_url.endswith("/"):
        readme_base_url += "/"
    print(f"Fixing up base url for long description {readme_base_url}")
    readme = re.sub(
        r"\[([^]]+)\]+\((?!http[s]://)([^)]+)\)",
        lambda match: f"[{match.group(1)}]({readme_base_url}{match.group(2)})",
        readme,
    )

setup(
    data_files=generate_data_files(),
    distclass=BinaryDistribution,
    long_description=readme,
    long_description_content_type="text/markdown",
    cmdclass=cmdclass,
    options={"bdist_wheel": {"py_limited_api": "cp39" if wheel_abi == "abi3" else "cp315"}},
    url="https://devsim.org",
)
