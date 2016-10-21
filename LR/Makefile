CXX=clang++
CFLAGS=-O3 --std=c++11

all: LR LR_DFA

LR: LR.cpp
	$(CXX) $(CFLAGS) $< -o $@

LR_DFA: LR_DFA.cpp LR.cpp
	$(CXX) $(CFLAGS) -D_GENERATE_DOT_  $^ -o $@

clean:
	@rm LR LR_DFA

test: LR LR_DFA
	for i in $$(ls Test/*.in); \
	do\
		./LR < $$i > "$$(echo $$i | sed -r 's/(.*)\.in/\1\.out/')";\
		./LR_DFA < $$i | dot -Tpdf -o "$$(echo $$i | sed -r 's/(.*)\.in/\1\.pdf/')";\
	done
