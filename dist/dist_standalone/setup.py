from setuptools import setup
from glob import glob
import os
import re

def fix_readme(text):
    regex = re.compile('\[([^]]+)\]+\((?!http[s]:\/\/)([^)]+)\)')
    base_url = os.environ.get('README_BASE_URL')
    if base_url:
        if base_url[-1] != '/':
            base_url += '/'
        print("Fixing up base url for long description {0}".format(base_url))
        text = regex.sub(lambda m, urlbase=base_url : r'[{0}]({1})'.format(m.group(1), urlbase + m.group(2)), text)
    return text

with open('README.md', 'r') as ifile:
    text = ifile.read()
    text = fix_readme(text)

setup(
      packages=['devsim', 'devsim.python_packages'],
      package_data={
        'devsim' : ['./*.so',],
      },
      long_description = text,
      long_description_content_type="text/markdown",
    )
