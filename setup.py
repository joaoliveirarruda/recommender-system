from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

ext_modules = [
    Pybind11Extension(
        "recommender",
        ["bindings.cpp", "listaCompras.cpp", "similaridade.cpp", "recomendacao.cpp"],
        cxx_std=11,
    ),
]

setup(
    name="recommender",
    version="1.0.0",
    description="Sistema de recomendação de produtos (C++ via pybind11)",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
