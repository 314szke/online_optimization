# The Ark C/CPP Makefile:tm: 23/07/2020
NAME= COCME

CC= gcc
CFLAGS= -std=c17 -Wall -Wextra -Wfloat-conversion -Wimplicit-int-float-conversion -I./$(SRCDIR)
CXX= clang++
CPPFLAGS= -std=c++17 -Wall -Wextra -Wfloat-conversion -Wimplicit-int-float-conversion -I./$(SRCDIR) -I/opt/gurobi1001/linux64/include/
LINKER=$(CXX)

LDFLAGS = -L/opt/gurobi1001/linux64/lib -lgurobi_c++ -lgurobi100 -lm

SRCDIR= src
TESTDIR= test
OBJDIR= obj
#===== END CONFIG =====#

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC_C = $(call rwildcard,$(SRCDIR),*.c)
OBJ_C = $(SRC_C:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SRC_CPP = $(call rwildcard,$(SRCDIR),*.cpp)
OBJ_CPP = $(SRC_CPP:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJ= $(OBJ_C) $(OBJ_CPP)

SRC_TEST_CPP = $(call rwildcard,$(TESTDIR),*.cpp)
OBJ_TEST_CPP = $(SRC_TEST_CPP:$(TESTDIR)/%.cpp=$(OBJDIR)/test/%.o)
OBJ_TEST= $(filter-out $(OBJDIR)/main.o, $(OBJ)) $(OBJ_TEST_CPP)

DEBUG = yes
ifeq ($(DEBUG), yes)
	CFLAGS += -O0 -g -DDEBUG
	CPPFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O3
	CPPFLAGS += -O3
endif

OPENMP = no
ifeq ($(OPENMP), yes)
	LDFLAGS += -fopenmp
	CFLAGS += -fopenmp
	CPPFLAGS += -fopenmp
endif

# AUTO VARIABLE DEFINITION


build: $(NAME)

test: test_$(NAME)
	./test_$(NAME)

test_$(NAME): $(OBJ_TEST)
	$(LINKER) -o $@ $^ $(LDFLAGS)

$(NAME): $(OBJ)
	$(LINKER) -o $@ $^ $(LDFLAGS)

-include $(OBJ:.o=.d)

# Generic C rule
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $< $(CFLAGS) -c -MMD
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp
	@sed -i '/\\\:/d' $(OBJDIR)/$*.d
# Generic CPP rule
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -o $@ $< $(CPPFLAGS) -c -MMD
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp
	@sed -i '/\\\:/d' $(OBJDIR)/$*.d

$(OBJDIR)/test/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -o $@ $< $(CPPFLAGS) -c -MMD
	@mv -f $(OBJDIR)/test/$*.d $(OBJDIR)/test/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/test/$*.o:|' < $(OBJDIR)/test/$*.d.tmp > $(OBJDIR)/test/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/test/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/test/$*.d
	@rm -f $(OBJDIR)/test/$*.d.tmp
	@sed -i '/\\\:/d' $(OBJDIR)/test/$*.d

.PHONY: clean test
clean:
	rm -f $(NAME) test_$(NAME)
	rm -rf $(OBJDIR)/*
	rm -rf $(OBJDIR)
	rm -f *log
	rm -f tmp.txt
	rm -f result.txt
