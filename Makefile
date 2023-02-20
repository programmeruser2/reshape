.PHONY: all
modules := 
module_libraries := $(addsuffix .so,$(modules))
all: bin/reshape $(module_libraries)
bin/reshape: 
	gcc -o bin/reshape src/app/*.c
%.so: 
	gcc -c -fPIC -o $(basename $@).o src/modules/*.c
	gcc -shared -o $@ bin/modules/$(basename $@).so

