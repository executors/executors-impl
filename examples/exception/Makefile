EXAMPLES = \
	bulk_twoway_reduce \
	bulk_twoway_reduce_vector

CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -I../../include

.PHONY: all clean

all: $(EXAMPLES)

clean:
	rm -f $(EXAMPLES)

$(EXAMPLES): %: %.cpp
	$(CXX) $(CXXFLAGS) -o$@ $<
