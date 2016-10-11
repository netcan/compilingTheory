CXX=clang++
CFLAGS=-O3 --std=c++11
SOURCE = LL1.cpp
DEPS = LL1.h
TARGET=./LL1
TESTIN=LL1.data.in LL1.data2.in

$(TARGET): $(SOURCE) $(DEPS)
	$(CXX) $(CFLAGS) $(SOURCE) -o $(TARGET)
	strip $(TARGET)

clean:
	rm $(TARGET)

test: $(TARGET) $(TESTIN)
	for i in $(TESTIN); \
	do\
		$(TARGET) < $$i > "$$(echo $$i | sed -r 's/(.*)\.in/\1\.out/')";\
	done
