#---------
#
# CppUTest Examples Makefile
#
#----------

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

CPPUTEST_CXXFLAGS += -Wno-old-style-cast

#--- Inputs ----#
COMPONENT_NAME = RingBufferT
CPPUTEST_HOME = extras/Testing/cpputest

CPPUTEST_USE_GCOV = Y

CPPUTEST_USE_EXTENSIONS = Y
CPP_PLATFORM = Gcc

CPPUTEST_WARNINGFLAGS += -Wno-unknown-warning-option
CPPUTEST_CXX_WARNINGFLAGS += -Wno-reserved-id-macro -Wno-keyword-macro

# This line is overriding the default new macros.  This is helpful
# when using std library includes like <list> and other containers
# so that memory leak detection does not conflict with stl.
#CPPUTEST_MEMLEAK_DETECTOR_NEW_MACRO_FILE = -include ApplicationLib/ExamplesNewOverrides.h
SRC_DIRS = \
	    src \

SRC_FILES = \
	    
TEST_SRC_DIRS = \
	extras/Testing/ \
	extras/Testing/tests \

MOCKS_SRC_DIRS = \

INCLUDE_DIRS =\
	      src/ \

include $(CPPUTEST_HOME)/build/MakefileWorker.mk


