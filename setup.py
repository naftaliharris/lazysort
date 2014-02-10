from distutils.core import setup, Extension

module1 = Extension('lazysorted', sources=['lazysorted.c'])

f = open("README.txt")
readme = f.read()
f.close()

setup(name='lazysorted',
      version='0.1.1',
      description='A partially and lazily sorted list data structure',
      author='Naftali Harris',
      author_email='naftaliharris@gmail.com',
      url='www.naftaliharris.com',
      keywords=["sort", "sorting", "partial", "lazy", "list"],
      classifiers=[
          "Development Status :: 4 - Beta",
          "Intended Audience :: Developers",
          "License :: OSI Approved :: BSD License",
          "Operating System :: OS Independent",
          "Programming Language :: Python :: 2",
          "Programming Language :: Python :: 2.5",
          "Programming Language :: Python :: 2.6",
          "Programming Language :: Python :: 2.7",
          "Programming Language :: Python :: 3",
          "Programming Language :: Python :: 3.1",
          "Programming Language :: Python :: 3.2",
          "Programming Language :: Python :: 3.3",
          "Programming Language :: Python :: Implementation :: CPython",
          "Topic :: Software Development :: Libraries :: Python Modules",
      ],
      ext_modules=[module1],
      long_description=readme)
