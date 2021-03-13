CC = gcc
CFLAGS = -I.
LDFLAGS = -lrt
OBJS_P = processes.o
OBJS_R = reader.o
OBJS_W = writer.o
DEPS = data.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: processes reader writer

processes: $(OBJS_P)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

reader: $(OBJS_R)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

writer: $(OBJS_W)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# clean objects - $(RM) is rm -f by default
clean:
	$(RM) $(OBJS_P) $(OBJS_R) $(OBJS_W) processes reader writer
