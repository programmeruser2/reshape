modules := 
module_libraries := $(addsuffix .so,$(modules))

CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion -Wunused-parameter -fsanitize=address,undefined

.PHONY: all clean
all: bin/reshape $(module_libraries)
bin/reshape: src/app/*.c
	$(CC) -o bin/reshape src/app/*.c
%.so: src/modules/$(basename $@)/*.c 
	$(CC) -c -fPIC -o $(basename $@).o src/modules/$(basename $@)/*.c
	$(CC) -shared -o $@ bin/modules/$(basename $@).so
clean:
	rm *.o

