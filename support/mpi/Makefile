
#
#  Assume you are on olympus.pnl.gov
#
BIN_DIR     = ../../bin
MAPIGCC     = /share/apps/mvapich2/1.7/gcc/4.6.2/bin


ifneq ($(METAGENOMICS_HOME), "")
  BIN_DIR   = $(METAGENOMICS_HOME)/bin
endif

BIN_DIR     = .

ifeq ($(shell hostname), bedstone.pnl.gov)
  MAPIGCC   = /share/apps/mvapich2/1.8/gcc/4.6.2/bin
endif


CXX         = $(MAPIGCC)/mpic++
LPP         = $(MAPIGCC)/mpic++

CFLAGS      = -Wall -g
LDFLAGS     = -O2   -g

CXX_SOURCES = $(wildcard *.cpp)
CXX_OBJECTS = $(CXX_SOURCES:%.cpp=%.o)
CXX_EXES    = $(CXX_OBJECTS:%.o=%)
OBJ_DIR     = obj
DIRS        = $(BIN_DIR) $(OBJ_DIR)


all: $(DIRS) $(CXX_EXES)


$(DIRS):
	mkdir -p $@


$(CXX_EXES): $(CXX_OBJECTS)
	${LPP} ${LDFLAGS} -o $(BIN_DIR)/$@ $(OBJ_DIR)/$<


%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@
	

.PHONY : clean


clean:
	$(foreach file, $(CXX_EXES), rm -f $(BIN_DIR)/$(file);)
	rm -f $(OBJ_DIR)/*
