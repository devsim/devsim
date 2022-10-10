from setuptools import setup, Distribution
from glob import glob

#https://stackoverflow.com/questions/35112511/pip-setup-py-bdist-wheel-no-longer-builds-forced-non-pure-wheels/36886459#36886459
#https://stackoverflow.com/questions/24071491/how-can-i-make-a-python-wheel-from-an-existing-native-library/24793171#24793171
class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True

setup(
      packages=['devsim', 'devsim.python_packages'],
      package_dir = {
        '' : 'lib',
      },
      package_data={
        'devsim' : ['./*.so', './*.dll', './*.dylib', './*.pyd',],
      },
      distclass=BinaryDistribution,
      data_files = [
        ('devsim_data', glob('*.*')),
        ('devsim_data/doc', glob('doc/devsim.pdf')),
        ('devsim_data/testing', glob('testing/*.*')),
        ('devsim_data/examples', glob('examples/**/*.*', recursive=True)),
      ],
    )
