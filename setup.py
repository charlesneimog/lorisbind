import os

num_cores = os.cpu_count()

from skbuild import setup
from setuptools import find_packages


setup(
    name="lorisbind",
    version="0.1.0",
    description="Python bindings for Loris using pybind11",
    author="Charles K. Neimog",
    packages=find_packages(),
    cmake_minimum_required_version="3.20",
    cmake_install_dir=".",
    cmake_args=[
        "-DPYBIND11_FINDPYTHON=ON",
        f"-DCMAKE_BUILD_PARALLEL_LEVEL={num_cores}",  # Habilita compilação paralela
    ],
    install_requires=["pybind11"],
)
