LIBS=-lglog -lpthread
INCLUDES=-I.

OBJECT=server client

all: $(OBJECT)

SRC=$(shell find . -maxdepth 1 -name "*.cpp")
server: test/server.cpp $(SRC)
	g++ $^ -o $@ $(LIBS) $(INCLUDES)

client: test/client.cpp $(SRC)
	g++ $^ -o $@ $(LIBS) $(INCLUDES)

clean :
	rm -f $(OBJECT)

rebuild : clean all
