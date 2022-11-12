import platform
from ctypes import *
import sys
import os
import site

class DevsimCheck:
    def check_devsim(self):
        mkl_found = {}
        dir_path = os.path.dirname(os.path.realpath(__file__))
        dir_name = os.path.split(dir_path)[-1]
        pieces = dir_name.split('_')
        if len(pieces) != 3 or pieces[0] != 'devsim':
            raise RuntimeError("This script needs to be run from a devsim directory")
        ret = {
            'dir_path' : dir_path,
            'dir_name' : dir_name,
            'build_type' : pieces[1],
            'version' : pieces[2]
        }
        return ret

#    def find_mkl(self):
#        mkl_found = {}
#        DLL_NAMES = {
#            'Darwin' : {'mkl' : 'libmkl_rt.1.dylib'},
#            'Linux' : {'mkl' : 'libmkl_rt.so.1'},
#            'Windows' : {'mkl' : 'mkl_rt.1.dll'},
#        }
#        mkl_found['mkl_loaded'] = False
#        try:
#            mydll = cdll.LoadLibrary(DLL_NAMES[self.osname]['mkl'])
#            mkl_found['mkl_loaded'] = True
#            print('''
#INFO: Intel MKL %s loaded successfully
#''' % DLL_NAMES[self.osname]['mkl'])
#        except:
#            print('''
#ERROR: Intel MKL %s could not be dynamically loaded.
#ERROR: If you are using Anaconda/Miniconda, please issue this command:
#ERROR: conda install numpy mkl
#ERROR:
#ERROR: Please note that the MKL shared library is now versioned, so please ensure
#ERROR: The exact file name is available in your installation.
#''' % (DLL_NAMES[self.osname]['mkl'],))
#
#
#        mkl_found['has_conda_mkl'] = False
#        if self.conda['is_conda']:
#            if self.osname == 'Windows':
#                cpath = os.path.join(self.conda['CONDA_PREFIX'], 'Library', 'bin', DLL_NAMES[self.osname]['mkl'])
#            else:
#                cpath = os.path.join(self.conda['CONDA_PREFIX'], 'lib', DLL_NAMES[self.osname]['mkl'])
#            if not os.path.exists(cpath):
#                print('''ERROR: %s not found in CONDA_PREFIX''' % DLL_NAMES[self.osname]['mkl'])
#                if mkl_found['mkl_loaded']:
#                    print('''ERROR: This could mean that the Intel MKL may be loaded from somewhere else on your system.
#''')
#                mkl_found['has_conda_mkl'] = False
#            else:
#                mkl_found['has_conda_mkl'] = True
#        return mkl_found
#
#
#    def find_vcruntime(self):
#        vcruntime_found = False
#        try:
#            mydll = cdll.LoadLibrary('MSVCP140.DLL')
#            vcruntime_found = True
#            print('''
#INFO: Visual Studio 2019 C++ Redistributable loaded successfully
#''' % DLL_NAMES[self.osname]['mkl'])
#        except:
#            print('''
#ERROR: Visual Studio 2019 C++ redistributable could not be loaded.
#ERROR: Please download and install from here:
#ERROR: https://visualstudio.microsoft.com/downloads/
#ERROR: https://aka.ms/vs/16/release/VC_redist.x64.exe
#''')
#        return vcruntime_found
#
#    def check_conda(self):
#        ret = {
#            'is_conda' : os.path.exists(os.path.join(sys.base_prefix, 'conda-meta'))
#        }
#
#        if 'CONDA_PREFIX' in os.environ:
#            ret['CONDA_PREFIX'] = os.environ['CONDA_PREFIX']
#            if self.python['site-packages'].find(ret['CONDA_PREFIX']) != 0:
#                RuntimeError("site-packages not found as a subdirectory of CONDA_PREFIX")
#        return ret

    def check_python(self):
        ret = {}
        try:
            ppath = os.environ['PYTHONPATH']
        except:
            ppath = None
        ret = {
          'PYTHONPATH' : ppath}

        for s in ('USER_BASE', 'USER_SITE', 'ENABLE_USER_SITE'):
            ret[s] = site.__dict__[s]

        ret['site-packages'] = None
        for s in sys.path:
            if os.path.split(s)[-1] == 'site-packages':
                ret['site-packages'] = s
                break

        print("Python Version " + ".".join([str(x) for x in sys.version_info[0:3]]))

        if sys.version_info[0] != 3 or sys.version_info[1] < 6:
            raise RuntimeError("A Python version of 3.6 or higher is required")
        return ret

    def install_link(self):
        setup_py='''#!/usr/bin/env python

from distutils.core import setup
#https://docs.python.org/3/distutils/setupscript.html
setup(name='devsim',
      version='%s',
      description='DEVSIM TCAD Semiconductor Device Simulator',
      author='Juan Sanchez',
      author_email='info@devsim.com',
      url='https://devsim.org',
      license='Apache-2.0',
      packages=['devsim', 'symdiff'],
)
''' % self.devsim['version']
        ofile = os.path.join(self.devsim['dir_path'], 'lib', 'setup.py')
        print('''
INFO: Writing %s
INFO:''' % ofile)
        with open(ofile, 'w') as of:
          of.write(setup_py)


        print('''\
INFO: Please type the following command to install devsim:
INFO: pip install -e lib
INFO:
INFO: To remove the file, type:
INFO: pip uninstall devsim
''')

    def __init__(self):
        self.osname = platform.system()
        self.devsim = self.check_devsim()
#        self.vcruntime = None
#        if self.devsim['build_type'] == 'win64':
#            self.vcruntime = self.find_vcruntime()
        self.python= self.check_python()
#        self.conda = self.check_conda()
#        self.mkl = self.find_mkl()



if __name__ == "__main__":
# check version 3, 64 bit etc, msys
    dc = DevsimCheck()
#    if not dc.conda['is_conda']:
#        raise RuntimeError("Currently installation from a conda environment is the only one supported")
#    if not dc.mkl['has_conda_mkl']:
#        raise RuntimeError("ERROR MKL NOT FOUND")
    dc.install_link()





