DIR		:= ./debug
EXE		:= $(DIR)/$(shell basename `pwd`)
CC		:= g++ 
LIBDIR	:= 
LIBS	:= pthread boost_system boost_filesystem
INC		:=
SRCS	:= $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cc)
OCPP	:= $(patsubst %.cpp, $(DIR)/%.o, $(wildcard *.cpp))
OC		:= $(patsubst %.c, $(DIR)/%.co, $(wildcard *.c))
OCC		:= $(patsubst %.cc, $(DIR)/%.cco, $(wildcard *.cc))
OBJS	:= $(OC) $(OCC) $(OCPP)
RM		:= rm -rf
CXXFLAGS	:= -std=c++11 -Wall -g
start	: mkdebug $(EXE) 
mkdebug :
	@if [ ! -d $(DIR) ]; then mkdir $(DIR); fi;
$(EXE) 	: $(OBJS)
	$(CC) -o $@ $^ $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
$(DIR)/%.o : %.cpp
	$(CC) $(CXXFLAGS) -c $^ $(addprefix -L,$(LIBDIR)) $(addprefix -I,$(INC)) -o $@
$(DIR)/%.co : %.c
	$(CC) $(CXXFLAGS) -c $^ $(addprefix -L,$(LIBDIR)) $(addprefix -I,$(INC)) -o $@
$(DIR)/%.cco : %.cc
	$(CC) $(CXXFLAGS) -c $^ $(addprefix -L,$(LIBDIR)) $(addprefix -I,$(INC)) -o $@
.PHONY 	: clean rmdebug rebuild 
clean	:
	@$(RM) $(EXE) $(DIR)/*.o $(DIR)/*.co $(DIR)/*.cco
rmdebug	:
	@$(RM) $(DIR)
rebuild: clean start