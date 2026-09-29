CXX := $(shell command -v g++-16 2>/dev/null || command -v g++-15 2>/dev/null || command -v g++-14 2>/dev/null || command -v g++-13 2>/dev/null || echo g++)
CXXFLAGS := -std=c++17 -Wall -Werror -MMD -MP -Iinclude -Itests
BUILD_DIR := build
BIN_DIR := bin
POLICY ?= fifo
PM ?= 256K

APP_SOURCES := $(wildcard src/domain/*.cpp) $(wildcard src/application/*.cpp) $(wildcard src/infrastructure/*.cpp)
APP_OBJECTS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(APP_SOURCES))
MAIN_OBJECT := $(BUILD_DIR)/main.o

TEST_SOURCES := $(wildcard tests/domain/*.cpp) $(wildcard tests/application/*.cpp) $(wildcard tests/infrastructure/*.cpp) tests/test_main.cpp
TEST_OBJECTS := $(patsubst tests/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))

VMSIM_BIN := $(BIN_DIR)/vmsim
TESTS_BIN := $(BIN_DIR)/tests
DEPFILES := $(APP_OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d)

.PHONY: all clean run test valgrind

all: $(VMSIM_BIN)

$(VMSIM_BIN): $(APP_OBJECTS) $(MAIN_OBJECT) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TESTS_BIN): $(APP_OBJECTS) $(TEST_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

run: $(VMSIM_BIN)
	./$(VMSIM_BIN) input.txt --policy=$(POLICY) --phys-mem=$(PM)

test: $(TESTS_BIN) $(VMSIM_BIN)
	./$(TESTS_BIN)
	@echo ""
	@echo "=== Test 1: Baseline (cabe en memoria, 256K por defecto) ==="
	./$(VMSIM_BIN) tests/cases/test1_baseline.txt --policy=fifo
	@echo ""
	@echo "=== Test 2: Reemplazo forzado, --phys-mem=256K (FIFO vs LRU) ==="
	./$(VMSIM_BIN) tests/cases/test2_replacement.txt --policy=fifo --phys-mem=256K
	./$(VMSIM_BIN) tests/cases/test2_replacement.txt --policy=lru --phys-mem=256K
	@echo ""
	@echo "=== Test 3: Localidad temporal, --phys-mem=256K (FIFO vs LRU) ==="
	./$(VMSIM_BIN) tests/cases/test3_locality.txt --policy=fifo --phys-mem=256K
	./$(VMSIM_BIN) tests/cases/test3_locality.txt --policy=lru --phys-mem=256K

valgrind: $(VMSIM_BIN)
	@command -v valgrind >/dev/null 2>&1 && \
		valgrind --leak-check=full --error-exitcode=1 ./$(VMSIM_BIN) tests/cases/test1_baseline.txt --policy=fifo || \
		( echo "valgrind no disponible, usando -fsanitize=address,leak como alternativa" && \
		  $(CXX) $(CXXFLAGS) -fsanitize=address,undefined -o $(BIN_DIR)/vmsim_asan $(APP_SOURCES) src/main.cpp && \
		  ./$(BIN_DIR)/vmsim_asan tests/cases/test1_baseline.txt --policy=fifo )

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPFILES)
