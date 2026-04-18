# GitHub Actions workflow_dispatch via `gh` (install: brew install gh && gh auth login)
# Run from the raylib-starters repo root.

.PHONY: ga-check-gh ga-quickstart-c ga-quickstart-c-all ga-quickstart-c-web ga-quickstart-c-linux ga-quickstart-c-linux-x86_64 ga-quickstart-c-linux-arm64

BRANCH ?= $(shell git rev-parse --abbrev-ref HEAD)
GH ?= gh

# Optional: itch channel for HTML5 push only (passed when non-empty; version is always quickstart-c/project.conf)
GA_CHANNEL ?=

ga-check-gh: ## Verify gh CLI is installed and authenticated
	@command -v $(GH) >/dev/null || (echo "Install GitHub CLI: brew install gh" && false)
	@$(GH) auth status

ga-quickstart-c: ga-check-gh ## Dispatch main quickstart-c workflow (GA_BUILD_PLATFORM=all|web|macos|…)
	@if [ -z "$(GA_BUILD_PLATFORM)" ]; then echo 'Set GA_BUILD_PLATFORM (e.g. GA_BUILD_PLATFORM=all)'; exit 1; fi
	@flags='-f build_platform=$(GA_BUILD_PLATFORM)'; \
	if [ -n "$(strip $(GA_CHANNEL))" ]; then flags="$$flags -f channel=$(GA_CHANNEL)"; fi; \
	echo "$(GH) workflow run quickstart-c.yml --ref $(BRANCH) $$flags"; \
	$(GH) workflow run quickstart-c.yml --ref "$(BRANCH)" $$flags

ga-quickstart-c-all: ga-check-gh ## Full release build + S3 + itch (build_platform=all)
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=all

# Thin wrappers (same as: make ga-quickstart-c GA_BUILD_PLATFORM=<value>)
ga-quickstart-c-web: ga-check-gh
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=web

ga-quickstart-c-linux: ga-check-gh
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=linux

ga-quickstart-c-linux-x86_64: ga-check-gh
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=linux-x86_64

ga-quickstart-c-linux-arm64: ga-check-gh
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=linux-arm64

