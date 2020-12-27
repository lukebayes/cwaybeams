export

################################################################
# Makefile for C project
################################################################

.PHONY: test

# Set DEBUG=true if 'make debug'
ifneq (,$(findstring debug,$(MAKECMDGOALS)))
DEBUG		:= true
endif

PROJDIR		:= $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
PROJNAME	:= cwaybeam
DIST		:= dist
CEE_FILES	:= src/*.c
CEE_FILES	+= examples/*.c
INCDIR		:= include
TESTDIR		:= test
TESTSRCS	:= $(wildcard test/*.c)
TESTHDRS	:= $(wildcard test/*.h)
TEST_FILES	:=test/*.c

TESTBIN		:= $(DIST)/$(PROJNAME)-test

CFLAGS		:= -Wall
CFLAGS		+= -Werror
ifeq ($(DEBUG), true)
CFLAGS		+= -ggdb
CFLAGS		+= -O0
OUTFILE		:= $(DIST)/$(PROJNAME)-dbg
else
CFLAGS		+= -Werror
CFLAGS		+= -Os
OUTFILE		:= $(DIST)/$(PROJNAME)
endif

VALG_FLAGS	:= --leak-check=full
VALG_FLAGS	+= --show-leak-kinds=all
VALG_FLAGS	+= --tool=memcheck
VALG_FLAGS	+= --track-origins=yes
VALG_FLAGS	+= --leak-resolution=high
VALG_FLAGS	+= --show-mismatched-frees=yes
VALG_FLAGS	+= -s

# Build and run optimized binary
run: $(OUTFILE)
	./$(OUTFILE)

# Create the dist directory
dist:
	mkdir -p ./dist

# Build the debug binary
$(OUTFILE): Makefile dist $(CEE_FILES)
	gcc $(CFLAGS) -o $(OUTFILE) -I$(INCDIR) $(CEE_FILES)
	ls -lah $(OUTFILE)

debug:
	@echo "DEBUG=true"

# Build for debug and run with Valgrind
valgrind: $(OUTFILE)
ifneq (,$(findstring debug,$(MAKECMDGOALS)))
	valgrind $(VALG_FLAGS) $(OUTFILE)
else
	@echo "ERROR: Must run 'make debug valgrind'"
endif

# Build and run the debug binary
gdb: $(OUTFILE)
ifneq (,$(findstring debug,$(MAKECMDGOALS)))
	gdb -ex run $(OUTFILE)
else
	@echo "ERROR: Must run 'make debug gdb'"
endif

# Built and run tests
test: $(CEE_FILES) $(TESTSRCS) $(TESTHDRS)
	gcc -I$(PROJDIR) \
		-I$(TESTDIR) \
		-I$(INCDIR) \
		$(TESTSRCS) \
		"$<" -DTEST_MODE \
		-fsanitize=leak \
		-Wunused \
		-lm \
		-ggdb \
		-o $(TESTBIN)
		@echo $(TESTBIN)
	./$(TESTBIN)

# Remove generated artifacts
clean:
	rm -rf dist

# Generate ctags (for vim tag completion)
ctags:
	ctags -R .
