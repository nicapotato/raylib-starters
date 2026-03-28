#!/usr/bin/env python3
"""
Copy non-system dylibs next to a macOS binary and rewrite load paths to @loader_path/<name>.
Used for itch.io zips so users without Homebrew can run the app.
"""
from __future__ import annotations

import os
import shutil
import subprocess
import sys


def is_system_or_special(path: str) -> bool:
    p = path.strip()
    if p.startswith(("/usr/lib/", "/System/")):
        return True
    if p.startswith(("@loader_path", "@rpath", "@executable_path")):
        return True
    return False


def otool_l(path: str) -> list[str]:
    out = subprocess.check_output(["otool", "-L", path], text=True)
    lines = out.strip().splitlines()
    deps: list[str] = []
    for line in lines[1:]:
        part = line.split("(", 1)[0].strip()
        if part:
            deps.append(part)
    return deps


def main() -> int:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <path-to-binary>", file=sys.stderr)
        return 2
    bin_path = os.path.abspath(sys.argv[1])
    if not os.path.isfile(bin_path):
        print(f"error: not a file: {bin_path}", file=sys.stderr)
        return 1

    bindir = os.path.dirname(bin_path)
    queue: list[str] = [bin_path]
    done: set[str] = set()

    while queue:
        f = queue.pop(0)
        if f in done:
            continue
        done.add(f)

        for dep in otool_l(f):
            base = os.path.basename(dep)
            if base == os.path.basename(f):
                continue
            if is_system_or_special(dep):
                continue
            if not os.path.isfile(dep):
                continue

            dest = os.path.join(bindir, base)
            if not os.path.isfile(dest):
                shutil.copy2(dep, dest)
                os.chmod(dest, 0o755)
            if dest not in done:
                queue.append(dest)

            subprocess.run(
                ["install_name_tool", "-change", dep, f"@loader_path/{base}", f],
                check=True,
            )

    print(f"Bundled dylibs for: {bin_path}")
    subprocess.run(["otool", "-L", bin_path], check=False)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
