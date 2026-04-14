#!/usr/bin/env python3
"""
Run a GUI/desktop binary for a wall-clock duration. Exit non-zero if the process
crashes before the timeout. Exit 0 after SIGTERM if still running (smoke success).
Usage: smoke_run.py <seconds> <command> [args...]
"""
from __future__ import annotations

import os
import subprocess
import sys
import time


def main() -> None:
    if len(sys.argv) < 3:
        print("Usage: smoke_run.py <seconds> <command> [args...]", file=sys.stderr)
        sys.exit(2)
    timeout = float(sys.argv[1])
    cmd = sys.argv[2:]
    env = os.environ.copy()
    p = subprocess.Popen(cmd, env=env)
    t0 = time.monotonic()
    while time.monotonic() - t0 < timeout:
        rc = p.poll()
        if rc is not None:
            sys.exit(rc)
        time.sleep(0.05)
    p.terminate()
    try:
        p.wait(timeout=10)
    except subprocess.TimeoutExpired:
        p.kill()
        p.wait(timeout=5)
    sys.exit(0)


if __name__ == "__main__":
    main()
