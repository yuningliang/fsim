#Makefile for fsim_sl1

#Common definition
TARGET = fsim_sl1
EXEC = pcore
BUILD_ROOT=../..
VPATH=../..
TCL_LIB=tcl_sl

-include $(BUILD_ROOT)/Makefile.gsetup

#Append include directories
INC_DIRS += \
-I$(BUILD_ROOT)/include \
-I$(BUILD_ROOT)/shell/include \
-I$(BUILD_ROOT)/simkernel/include \
-I$(BUILD_ROOT)/com/include \
-I$(BUILD_ROOT)/sl1/include \
-I$(BUILD_ROOT)/utils/include \
-I$(BUILD_ROOT)/shell/tclsh/generic \
-I$(BUILD_ROOT)/shell/tclsh/readline \

#Append compilation flags
#CFLAGS += -DIs_True_On=1 -static -D_RAREG_TO_31=1
CFLAGS += -DIs_True_On=1 -static -D_SIMULATE_LCD=1 -D_SIMULATE_INTERRUPT=1 \
-D_INCLUDE_REG_DETAIL=0 -D_WARN_LAYOUT=0 -D_WARN_SHELL=0 -DNEED_LONG_JMP \
-D_ERROR_ALL_FATAL=1 -D_LOG_LOOP=1

#CFLAGS +=-DGPR_ZERO_WARN
#Append extra compilation flags
EXTRA_CFLAGS +=

#Append linker flags
LD_FLAGS += -l$(TCL_LIB) -lrt -ltermcap -lpthread -lX11 -lffe

#Append library searching path
LIB_DIRS += -L$(BUILD_ROOT)/lib

#Source files
#Source files shared by all architecture
KERNEL_SOURCE = \
simkernel/gui.cpp \
simkernel/accesslist.cpp \
simkernel/memory.cpp \
simkernel/symtable.cpp 


SL1_SOURCE = \
sl1/sl1decoder.cpp \
sl1/sl1disasm.cpp \
sl1/sl1exec.cpp \
sl1/sl1instr.cpp \
sl1/sl1instrtable.cpp \
com/cmmu.cpp \
sl1/sl1mmu.cpp \
sl1/sl1reg.cpp \
sl1/sl1shell.cpp \
sl1/sl1ffe.cpp \
sl1/sl1viterbi.cpp \
sl1/sl1traceback.cpp \
main/sl1/sl1main.cpp

SHELL_SOURCE = \
shell/shellbase.cpp \
shell/tclapi.cpp \
shell/arch4gdb.cpp

UTILS_SOURCE = \
utils/chameleon.cpp \
utils/configfile.cpp \
utils/messg.cpp \
utils/utils.cpp \
utils/elf_reader.cpp \
utils/longjmp.cpp

ALL_SOURCE := $(KERNEL_SOURCE) $(SL1_SOURCE) $(SHELL_SOURCE) $(SLCOM_SOURCE) $(UTILS_SOURCE)
#ALL_SOURCE := $(SHELL_SOURCE) $(UTILS_SOURCE)

#object files. Object files are placed in current directory
OBJS=$(patsubst %.cpp, %.o, $(notdir $(ALL_SOURCE)))

#Target
all: $(TARGET)

header: $(BUILD_ROOT)/sl1/include/regdefs.h

sl1.vh:
	svn cat file:///srcrt/svnroot/sl1/rtl/include/sl1.vh | sed 's/SL1_//g' > sl1.vh

$(BUILD_ROOT)/sl1/include/regdefs.h: sl1.vh
	perl ../convert.pl $< $@
	 
$(TARGET): $(OBJS) $(BUILD_ROOT)/lib/lib$(TCL_LIB).a $(BUILD_ROOT)/lib/libreadline.a $(BUILD_ROOT)/lib/libhistory.a
	$(LINK) $(LIB_DIRS) -o $(TARGET) $^  $(LD_FLAGS) 

#Compilation  and dependency generation rules
__SRC = $(filter %/$(patsubst %.o,%.cpp,$@),$(ALL_SOURCE))
__DEP =$(patsubst %.o,%.d, $@)
$(OBJS): $(BUILD_ROOT)/sl1/include/regdefs.h
#	echo INC_DIRS:$(INC_DIRS)
#	@echo Building $@ from $(__SRC) by $(__DEP)
	$(CPP)  $(CFLAGS) $(EXTRA_CFLAGS) $(INC_DIRS) $(BUILD_ROOT)/$(__SRC)
#	@echo Creating dependency file $(__DEP) for $@
	@$(GEN_DEP)  $(CFLAGS) $(EXTRA_CFLAGS) $(INC_DIRS)  $(BUILD_ROOT)/$(__SRC) > $(__DEP)

#Autogenerated dependencies. Dependency files are placed in current directory too
DEPFILES_ALL=$(patsubst %.o,%.d,$(OBJS))
-include $(DEPFILES_ALL)

clean:
	rm -Rf *.o *.d $(TARGET) sl1.vh $(BUILD_ROOT)/sl1/include/regdefs.h
	
install:
	cp -f $(TARGET) $(INSTALL_DIR)
