INCLUDES += -Iinclude

CFLAGS += $(INCLUDES)

CFLAGS += $(shell pkg-config --cflags portaudio-2.0)
LDFLAGS += $(shell pkg-config --libs portaudio-2.0)

CXXFLAGS += $(CFLAGS)

STATIC_LIB := oplfm.a
STATIC_OBJ := src/oplfm.o src/instrument.o src/playback.o src/stream.o src/woody/woodyopl.o src/woody/woody.o

all: $(STATIC_LIB)

$(STATIC_LIB): $(STATIC_OBJ)
	$(AR) rcs $@ $^

clean:
	$(RM) $(STATIC_OBJ)

distclean: clean
	$(RM) $(STATIC_LIB)
