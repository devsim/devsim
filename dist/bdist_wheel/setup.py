from setuptools import setup, Distribution
from glob import glob
from itertools import chain
import os

#https://stackoverflow.com/questions/35112511/pip-setup-py-bdist-wheel-no-longer-builds-forced-non-pure-wheels/36886459#36886459
#https://stackoverflow.com/questions/24071491/how-can-i-make-a-python-wheel-from-an-existing-native-library/24793171#24793171
class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True

#https://stackoverflow.com/questions/27829754/include-entire-directory-in-python-setup-py-data-files/65432634#65432634
def generate_data_files():
    data_files = []
    data_dirs = ('doc', 'testing', 'examples')
    for path, dirs, files in chain.from_iterable(os.walk(data_dir) for data_dir in data_dirs):
        install_dir = os.path.join('devsim_data', path)
        list_entry = (install_dir, [os.path.join(path, f) for f in files])
        data_files.append(list_entry)
    return data_files

data_files = [('devsim_data', glob('*.*'))]
data_files.extend(generate_data_files())

setup(
      packages=['devsim', 'devsim.python_packages'],
      package_dir = {
        '' : 'lib',
      },
      package_data={
        'devsim' : ['./*.so', './*.dll', './*.dylib', './*.pyd',],
      },
      distclass=BinaryDistribution,
      data_files = data_files
    )
