.PHONY: all clean version.h

OS = lin

BINARY_FOLDER = bin

ifeq ($(OS),win)
CC := i686-w64-mingw32-gcc
TARGET := $(BINARY_FOLDER)/steammontage.exe
else
CC := gcc -m32
TARGET := $(BINARY_FOLDER)/steammontage
endif

LIBS = -lssl -lcrypto -pthread -lnghttp2 -lssh2 -lz -lcurl -lzstd -ljpeg
CFLAGS = -Wall
SOURCES := $(shell find . -name "*.c")
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

VERSION := "$(shell git describe --abbrev=4 --dirty --always --tags)"
VERSIONFILE := src/version.h

all: $(VERSIONFILE) $(OBJECTS)
	mkdir -p $(BINARY_FOLDER)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIBS)

$(VERSIONFILE): .git/COMMIT_EDITMSG .git/HEAD
	@echo "#ifndef SM_VERSION_H" > $(VERSIONFILE)
	@echo "#define SM_VERSION_H" >> $(VERSIONFILE)
	@echo "#define SM_VERSION \"$(VERSION)\"" >> $(VERSIONFILE)
	@echo "#endif" >> $(VERSIONFILE)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(BINARY_FOLDER)/*

help:
	@echo
	@echo " make [target] [OPTIONS]"
	@echo
	@echo " Targets:"
	@echo "		all 		Builds steammontage."
	@echo "		clean 		Clean up object files and binaries. Run if"
	@echo "					you are receiving errors."
	@echo "		help		Display this help text."
	@echo " Options:"
	@echo "		OS=lin		Creates a Linux build (default)."
	@echo "		OS=win		Creates a Windows build."
	@echo "					"
	@echo
