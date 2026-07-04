#!/bin/fish

echo "" > codebase.txt; for file in (find src include -type f -name "*.c" -o -name "*.h" -o -name "*.asm")
          echo "=== $file ===" >> codebase.txt
          cat $file >> codebase.txt
          echo -e "\n\n" >> codebase.txt
end
