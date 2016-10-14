CXX=clang++
CFLAGS=-O3 --std=c++11
SOURCE = LL1.cpp
DEPS = LL1.h
TARGET=./LL1

$(TARGET): $(SOURCE) $(DEPS)
	$(CXX) $(CFLAGS) $(SOURCE) -o $(TARGET)
	strip $(TARGET)

clean:
	rm $(TARGET)

test: $(TARGET)
	for i in $$(ls Test/*.in); \
	do\
		$(TARGET) < $$i > "$$(echo $$i | sed -r 's/(.*)\.in/\1\.out/')";\
	done
