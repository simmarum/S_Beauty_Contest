# find cpp files in subdirectories
SOURCES := $(shell find . -name '*.cpp')

# find headers
HEADERS := $(shell find . -name '*.h')

OUTPUT := contest

# Everything depends on the output
all: clean $(OUTPUT)

# The output depends on sources and headers
$(OUTPUT): $(SOURCES) $(HEADERS)
	mpic++ -pthread -o $(OUTPUT) $(SOURCES)

clean:
	$(RM) $(OUTPUT)