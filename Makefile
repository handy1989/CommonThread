LIBS=-lglog -lpthread
INCLUDES=-I.

OBJECT=test

all: $(OBJECT)

SRC=$(shell find . -name "*.cpp")
test: $(SRC)
	g++ $^ -o $@ $(LIBS) $(INCLUDES)

clean :
	rm -f $(OBJECT)

rebuild : clean all
