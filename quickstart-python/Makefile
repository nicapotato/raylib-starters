# raylib-quickstart-python Makefile

.PHONY: help install run clean test

APP_NAME = raylib-quickstart-python
MAIN_FILE = main.py

UV_VENV = .venv

help:
	@echo "raylib-quickstart-python Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  install        - Install dependencies using uv"
	@echo "  run            - Install dependencies and run the application"
	@echo "  clean          - Remove virtual environment and build artifacts"
	@echo "  test           - Run the application (same as run)"
	@echo ""
	@echo "Examples:"
	@echo "  make install"
	@echo "  make run"

check-uv:
	@command -v uv >/dev/null 2>&1 || { \
		echo "Error: uv is not installed. Please install it first:"; \
		echo "  curl -LsSf https://astral.sh/uv/install.sh | sh"; \
		echo "  Or visit: https://github.com/astral-sh/uv"; \
		exit 1; \
	}

install: check-uv
	@echo "Installing dependencies with uv..."
	@uv sync --no-install-project
	@echo "Dependencies installed"

run: install
	@echo "Copying resources..."
	@cp ../resources/crystal_cave_track.mp3 resources/
	@echo "Running $(APP_NAME)..."
	@uv run python $(MAIN_FILE)

clean:
	@echo "Cleaning..."
	@rm -rf $(UV_VENV)
	@rm -rf __pycache__ *.pyc *.pyo
	@rm -rf build dist *.egg-info
	@echo "Clean complete"

test: run
