mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS += 

APPL_CXXOBJS += \
	BatteryMonitor.o \
	TouchStarter.o \
	LineMonitor.o \
	LineTracer.o \
	TraceController.o \
	Volante.o \
	Driver.o \
	Navigator.o \
	Map.o \
	PosEstimator.o \
	LCD.o \
	Timer.o \
	Logger.o \
	StopWatch.o \
	ColorIdentify.o \
	Motion.o \
	MotionSequencer.o \
	ScenarioCreator.o \
	MotionTranslator.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile

endif

APPL_DIR += \
	$(mkfile_path)util \
   	$(mkfile_path)unit \
   	$(mkfile_path)unit/navi \
   	$(mkfile_path)unit/motion \
   	$(mkfile_path)unit/scenario \
   	$(mkfile_path)unit/trace \
   	$(mkfile_path)unit/etc

INCLUDES += \
	-I$(mkfile_path)util \
	-I$(mkfile_path)unit \
	-I$(mkfile_path)unit/navi \
	-I$(mkfile_path)unit/motion \
	-I$(mkfile_path)unit/scenario \
	-I$(mkfile_path)unit/trace \
	-I$(mkfile_path)unit/etc

#COPTS += -fno-use-cxa-atexit
COPTS +=
