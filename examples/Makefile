.PHONY : clean all

BINDIR := bin
OBJDIR := obj
SRCDIR := src
INCDIR := ../include
DEPOBJDIR := ../obj
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
ALL := $(OBJECTS:$(OBJDIR)/%.o=$(BINDIR)/%)

CXXFLAGS := --std=c++11 -O3 -D NDEBUG -I$(INCDIR) -Wall -lpthread

all : $(ALL)

$(BINDIR) :
	mkdir $@

$(OBJDIR) :
	mkdir $@

$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

$(BINDIR)/%Example : $(OBJDIR)/%Example.o $(DEPOBJDIR)/%.o | $(BINDIR)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean :
	$(RM) $(OBJECTS) $(ALL)
