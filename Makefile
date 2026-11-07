CXX ?= c++
CPPFLAGS ?= -Isrc
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
LDLIBS = -lbe -ltranslation

BUILD_DIR = build
TARGET = $(BUILD_DIR)/hdialog
TEST_TARGET = $(BUILD_DIR)/dialog_options_test
SOURCES = src/DialogApplication.cpp src/DialogOptions.cpp \
	src/DialogWindow.cpp src/IconView.cpp src/main.cpp
OBJECTS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TEST_OBJECT = $(BUILD_DIR)/DialogOptionsTest.o
DEPENDENCIES = $(OBJECTS:.o=.d) $(TEST_OBJECT:.o=.d)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(TEST_TARGET): $(TEST_OBJECT) $(BUILD_DIR)/DialogOptions.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(TEST_OBJECT): tests/DialogOptionsTest.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECT) $(DEPENDENCIES)

-include $(DEPENDENCIES)
