from distutils.core import setup, Extension

module1 = Extension('lazysorted',
                    sources=['lazysorted.c'],
                    extra_compile_args=['-O0'])

setup(name='lazysorted',
      version=1.0',
      description='This is a demo package',
      ext_modules=[module1])
