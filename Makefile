LIBS=-lglog
INCLUDES=-I.

OBJECT=test

all: $(OBJECT)

test: epoll_socket.cpp test.cpp
	g++ $^ -o $@ $(LIBS) $(INCLUDES)

clean :
	rm -f $(OBJECT)

rebuild : clean all
