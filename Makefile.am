bin_PROGRAMS = nntpfs mplayerfs
noinst_PROGRAMS = cpxprep

nntpfs_SOURCES = nntpfs.cpp
nntpfs_LDADD = -lfuse -lz
man_MANS = nntpfs.1 mplayerfs.1

mplayerfs_SOURCES = mplayerfs.cpp
mplayerfs_LDADD = -lfuse

cpxprep_SOURCES = cpxprep.cpp

BUILT_SOURCES = nntpfs.cpp

nntpfs.cpp: cpxprep fileinfos.cpx fuse.cpx globals.cpx group.cpx grouplist.cpx groupmanager.cpx group.matchpart.cpx groupsp.cpx group.titleop.cpx guessing.cpx includes.cpx matching.cpx nntpclient.cpx nntpfs.cpx openfile.cpx packednumbers.cpx stringset.cpx tests.cpx crc32.cpx
	./cpxprep nntpfs.cpx >nntpfs.cpp
