# Makefile - DealerDesk build automation for Linux, macOS and WSL
# Compiler and flags
CC ?= gcc
CFLAGS ?= -Wall -Wextra -pedantic -std=c99
LIBS = -lm

SRC_APP = main.c calc.c dbfile.c screen.c
BIN_APP = dealerdesk

TEST_CALC_SRC = tests/test_calc.c calc.c
TEST_CALC_BIN = test_calc

TEST_DBFILE_SRC = tests/test_dbfile.c dbfile.c
TEST_DBFILE_BIN = test_dbfile

.PHONY: all test clean

all: $(BIN_APP)

$(BIN_APP): $(SRC_APP)
	$(CC) $(CFLAGS) -o $(BIN_APP) $(SRC_APP) $(LIBS)

test: $(TEST_CALC_BIN) $(TEST_DBFILE_BIN)
	@echo "===> Running Pricing Engine Tests..."
	./$(TEST_CALC_BIN)
	@echo ""
	@echo "===> Running Persistence Tests..."
	./$(TEST_DBFILE_BIN)

$(TEST_CALC_BIN): $(TEST_CALC_SRC)
	$(CC) $(CFLAGS) -o $(TEST_CALC_BIN) $(TEST_CALC_SRC) $(LIBS)

$(TEST_DBFILE_BIN): $(TEST_DBFILE_SRC)
	$(CC) $(CFLAGS) -o $(TEST_DBFILE_BIN) $(TEST_DBFILE_SRC)

clean:
	rm -f $(BIN_APP) $(TEST_CALC_BIN) $(TEST_DBFILE_BIN) *.o
