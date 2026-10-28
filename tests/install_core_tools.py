#!/usr/bin/env python3
"""Install the exact core's RISC-V build tools with HTTPS and SHA-256 checks."""

import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import urllib.request


def checksum(path):
    value = hashlib.sha256()
    with path.open('rb') as stream:
        for data in iter(lambda: stream.read(1024 * 1024), b''):
            value.update(data)
    return value.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--core-path', required=True, type=Path)
    args = parser.parse_args()
    core = args.core_path.resolve()
    spec = importlib.util.spec_from_file_location('core_tools', core / 'tools/get.py')
    installer = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(installer)
    installer.verbose = False
    host = installer.identify_platform()
    manifest = json.loads((core / 'package/package_esp32_index.template.json').read_text())
    required = {'esp32-arduino-libs', 'riscv32-esp-elf-gcc', 'esptool_py'}
    tools = [tool for tool in manifest['packages'][0]['tools'] if tool['name'] in required]
    if {tool['name'] for tool in tools} != required:
        raise ValueError('Core manifest does not contain the required build tools')
    dist = core / 'tools/dist'
    dist.mkdir(exist_ok=True)
    os.chdir(core / 'tools')
    for tool in tools:
        matches = [system for system in tool['systems'] if system['host'] == host]
        if len(matches) != 1:
            raise ValueError(f'No unique {host} build for {tool["name"]}')
        system = matches[0]
        name = system['archiveFileName']
        algorithm, expected = system['checksum'].split(':', 1)
        if algorithm != 'SHA-256' or Path(name).name != name or not system['url'].startswith('https://'):
            raise ValueError('Unexpected archive name, URL, or checksum algorithm')
        archive = dist / name
        size = int(system['size'])
        valid = archive.is_file() and archive.stat().st_size == size and checksum(archive) == expected
        if not valid:
            partial = archive.with_suffix(archive.suffix + '.partial')
            with urllib.request.urlopen(system['url'], timeout=180) as source, partial.open('wb') as target:
                shutil.copyfileobj(source, target)
            if partial.stat().st_size != size or checksum(partial) != expected:
                raise ValueError(f'Archive integrity check failed for {tool["name"]}')
            partial.replace(archive)
        print(f'Verified {tool["name"]} {tool["version"]}: {expected}', flush=True)
        # The archive now exists and is verified: get_tool only verifies and
        # extracts it with the unchanged core installer, without downloading.
        if not installer.get_tool(system, False, False):
            raise ValueError(f'Core extraction failed for {tool["name"]}')


if __name__ == '__main__':
    main()
