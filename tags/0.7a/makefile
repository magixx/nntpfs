all: cpxprep mplayerfs
	./cpxprep nntpfs.cpx >nntpfs.cpp
#	g++ -O0 -g3 -ggdb -o nntpfs -D_FILE_OFFSET_BITS=64 -lfuse -lz nntpfs.cpp
#	g++ -O0 -g3 -ggdb -o nntpfs -pg -fprofile-arcs -D_FILE_OFFSET_BITS=64 -lfuse -lz nntpfs.cpp
#	g++ -Wall -O0 -g3 -ggdb -o nntpfs -D_FILE_OFFSET_BITS=64 -lfuse -lz nntpfs.cpp
#-D_REENTRANT
#	g++ -O3 -g3 -ggdb -o nntpfs -pg -fprofile-arcs -fprofile-use -D_FILE_OFFSET_BITS=64  -lfuse -lz nntpfs.cpp
#	g++ -O3 -g3 -ggdb -o nntpfs -fprofile-use -D_FILE_OFFSET_BITS=64  -lfuse -lz nntpfs.cpp
	g++ -Wall -O3 -g3 -ggdb -o nntpfs -D_FILE_OFFSET_BITS=64 -lfuse -lz nntpfs.cpp
cpxprep:
	g++ -O3 -o cpxprep cpxprep.cpp
mplayerfs: mplayerfs.cpp
	g++ -O0 -g3 -ggdb -o mplayerfs -D_FILE_OFFSET_BITS=64 -lfuse mplayerfs.cpp