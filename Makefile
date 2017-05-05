PYINCLUDES = -I $(PYTHIA8DATA)/../../../include
PYLDLIBS = -L $(PYTHIA8DATA)/../../../lib -lpythia8
CFLAGS = -g $(PYINCLUDES)
LIBDIR = $(HOME)/lib
BINDIR = $(HOME)/bin
SHLIB =
CXXFLAGS = -g $(PYINCLUDES)
LDFLAGS = $(PYLDLIBS)
include $(HOME)/mkdep_makefile/config
