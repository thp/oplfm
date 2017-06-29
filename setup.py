from distutils.core import setup
from distutils.extension import Extension

SOURCES = [
    'src/oplfm_python.c',
    'src/oplfm.c', 'src/instrument.c', 'src/playback.c', 'src/stream.c',
    'src/woody/woodyopl.cpp', 'src/woody/woody.cpp',
]

setup(name='oplfm', version='1.0', description='OPL2/OPL3 Instrument Toolkit', license='GPL',
      author='Thomas Perl', author_email='m@thp.io',
      url='https://github.com/thp/oplfm',
      ext_modules=[Extension('oplfm', SOURCES, include_dirs=['include'], libraries=['portaudio'])])
