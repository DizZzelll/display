#!/usr/bin/env python3
"""
Простой скрипт для Windows без Unicode символов
"""

import os
import re
import shutil
import glob
import sys

def main():
    #print("=== Add Version to HEX Files ===")
    
    # 1. Ищем version.c
    version_file = None
    search_paths = ["User/version.c"]
    
    for path in search_paths:
        if os.path.exists(path):
            version_file = path
            break
    
    if not version_file:
        print("ERROR: version.c not found!")
        return
    
    #print(f"Found version file: {version_file} ",end="")
    
    # 2. Читаем версию
    with open(version_file, 'r') as f:
        content = f.read()
    
    match = re.search(r'"(\d+\.\d+)"', content)
    if not match:
        print("ERROR: Version not found in format 'X.Y'")
        return
    
    version = match.group(1)
    print(f"Version: {version}")
    
    # 3. Ищем HEX файлы
    hex_dir = "build/Debug"
    if not os.path.exists(hex_dir):
        print(f"ERROR: Directory not found: {hex_dir}")
        return
    
    hex_files = glob.glob(os.path.join(hex_dir, "*.hex"))
    if len(hex_files) == 0:
        print("ERROR: No HEX file found!")
        return
    elif len(hex_files) > 1:
        print("ERROR: found multiples HEX files!")
        return
    else:
        print(f"HEX file: {os.path.basename(hex_files[0])} ")

    # 4. Создаем копию с версией в директории "prod"
    prodhex_dir = "build/Debug/prod"
    if not os.path.exists(prodhex_dir):
        os.mkdir(prodhex_dir)

    filename = os.path.basename(hex_files[0])
    name, ext = os.path.splitext(filename)
        
    # Новое имя: name_version.hex
    new_name = f"{name}_{version}{ext}"
    new_path = os.path.join(prodhex_dir, new_name)

    shutil.copy2(hex_files[0], new_path)
    print(f"Copy created: prod/{new_name}")
    
    print("\nDone!")

if __name__ == "__main__":
    main()