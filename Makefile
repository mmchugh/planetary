build:
	mkdir -p bin
	g++ -o bin/planet -std=c++11 -I`find includes -type d -exec echo {} + | sed 's/ / -I/g'` `find . -name \*.cpp -exec echo {} + -o -name \*.c -exec echo {} +` -lGL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor
	cp shaders/* bin/
