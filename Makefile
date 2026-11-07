CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
HAIKU_LIBS = -lbe

BUILD_DIR = build
TARGET = $(BUILD_DIR)/hdialog
TEST_TARGET = $(BUILD_DIR)/dialog_options_test
SOURCES = src/DialogApplication.cpp src/DialogOptions.cpp \
	src/DialogWindow.cpp src/IconView.cpp src/main.cpp

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Isrc $(SOURCES) $(HAIKU_LIBS) -ltranslation -o $@

$(TEST_TARGET): tests/DialogOptionsTest.cpp src/DialogOptions.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Isrc tests/DialogOptionsTest.cpp src/DialogOptions.cpp -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
