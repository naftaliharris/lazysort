from distutils.core import setup, Extension

module1 = Extension('lazysorted', sources = ['lazysorted.c'])

setup(name = 'lazysorted',
      version = '1.0',
      description = 'This is a demo package',
      ext_modules = [module1])
