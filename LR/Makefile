CXX=clang++
CFLAGS=-O3 --std=c++11
SOURCE = LR.cpp
# DEPS = LR.h
TARGET=./LR

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
