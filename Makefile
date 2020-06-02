cc=gcc -g
cflags=
deps=*.h
obj=medis.o tcp.c net.c schedule.c resp.c dm_string.c hash.c

%.o: %.c $(deps)
	$(cc) -c -o $@ $< $(cflags)
medis: $(obj)
	$(cc) -o $@ $^ $(cflags)

build = $(obj) medis
