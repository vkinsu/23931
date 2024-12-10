#!/bin/bash

TEST_DIR="test_dir"

if [ -d "$TEST_DIR" ]; then
    rm -rf "$TEST_DIR"
fi

mkdir "$TEST_DIR"
cd "$TEST_DIR" || exit

touch file1.txt file2.txt file3.txt
chmod 644 file1.txt
chmod 600 file2.txt
chmod 755 file3.txt

mkdir empty_dir

echo "Hello, world!" > file_with_content.txt

ln -s file1.txt symlink_to_file1

cd ..
ls -lR "$TEST_DIR"
