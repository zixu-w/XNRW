.PHONY : clean all examples ThreadPool exampleClean

OBJDIR := obj
SRCDIR := src
INCDIR := include
EXAMPLEDIR := examples
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CXXFLAGS := --std=c++11 -O3 -D NDEBUG -I$(INCDIR) -Wall -lpthread

all : $(OBJECTS)
examples : all
	$(MAKE) -C $(EXAMPLEDIR) -j8

ThreadPool : $(OBJDIR)/ThreadPool.o

$(OBJDIR) :
	mkdir $@

$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

clean : exampleClean
	$(RM) $(OBJECTS)

exampleClean :
	$(MAKE) clean -C $(EXAMPLEDIR)
