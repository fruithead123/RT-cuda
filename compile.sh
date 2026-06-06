# Define the build and src directories
cmake -B build -S .

# Build the project
cmake --build build --clean-first
