# So that in 5 years you won't be looking for a version of libpng and a way to run it on your Linux
all: convert_png_to_micro80

convert_png_to_micro80: *.cpp *.h
	g++ -O2 -std=c++17 -o$@ *.cpp -lpng -lz

convert_png_to_micro80.exe: *.cpp *.h
	x86_64-w64-mingw32-g++ -std=c++17 -o$@ *.cpp -lpng -lz

