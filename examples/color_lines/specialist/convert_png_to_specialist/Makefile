all: convert_png_to_specialist

convert_png_to_specialist: *.cpp *.h
	g++ -O2 -std=c++17 -o$@ *.cpp -lpng -lz

convert_png_to_specialist.exe: *.cpp *.h
	x86_64-w64-mingw32-g++ -std=c++17 -o$@ *.cpp -lpng -lz

