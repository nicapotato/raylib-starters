#!/usr/bin/env bash
# raylib (via raylib-zig) still points xcode_frameworks at git+https://github.com/hexops/xcode-frameworks,
# which was removed from GitHub (raylib PR #5693 switched to pkg.machengine.org). Zig then fails with
# ProtocolError when probing that URL. The tarball is the same content; the declared .hash stays valid.
set -eo pipefail

GIT_URL='git+https://github.com/hexops/xcode-frameworks#9a45f3ac977fd25dff77e58c6de1870b6808c4a7'
TAR_URL='https://pkg.machengine.org/xcode-frameworks/9a45f3ac977fd25dff77e58c6de1870b6808c4a7.tar.gz'

patch_one() {
  local zon="$1"
  [[ -f "$zon" ]] || return 0
  grep -q 'hexops/xcode-frameworks' "$zon" || return 0
  if sed --version >/dev/null 2>&1; then
    sed -i "s|${GIT_URL}|${TAR_URL}|g" "$zon"
  else
    sed -i '' "s|${GIT_URL}|${TAR_URL}|g" "$zon"
  fi
  echo "Patched xcode_frameworks URL in ${zon}"
}

roots=()
[[ -n "${ZIG_GLOBAL_CACHE_DIR:-}" ]] && roots+=("${ZIG_GLOBAL_CACHE_DIR}/p")
[[ -d "${HOME}/.cache/zig/p" ]] && roots+=("${HOME}/.cache/zig/p")
[[ -d "${PWD}/.zig-cache/p" ]] && roots+=("${PWD}/.zig-cache/p")

for root in "${roots[@]}"; do
  [[ -d "$root" ]] || continue
  shopt -s nullglob
  for zon in "${root}"/raylib-5.6.0-dev-*/build.zig.zon; do
    patch_one "$zon"
  done
  shopt -u nullglob
done
