CC = gcc
CFLAGS = -Wall -Wextra -g \
         -I./Network \
         -I./Simulation \
         -I./Client/Graphics_Libraries/glfw-3.4/include \
         -I./Client/Graphics_Libraries/glad/include

LDFLAGS = -lpthread -lm -ldl

all: server client

# Static libraries
libnetwork.a: Network/network.c Network/network.h
	$(CC) $(CFLAGS) -c Network/network.c -o Network/network.o
	ar rcs libnetwork.a Network/network.o

libsimulation.a: Simulation/maps.c Simulation/maps.h Simulation/labyrinth.c Simulation/labyrinth.h
	$(CC) $(CFLAGS) -c Simulation/maps.c -o Simulation/maps.o
	$(CC) $(CFLAGS) -c Simulation/labyrinth.c -o Simulation/labyrinth.o
	ar rcs libsimulation.a Simulation/maps.o Simulation/labyrinth.o

libglfw.a:
	cmake -S Client/Graphics_Libraries/glfw-3.4 -B Client/Graphics_Libraries/glfw-3.4/build -DBUILD_SHARED_LIBS=OFF
	cmake --build Client/Graphics_Libraries/glfw-3.4/build
	cp Client/Graphics_Libraries/glfw-3.4/build/src/libglfw3.a libglfw.a

glad.o:
	$(CC) $(CFLAGS) -c Client/Graphics_Libraries/glad/src/glad.c -o glad.o

# Targets
server: Server/server.c libnetwork.a libsimulation.a
	$(CC) $(CFLAGS) -o server Server/server.c -L. -lnetwork -lsimulation $(LDFLAGS)
	
client: Client/client.c Client/graphics.c libnetwork.a libsimulation.a libglfw.a glad.o
	$(CC) $(CFLAGS) -o client Client/client.c Client/graphics.c glad.o -L. -lnetwork -lsimulation -lglfw $(LDFLAGS)
clean:
	rm -f server client libnetwork.a libsimulation.a libglfw.a glad.o
	rm -f Network/network.o Simulation/maps.o
	rm -rf Client/Graphics_Libraries/glfw-3.4/build