# {dist}-{version}(-{build})?-{python}-{abi}-{platform}.whl

from setuptools import setup
import sys

SETUP = {
    "name"             : "robworld",
    "version"          : "1.1.0",
    "description"      : "Control de robots en el mundo de robots",
    "license"          : "MIT",
    "author"           : "Roberto Carrasco",
    "author_email"     : "titos.carrasco@gmail.com",
    "maintainer"       : "Roberto Carrasco",
    "maintainer_email" : "titos.carrasco@gmail.com",
    "packages"         : [ "robworld" ],
    "package_dir"      : { "robworld": "pyclient/" },
}

setup( **SETUP )
