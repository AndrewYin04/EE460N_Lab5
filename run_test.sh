#!/bin/bash

# Check if at least two arguments were provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 <microcode-file> <file1.asm> [file2.asm] ..."
    exit 1
fi

# Get the microcode file and shift to process the remaining arguments
MICROCODE_FILE=$1
shift

# Compile the C program
gcc -lm -std=c99 -o simulate lc3bsim4.c
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Initialize an array to hold the processed object files
OBJ_FILES=()

# Process each input file
for FILE_LOCATION in "$@"; do
    # Convert the file to Unix format
    dos2unix "$FILE_LOCATION"
    if [ $? -ne 0 ]; then
        echo "dos2unix conversion failed for $FILE_LOCATION."
        exit 1
    fi

    # Assemble the input file
    OBJ_FILE="${FILE_LOCATION%.asm}.obj"
    ./assembler.linux "$FILE_LOCATION" "$OBJ_FILE"
    if [ $? -ne 0 ]; then
        echo "Assembler failed for $FILE_LOCATION."
        exit 1
    fi

    # Add the object file to the list of files to pass to the simulator
    OBJ_FILES+=("$OBJ_FILE")
done

# Run the simulation with the microcode file and all object files
./simulate "$MICROCODE_FILE" "${OBJ_FILES[@]}"
if [ $? -ne 0 ]; then
    echo "Simulation failed."
    exit 1
fi

echo "Process completed successfully."

