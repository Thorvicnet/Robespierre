#########################
CFILES := main.c
PROG := main
DEBUG_PROG := main
CFLAGS := -Wall -Wextra -pedantic
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
DEBUG_DIR := debug
########################

# -MMD generates dependencies while compiling
CFLAGS += -MMD
CC := clang

# Add paths after defining the variables
OBJFILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(CFILES))
DEPFILES := $(OBJFILES:.o=.d)
SRCS := $(patsubst %.c, $(SRC_DIR)/%.c, $(CFILES))

all: $(BIN_DIR)/$(PROG)

debug: CFLAGS += -DDEBUG -g -fsanitize=address
debug: $(DEBUG_DIR)/$(DEBUG_PROG)

$(BIN_DIR)/$(PROG): $(OBJFILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(DEBUG_DIR)/$(DEBUG_PROG): $(OBJFILES)
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(DEBUG_DIR) $(OBJ_DIR) *.d

-include $(DEPFILES)
