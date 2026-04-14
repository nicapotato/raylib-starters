# GitHub Actions workflow_dispatch via `gh` (install: brew install gh && gh auth login)
# Run from the raylib-starters repo root.

.PHONY: ga-check-gh ga-quickstart-c ga-quickstart-c-all ga-quickstart-c-web

BRANCH ?= $(shell git rev-parse --abbrev-ref HEAD)
GH ?= gh

# Optional: itch channel / version override (passed when non-empty)
GA_CHANNEL ?=
GA_VERSION ?=

ga-check-gh: ## Verify gh CLI is installed and authenticated
	@command -v $(GH) >/dev/null || (echo "Install GitHub CLI: brew install gh" && false)
	@$(GH) auth status

ga-quickstart-c: ga-check-gh ## Dispatch main quickstart-c workflow (GA_BUILD_PLATFORM=all|web|macos|…)
	@if [ -z "$(GA_BUILD_PLATFORM)" ]; then echo 'Set GA_BUILD_PLATFORM (e.g. GA_BUILD_PLATFORM=all)'; exit 1; fi
	@flags='-f build_platform=$(GA_BUILD_PLATFORM)'; \
	if [ -n "$(strip $(GA_CHANNEL))" ]; then flags="$$flags -f channel=$(GA_CHANNEL)"; fi; \
	if [ -n "$(strip $(GA_VERSION))" ]; then flags="$$flags -f version=$(GA_VERSION)"; fi; \
	echo "$(GH) workflow run quickstart-c.yml --ref $(BRANCH) $$flags"; \
	$(GH) workflow run quickstart-c.yml --ref "$(BRANCH)" $$flags

ga-quickstart-c-all: ga-check-gh ## Full release build + S3 + itch (build_platform=all)
	@$(MAKE) ga-quickstart-c GA_BUILD_PLATFORM=all

ga-quickstart-c-web: ga-check-gh ## WASM-only wrapper workflow (.github/workflows/quickstart-c/quickstart-c-web.yml)
	@echo "$(GH) workflow run quickstart-c/quickstart-c-web.yml --ref $(BRANCH)"
	@$(GH) workflow run quickstart-c/quickstart-c-web.yml --ref "$(BRANCH)"
