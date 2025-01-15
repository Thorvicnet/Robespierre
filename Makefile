#########################
CFILES := main.c board.c move.c history.c bb.c threat.c uci.c bot.c
PROG := main
DEBUG_PROG := main
CFLAGS := -Wall -Wextra -pedantic -O3#-DMENACE
LDFLAGS := -lm -O3

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
DEBUG_DIR := debug
TEST_FILE := tests.c
PERFT_FILE := perft.c
########################

# -MMD generates dependencies while compiling, add pg for profiling
CFLAGS += -MMD
CC := clang

# Add paths after defining the variables
OBJFILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(CFILES))
DEPFILES := $(OBJFILES:.o=.d)
SRCS := $(patsubst %.c, $(SRC_DIR)/%.c, $(CFILES))

all: $(BIN_DIR)/$(PROG)

debug: CFLAGS += -DDEBUG -g -pg -fsanitize=address
debug: LDFLAGS += -g -pg -fsanitize=address
debug: $(DEBUG_DIR)/$(DEBUG_PROG)

debug-perft: CFLAGS += -DDEBUG -g -pg -fsanitize=address
debug-perft: LDFLAGS += -g -pg -fsanitize=address
debug-perft: $(SRC_DIR)/$(PERFT_FILE) $(filter-out $(OBJ_DIR)/main.o, $(OBJFILES))
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(LDFLAGS) -o $(DEBUG_DIR)/perft $^

debug-tests: CFLAGS += -DDEBUG -g -pg -fsanitize=address
debug-tests: LDFLAGS += -g -pg -fsanitize=address
debug-tests: $(SRC_DIR)/$(TEST_FILE) $(filter-out $(OBJ_DIR)/main.o, $(OBJFILES))
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(LDFLAGS) -o $(DEBUG_DIR)/tests $^

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

tests: $(SRC_DIR)/$(TEST_FILE) $(filter-out $(OBJ_DIR)/main.o, $(OBJFILES))
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/tests $^

perft: $(SRC_DIR)/$(PERFT_FILE) $(filter-out $(OBJ_DIR)/main.o, $(OBJFILES))
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/perft $^

