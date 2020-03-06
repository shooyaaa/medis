cc=gcc -g
cflags=-I
deps=*.h
obj=medis.o tcp.c net.c

%.o: %.c $(deps)
	$(cc) -c -o $@ $< $(cflags)
medis: $(obj)
	$(cc) -o $@ $^ $(cflags)

build = $(obj) medis
clean:
	rm $(build)