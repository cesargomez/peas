WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -O3 -fexceptions 
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/PEAS

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/PEAS

OBJ_DEBUG = $(OBJDIR_DEBUG)/workpool.o $(OBJDIR_DEBUG)/work.o $(OBJDIR_DEBUG)/server.o $(OBJDIR_DEBUG)/scheduler.o $(OBJDIR_DEBUG)/queue.o $(OBJDIR_DEBUG)/nsga-ii.o $(OBJDIR_DEBUG)/ABC.o $(OBJDIR_DEBUG)/moheft.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/csv_parser.o $(OBJDIR_DEBUG)/core.o $(OBJDIR_DEBUG)/chameleon.o $(OBJDIR_DEBUG)/ConfigFile.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/workpool.o $(OBJDIR_RELEASE)/work.o $(OBJDIR_RELEASE)/server.o $(OBJDIR_RELEASE)/scheduler.o $(OBJDIR_RELEASE)/queue.o $(OBJDIR_RELEASE)/nsga-ii.o $(OBJDIR_RELEASE)/ABC.o $(OBJDIR_RELEASE)/moheft.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/csv_parser.o $(OBJDIR_RELEASE)/core.o $(OBJDIR_RELEASE)/chameleon.o $(OBJDIR_RELEASE)/ConfigFile.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/workpool.o: workpool.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c workpool.cpp -o $(OBJDIR_DEBUG)/workpool.o

$(OBJDIR_DEBUG)/work.o: work.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c work.cpp -o $(OBJDIR_DEBUG)/work.o

$(OBJDIR_DEBUG)/server.o: server.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c server.cpp -o $(OBJDIR_DEBUG)/server.o

$(OBJDIR_DEBUG)/scheduler.o: scheduler.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c scheduler.cpp -o $(OBJDIR_DEBUG)/scheduler.o

$(OBJDIR_DEBUG)/queue.o: queue.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c queue.cpp -o $(OBJDIR_DEBUG)/queue.o

$(OBJDIR_DEBUG)/nsga-ii.o: nsga-ii.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c nsga-ii.cpp -o $(OBJDIR_DEBUG)/nsga-ii.o

$(OBJDIR_DEBUG)/ABC.o: ABC.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ABC.cpp -o $(OBJDIR_DEBUG)/ABC.o

$(OBJDIR_DEBUG)/moheft.o: moheft.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c moheft.cpp -o $(OBJDIR_DEBUG)/moheft.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/csv_parser.o: csv_parser.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c csv_parser.cpp -o $(OBJDIR_DEBUG)/csv_parser.o

$(OBJDIR_DEBUG)/core.o: core.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c core.cpp -o $(OBJDIR_DEBUG)/core.o

$(OBJDIR_DEBUG)/chameleon.o: chameleon.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c chameleon.cpp -o $(OBJDIR_DEBUG)/chameleon.o

$(OBJDIR_DEBUG)/ConfigFile.o: ConfigFile.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ConfigFile.cpp -o $(OBJDIR_DEBUG)/ConfigFile.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/workpool.o: workpool.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c workpool.cpp -o $(OBJDIR_RELEASE)/workpool.o

$(OBJDIR_RELEASE)/work.o: work.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c work.cpp -o $(OBJDIR_RELEASE)/work.o

$(OBJDIR_RELEASE)/server.o: server.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c server.cpp -o $(OBJDIR_RELEASE)/server.o

$(OBJDIR_RELEASE)/scheduler.o: scheduler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c scheduler.cpp -o $(OBJDIR_RELEASE)/scheduler.o

$(OBJDIR_RELEASE)/queue.o: queue.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c queue.cpp -o $(OBJDIR_RELEASE)/queue.o

$(OBJDIR_RELEASE)/nsga-ii.o: nsga-ii.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c nsga-ii.cpp -o $(OBJDIR_RELEASE)/nsga-ii.o

$(OBJDIR_RELEASE)/ABC.o: ABC.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ABC.cpp -o $(OBJDIR_RELEASE)/ABC.o

$(OBJDIR_RELEASE)/moheft.o: moheft.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c moheft.cpp -o $(OBJDIR_RELEASE)/moheft.o

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/csv_parser.o: csv_parser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c csv_parser.cpp -o $(OBJDIR_RELEASE)/csv_parser.o

$(OBJDIR_RELEASE)/core.o: core.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c core.cpp -o $(OBJDIR_RELEASE)/core.o

$(OBJDIR_RELEASE)/chameleon.o: chameleon.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c chameleon.cpp -o $(OBJDIR_RELEASE)/chameleon.o

$(OBJDIR_RELEASE)/ConfigFile.o: ConfigFile.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ConfigFile.cpp -o $(OBJDIR_RELEASE)/ConfigFile.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

