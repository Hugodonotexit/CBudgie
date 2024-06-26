# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I./depen -I./lexer -I./parser -fsanitize=address -Ofast

# Directories
SRCDIR = .
DEPDIR = depen
LEXERDIR = lexer
PARSERDIR = parser
BUILDDIR = build

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp) \
       $(wildcard $(LEXERDIR)/*.cpp) \
       $(wildcard $(PARSERDIR)/*.cpp)

# Object files
OBJS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(notdir $(SRCS)))

# Target executable
TARGET = interpreter

# Create build directory if it doesn't exist
$(shell mkdir -p $(BUILDDIR))

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(LEXERDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(PARSERDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Phony targets
.PHONY: all clean