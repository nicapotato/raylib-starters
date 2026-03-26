#!/usr/bin/env bash
# Wrapper: first zig invocation may fail while resolving deleted hexops/xcode-frameworks (see patch-raylib-xcode-mirror.sh).
set -eo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if zig build "$@"; then
  exit 0
fi
bash "${SCRIPT_DIR}/patch-raylib-xcode-mirror.sh"
exec zig build "$@"
