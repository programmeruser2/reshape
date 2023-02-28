modules := sum double
module_libraries := $(addprefix bin/modules/,$(addsuffix .so,$(modules)))

CC ?= gcc
CFLAGS := -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion -Wunused-parameter
LDFLAGS := -ldl

# TODO: debug target because it's probably not a good idea to have address sanitizer on in production/release

.PHONY: all clean
all: bin/reshape $(module_libraries)

debug: CFLAGS += -fsanitize=address,undefined -g
debug: all

bin/reshape: src/app/*.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/reshape src/app/*.c
bin/modules/%.so: src/modules/%/*.c 
	$(CC) $(CFLAGS) -c -fPIC -o $(basename $@).o $^
	$(CC) -shared -o $@ $(basename $@).o
clean:
	rm *.o

