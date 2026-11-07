CXX ?= c++
CPPFLAGS ?= -Isrc
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
LDLIBS = -lbe -ltranslation
ICON2ICON ?= icon2icon
RC ?= rc
XRES ?= xres

BUILD_DIR = build
TARGET = $(BUILD_DIR)/hdialog
TEST_TARGET = $(BUILD_DIR)/dialog_options_test
ICON_SOURCE = assets/hdialog.svg
ICON_HVIF = $(BUILD_DIR)/hdialog.hvif
RESOURCE_SOURCE = resources/hdialog.rdef
RESOURCE_FILE = $(BUILD_DIR)/hdialog.rsrc
SOURCES = src/DialogApplication.cpp src/DialogOptions.cpp \
	src/DialogWindow.cpp src/IconView.cpp src/JsonOutput.cpp src/main.cpp
OBJECTS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TEST_OBJECT = $(BUILD_DIR)/DialogOptionsTest.o
DEPENDENCIES = $(OBJECTS:.o=.d) $(TEST_OBJECT:.o=.d)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS) $(RESOURCE_FILE) $(ICON_HVIF)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDLIBS) -o $@
	$(XRES) -o $@ $(RESOURCE_FILE) -a VICN:101:BEOS\:ICON $(ICON_HVIF)

$(ICON_HVIF): $(ICON_SOURCE) | $(BUILD_DIR)
	@command -v $(ICON2ICON) >/dev/null 2>&1 || { \
		echo "error: icon2icon is required (pkgman install hvif_tools)" >&2; \
		exit 1; \
	}
	$(ICON2ICON) $< $@

$(RESOURCE_FILE): $(RESOURCE_SOURCE) | $(BUILD_DIR)
	$(RC) -o $@ $<

$(TEST_TARGET): $(TEST_OBJECT) $(BUILD_DIR)/DialogOptions.o \
	$(BUILD_DIR)/JsonOutput.o
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
	rm -f $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECT) $(DEPENDENCIES) \
		$(ICON_HVIF) $(RESOURCE_FILE)

-include $(DEPENDENCIES)
