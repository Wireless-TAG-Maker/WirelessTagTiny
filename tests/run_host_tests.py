#!/usr/bin/env python3
"""Exercise real helper code with Arduino fakes; this is not a device test."""

from pathlib import Path
import argparse
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default="c++")
    parser.add_argument("--variants-dir", required=True, type=Path,
                        help="External Arduino platform variants directory; pins are not copied into this library")
    args = parser.parse_args()
    compiler = shutil.which(args.compiler)
    if compiler is None:
        parser.error(f"C++ compiler not found: {args.compiler}")

    tests = Path(__file__).resolve().parent
    library = tests.parent
    variants = args.variants_dir.expanduser().resolve()
    targets = {
        "wt9932c3_tiny": "esp32c3",
        "wt9932c5_tiny": "esp32c5",
        "wt9932p4_tiny": "esp32p4",
        "wt9932p4x_tiny": "esp32p4",
        "wt9932p4c61_tiny": "esp32p4",
        "wt9932s31_tiny": "esp32s31",
    }
    for variant in targets:
        if not (variants / variant / "pins_arduino.h").is_file():
            parser.error(f"Missing external variant: {variants / variant / 'pins_arduino.h'}")
    with tempfile.TemporaryDirectory(prefix="wt-tiny-host-") as temporary:
        for variant, target in targets.items():
            executable = Path(temporary) / variant
            subprocess.run(
                [
                    compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    "-Wno-unused-parameter",
                    "-I" + str(tests / "fakes"),
                    "-I" + str(variants / variant),
                    "-I" + str(library / "src"),
                    "-DCONFIG_IDF_TARGET_" + target.upper() + "=1",
                    str(library / "src/WirelessTagTiny.cpp"),
                    str(tests / "test_helpers.cpp"), "-o", str(executable),
                ],
                check=True,
            )
            subprocess.run([str(executable)], check=True)
            print("PASS", variant, flush=True)


if __name__ == "__main__":
    main()
