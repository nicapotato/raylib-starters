TARGET = raylib-quickstart-lua
SRC = src/host.c
CC = gcc
CFLAGS = -Wall -std=c99 -Isrc
LDFLAGS = 

# Check for pkg-config
PKG_CONFIG ?= pkg-config

# Raylib detection
ifeq ($(shell $(PKG_CONFIG) --exists raylib && echo 0),0)
	CFLAGS += $(shell $(PKG_CONFIG) --cflags raylib)
	LDFLAGS += $(shell $(PKG_CONFIG) --libs raylib)
else
	# Fallback for macOS if installed via brew but no pkg-config or custom install
	CFLAGS += -I/opt/homebrew/include -I/usr/local/include
	LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib -lraylib
	
	# macOS frameworks required by raylib static lib
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	endif
endif

# Lua detection (lua, lua5.4, lua5.3, luajit)
LUA_PKGS := lua5.4 lua5.3 lua luajit
FOUND_LUA = $(shell for pkg in $(LUA_PKGS); do $(PKG_CONFIG) --exists $$pkg && echo $$pkg && break; done)

ifneq ($(FOUND_LUA),)
	CFLAGS += $(shell $(PKG_CONFIG) --cflags $(FOUND_LUA))
	LDFLAGS += $(shell $(PKG_CONFIG) --libs $(FOUND_LUA))
else
	# Fallback: try -llua and hope headers are standard
	LDFLAGS += -llua
endif

.PHONY: all clean run setup_resources

all: setup_resources $(TARGET)

setup_resources:
	@if [ ! -d "resources" ]; then \
		echo "Copying resources from quickstart-c..."; \
		cp -r ../quickstart-c/resources .; \
	fi

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

