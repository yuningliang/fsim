################################################################################
# Makefile rules:
#   1. Two level directories(Assume buildroot is under root): 
#        buildroot/target_1
#        buildroot/target_2
#        ...
#        buildroot/target_n
#   2. A root makefile is placed under root directory at the same level as buidroot.
#   3. A bypass makefile is placed just under buildroot.
#   4. Makefiles for specific targets are under the corresponding target directory.
#   5. Common makefiles are placed under root directory.
#   6. All intermediate files for target "target_n" are under directory for "target_n".
################################################################################

#Makefile for all simulator targets
#For common definitions, please refer to Makefile.gsetup

ALL_COMPONENTS = libtcl_sl libfsim_sl2 fsim_sl1 fsim_sl2 psim_sl1 soc_sl1 soc_sl1_fast

SUBDIRS = $(ALL_COMPONENTS)

BUILDDIR=build

JOBS=-j 2

#Installation directory. You can override it by setting env variables
export INSTALL_DIR = $(TOOLROOT)/usr/bin

.PHONY: libtcl_sl libfsim_sl2 fsim_sl1 psim_sl1 fsim_sl2 soc_sl1 all clean install

#First target. Don't put any other targets before this one.
all: $(ALL_COMPONENTS)

fsim_sl1:
	gmake $(JOBS) -C $(BUILDDIR)/fsim_sl1

fsim_sl2:
	gmake $(JOBS) -C $(BUILDDIR)/fsim_sl2

libtcl_sl:
	gmake $(JOBS) -C $(BUILDDIR)/libtcl_sl

libcmod_sl1:
	gmake $(JOBS) -C $(BUILDDIR)/cmodel_sl1

libfsim_sl2:
	gmake $(JOBS) -C $(BUILDDIR)/libfsim_sl2

psim_sl1:
	gmake $(JOBS) -C $(BUILDDIR)/psim_sl1

soc_sl1:
	gmake $(JOBS) -C $(BUILDDIR)/soc_sl1

soc_sl1_fast:
	gmake $(JOBS) -C $(BUILDDIR)/soc_sl1_fast

clean install:
	@for i in $(SUBDIRS) ; do \
		gmake $@ -C $(BUILDDIR)/$$i; \
	done
