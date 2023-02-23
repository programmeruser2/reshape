modules := 
module_libraries := $(addsuffix .so,$(modules))

CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion -Wunused-parameter -fsanitize=address,undefined
LDFLAGS := -ldl

# TODO: debug target because it's probably not a good idea to have address sanitizer on in production/release (or is it?)

.PHONY: all clean debug
all: bin/reshape $(module_libraries)

bin/reshape: src/app/*.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/reshape src/app/*.c
%.so: src/modules/$(basename $@)/*.c 
	$(CC) $(CFLAGS) -c -fPIC -o $(basename $@).o src/modules/$(basename $@)/*.c
	$(CC) -shared -o $@ bin/modules/$(basename $@).so
clean:
	rm *.o

