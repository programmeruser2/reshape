.PHONY: all clean
modules := 
module_libraries := $(addsuffix .so,$(modules))
all: bin/reshape $(module_libraries)
bin/reshape: src/app/*.c
	gcc -o bin/reshape src/app/*.c
%.so: src/modules/$(basename $@)/*.c 
	gcc -c -fPIC -o $(basename $@).o src/modules/$(basename $@)/*.c
	gcc -shared -o $@ bin/modules/$(basename $@).so
clean:
	rm *.o

