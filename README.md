bambamc
=======

This package contains mainly a lightweight C implementation of the read name
collation code from the larger libmaus/biobambam C++ project.

The github repository does not contain the final configure script but only
the source files passed to autoconf/automake. Please use

	autoreconf -i -f

to obtain a set of working configuration scripts.

The library can then be compiled using

	./configure
	make

And installed using

	make install

An alternative installation path can be set using

	./configure --prefix=/path/to/installation

