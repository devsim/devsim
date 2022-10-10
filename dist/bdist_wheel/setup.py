#setup.py
from setuptools import setup, Extension
from glob import glob

module1 = Extension('devsim.foo',
                    py_limited_api = True,
                    sources = ['foo.c'])
setup(name='devsim',
      version = '1.0.0',
      #include_package_data = True,
      packages=['devsim', 'devsim.python_packages'],
      package_dir = {
        '' : 'lib',
      },
      package_data={
        'devsim' : ['./*.so', '../testing/*.py',],
      },
      ext_modules = [module1],
      data_files = [
        ('', glob('*.*')),
        ('doc', glob('doc/devsim.pdf')),
        ('testing', glob('testing/*')),
        ('examples', glob('examples/**/*.*', recursive=True)),
      ]
    )
