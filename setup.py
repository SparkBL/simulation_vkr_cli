import sys

from pybind11 import get_cmake_dir
# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup,find_packages

__version__ = "1.0.0"


ext_modules = [
    Pybind11Extension("_simulation",
        ["./rq_analysis/src/binding.cpp"],
        include_dirs=['./rq_analysis/include/'],
        # Example: passing in the version to the compiled code
        extra_compile_args = ["-O3","-Ofast","-std=c++17"],
        define_macros = [('VERSION_INFO', __version__)],
        ),
]
setup(
    name='rq_analysis',
    version=__version__,
    author='Alexey Blaginin',
    author_email='alex-b.l@yandex.ru',
    description='A minimal self-contained tool for RQ system analysis',
    long_description=open("README.rst").read(),
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    cmdclass={"build_ext": build_ext},
    url="https://github.com/SparkBL/simulation_vkr_cli.git",
    zip_safe=False,
    packages=find_packages(),
    install_requires=[
        "numpy","scipy"
    ]
)