CXX=g++
CFLAGS=-O3 --std=c++11
SOURCE = lexical.cpp
DEPS = lexical.h
TARGET=lexical

$(TARGET): $(SOURCE) $(DEPS)
	$(CXX) $(CFLAGS) $(SOURCE) -o $(TARGET)
	strip $(TARGET)

clean:
	rm $(TARGET)
