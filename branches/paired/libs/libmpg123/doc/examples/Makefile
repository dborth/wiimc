targets = feedseek mpg123_to_wav scan id3dump mpglib dump_seekindex

all: $(targets)

# On largefile-aware systems you might want to use these instead:
#MPG123_CFLAGS  := $(shell pkg-config --cflags libmpg123_64)
#MPG123_LDFLAGS := $(shell pkg-config --libs   libmpg123_64)
# This works on sane setups where off_t is off_t, and just that.
MPG123_CFLAGS  := $(shell pkg-config --cflags libmpg123)
MPG123_LDFLAGS := $(shell pkg-config --libs   libmpg123)
SND_CFLAGS     := $(shell pkg-config --cflags sndfile)
SND_LDFLAGS    := $(shell pkg-config --libs   sndfile)

# Oder of libs not that important here...
compile = $(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(MPG123_CFLAGS) $(MPG123_LDFLAGS)

mpg123_to_wav: mpg123_to_wav.c
	$(compile) $(SND_CFLAGS) $(SND_LDFLAGS) -o mpg123_to_wav mpg123_to_wav.c

feedseek: feedseek.c
	$(compile) -o feedseek feedseek.c

scan: scan.c
	$(compile) -o scan scan.c

id3dump:id3dump.c
	$(compile) -o id3dump id3dump.c

dump_seekindex: dump_seekindex.c
	$(compile) -o dump_seekindex dump_seekindex.c

mpglib: mpglib.c
	$(compile) -o mpglib mpglib.c

clean:
	rm -vf $(targets)
