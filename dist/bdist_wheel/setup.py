#setup.py
from setuptools import setup, Extension
from glob import glob

module1 = Extension('devsim.foo',
                    py_limited_api = True,
                    sources = ['foo.c'])
setup(name='devsim',
      url = 'https://devsim.org',
      author = "Juan Sanchez and others",
      author_email = "info@devsim.com",
      description = "DEVSIM: TCAD Device Simulator",
      version = '1.0.0',
      #include_package_data = True,
      packages=['devsim', 'devsim.python_packages'],
      package_dir = {
        '' : 'lib',
      },
      package_data={
        'devsim' : ['./*.so', './*.dylib', './*.pyd',],
      },
      ext_modules = [module1],
      data_files = [
        ('', glob('*.*')),
        ('doc', glob('doc/devsim.pdf')),
        ('testing', glob('testing/*.*')),
        ('examples', glob('examples/**/*.*', recursive=True)),
      ],
         classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python :: 3",
        "Topic :: Scientific/Engineering :: Electronic Design Automation (EDA)",

    ],
    )
