#Makefile for fsim_sl1

#Common definition
TARGET = libfsim_sl2.a
BUILD_ROOT=../..
VPATH=../..

-include $(BUILD_ROOT)/Makefile.gsetup

#Append include directories
INC_DIRS += \
-I$(BUILD_ROOT)/include \
-I$(BUILD_ROOT)/simkernel/include \
-I$(BUILD_ROOT)/com/include \
-I$(BUILD_ROOT)/sl2/include \
-I$(BUILD_ROOT)/shell/include \
-I$(BUILD_ROOT)/utils/include 

#Append compilation flags
CFLAGS += -DIs_True_On=1

#Append extra compilation flags
EXTRA_CFLAGS += \
  -D_TIME="'$(TIME)'" -D_SL2BUILD=1

#Append linker flags
LD_FLAGS += -l$(TCL_LIB) -ltermcap

#Append library searching path
LIB_DIRS += -L$(BUILD_ROOT)/lib

#Source files
#Source files shared by all architecture
KERNEL_SOURCE = \
simkernel/memory.cpp 

SL2_SOURCE = \
sl2/sl2decoder.cpp \
sl2/sl2disasm.cpp \
sl2/sl2exec.cpp \
sl2/sl2instr.cpp \
sl2/sl2instrtable.cpp \
sl2/sl2mmu.cpp \
sl2/sl2reg.cpp \


ALL_SOURCE := $(KERNEL_SOURCE) $(SL2_SOURCE) 
#ALL_SOURCE := $(SHELL_SOURCE) $(UTILS_SOURCE)

#object files. Object files are placed in current directory
LIBOBJS=$(patsubst %.cpp, %.o, $(notdir $(ALL_SOURCE)))

#Target
all: $(TARGET)
	 
$(TARGET): $(LIBOBJS)
	$(AR) cru $@ $^

$(LIBOBJS): __SRC = $(filter %/$(patsubst %.o,%.cpp,$@),$(ALL_SOURCE))
$(LIBOBJS): __DEP =$(patsubst %.o,%.d, $@)
$(LIBOBJS):
	echo INC_DIRS:$(INC_DIRS)
	@echo Building $@ from $(__SRC) by $(__DEP)
	$(CPP)  $(CFLAGS) $(EXTRA_CFLAGS) $(INC_DIRS) $(BUILD_ROOT)/$(__SRC)
	@echo Creating dependency file $(__DEP) for $@
	$(GEN_DEP)  $(CFLAGS) $(EXTRA_CFLAGS) $(INC_DIRS)  $(BUILD_ROOT)/$(__SRC) > $(__DEP)

#Autogenerated dependencies. Dependency files are placed in current directory too
DEPFILES_ALL=$(patsubst %.o,%.d,$(LIBOBJS))
-include $(DEPFILES_ALL)

clean:
	rm -Rf *.o *.d $(TARGET)
	
install:
	cp -f $(TARGET) $(INSTALL_DIR)
