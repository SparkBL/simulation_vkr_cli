import sys

from pybind11 import get_cmake_dir
# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension("rq_simulation",
        ["src/binding.cpp"],
        include_dirs=['./include/'],
        # Example: passing in the version to the compiled code
        extra_compile_args = ["-Ofast","-std=c++17"],
        define_macros = [('VERSION_INFO', __version__)],
        ),
]
setup(
    name='rq_simulation',
    version='1.0.0',
    author='Alexey Blaginin',
    author_email='alex-b.l@yandex.ru',
    description='A minimal self-contained tool for RQ system analysis',
    long_description=open("README.rst").read(),
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    cmdclass={"build_ext": build_ext},
    url="https://github.com/SparkBL/simulation_vkr_cli.git",
    zip_safe=False,
    install_requires=[
        "numpy"
    ]
)