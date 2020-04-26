CC=g++
CPPFLAGS=
LDFLAGS=-lSDL2 -lSDL2_image

CPPSRC = $(wildcard src/main.cpp) 		\
		 $(wildcard src/oxuUtils/*.cpp) \
		 $(wildcard src/oxuCore/*.cpp) 	\
		 $(wildcard src/oxuGameHandlers/graphicsHandler.cpp) \
		 $(wildcard src/oxuGameComponents/cursor.cpp) \

OBJ = $(CPPSRC:.cpp=.o)

release: $(OBJ)
	$(CC) $^ -o oXu $(LDFLAGS)
 
%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
