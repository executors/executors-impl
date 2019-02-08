EXAMPLES = \
	for_each_n_1 \
	for_each_n_2 \
	for_each_n_3 \
	for_each_n_4 \
	for_each_n_5

CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -I../../include

.PHONY: all clean

all: $(EXAMPLES)

clean:
	rm -f $(EXAMPLES)

$(EXAMPLES): %: %.cpp
	$(CXX) $(CXXFLAGS) -o$@ $<
