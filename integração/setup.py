from setuptools import setup, Extension
import pybind11

functions_module = Extension(
    'meu_modulo', # Nome do módulo que você vai importar no Python
    sources=['solver.cpp'], # Nome do seu arquivo C++
    include_dirs=[pybind11.get_include()],
    language='c++'
)

setup(
    name='meu_modulo',
    version='1.0',
    ext_modules=[functions_module],
)