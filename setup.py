from distutils.core import setup, Extension

module1 = Extension('lazysorted',
                    sources=['lazysorted.c'],
                    extra_compile_args=['-O0'],
                    undef_macros=['NDEBUG'])

setup(name='lazysorted',
      version='1.0',
      description='TODO',
      ext_modules=[module1])
