# ============================================================================
# Bin Packing Algorithm Analysis - Makefile
# ============================================================================
# Build the experimental framework for bin packing algorithm analysis.
#
# Usage:
#   make          - Build the runner executable
#   make run      - Build and run experiments
#   make plot     - Generate plots from results
#   make clean    - Remove build artifacts
#   make all      - Build, run experiments, and generate plots
# ============================================================================

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -I.

# Build directory
BUILD_DIR = build

# Source files
RUNNER_SRC = runner.cpp

# Output files
RUNNER_EXE = runner
RESULTS_CSV = results.csv
PLOTS_DIR = plots

# Detect OS for executable extension
ifeq ($(OS),Windows_NT)
    RUNNER_EXE = runner.exe
    RM = del /Q
    RMDIR = rmdir /S /Q
    MKDIR = mkdir
else
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
endif

# Default target
.PHONY: all
all: run plot

# Build the runner executable
$(RUNNER_EXE): $(RUNNER_SRC) common/types.h common/utils.h common/algorithm.h testcase_generator/generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(RUNNER_SRC)

# Build target
.PHONY: build
build: $(RUNNER_EXE)

# Run experiments
.PHONY: run
run: $(RUNNER_EXE)
	./$(RUNNER_EXE) $(RESULTS_CSV)

# Generate plots (requires Python with matplotlib and pandas)
.PHONY: plot
plot: $(RESULTS_CSV)
	python analysis/plot.py $(RESULTS_CSV) $(PLOTS_DIR)

# Install Python dependencies
.PHONY: install-deps
install-deps:
	pip install matplotlib pandas numpy

# Clean build artifacts
.PHONY: clean
clean:
	$(RM) $(RUNNER_EXE) $(RESULTS_CSV)
	$(RMDIR) $(PLOTS_DIR) 2>/dev/null || true
	$(RMDIR) $(BUILD_DIR) 2>/dev/null || true

# Clean and rebuild
.PHONY: rebuild
rebuild: clean all

# Help target
.PHONY: help
help:
	@echo "Bin Packing Algorithm Analysis Framework"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the runner executable"
	@echo "  make run      - Build and run experiments (outputs to results.csv)"
	@echo "  make plot     - Generate plots from results (outputs to plots/)"
	@echo "  make all      - Build, run, and plot"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Dependencies:"
	@echo "  - C++17 compatible compiler (g++ or clang++)"
	@echo "  - Python 3 with matplotlib, pandas, numpy"
	@echo ""
	@echo "Run 'make install-deps' to install Python dependencies"
