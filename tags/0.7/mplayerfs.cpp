/*
Copyright (c) 2008 Radu Andrei Stefan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

int currentfile=0;
int mplayerrunning=0;
int pipeid[2];
int mppid;
int written;

char *execp="/usr/local/bin/mplayer";
char *execn="mplayer";
char **params;
char *mountpoint="/mnt/mplayerfs";

typedef char * pchar;

static int mplayerfs_chmod(const char * fn, mode_t t)
{
	return 0;
}


static int mplayerfs_chown(const char * fn, uid_t q, gid_t w)
{
	return 0;
}
		

static void handler_sigpipe(int i)
{
	close(pipeid[1]);
	mplayerrunning=0;
	return;
}

static int mplayerfs_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	else return -ENOENT;
}

static int mplayerfs_fgetattr(const char *path, struct stat *stbuf, fuse_file_info* fi)
{
	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mode = S_IFREG | 0644;
	stbuf->st_nlink=1;
	return 0;
}


static int mplayerfs_readdir(const char *p, void *buf, fuse_fill_dir_t fl, off_t off, fuse_file_info* fi)
{
	if (strcmp(p, "/") != 0) return -ENOENT;

	fl(buf, ".", NULL, 0);
	fl(buf, "..", NULL, 0);

	return 0;
}

void createmplayerthread()
{
	int res;
	printf("Creating pipe: ");
	res=pipe(pipeid);
	if (res==-1)
	{
		printf("error %x = %s\n",errno,strerror(errno));
		return;
	}
	printf("successful\n");
	printf("Forking: ");
	res=fork();
	if (res==-1)
	{
		printf("error %x = %s\n",errno,strerror(errno));
		close(pipeid[0]);
		close(pipeid[1]);
		return;
	}
	printf("successful\n");	
	if (res==0)
	{
		// child
		close(0);
		close(pipeid[1]);
		res=dup2(pipeid[0],0);
		if (res<0)
		{
			printf("error perfoming dup2 in child %x = %s\n",errno,strerror(errno));
			exit(1);
		}		
		execv(execp,params);
		printf("error launching executable %s %x = %s\n",execp,errno, strerror(errno));		
		exit(1);
	}
	else
	{
		// parent
		close(pipeid[0]);
		mppid=res;
		mplayerrunning=1;
	}
}

static int mplayerfs_create(const char *path, mode_t mod, struct fuse_file_info *fi)
{
	if (mplayerrunning) return -EACCES;
	written=0;
	createmplayerthread();
	fi->direct_io=1;
	fi->fh=++currentfile;
	
	return 0;
}

static int mplayerfs_open(const char *path, struct fuse_file_info *fi)
{
	if ( (fi->flags & O_CREAT)==0 ) return -EACCES;
	return mplayerfs_create(path,0,fi);
}

static int mplayerfs_write(const char *path, const char *buf, size_t size, off_t of, fuse_file_info *fi)
{
	int sofar=0;
	if (fi->fh != currentfile) return -ENOENT;
	written+=size;
	while (sofar<size)
	{
		printf("Writing: %d/%d\n",sofar,size);
		if (!mplayerrunning) return -ENOENT;
		sofar+=write(pipeid[1],buf+sofar,size-sofar);		
	}
	return size;
}

static int mplayerfs_release(const char *path, struct fuse_file_info *fi)
{
	if (!mplayerrunning) return 0;
	close(pipeid[1]);
	mplayerrunning=0;
	return 0;
}

struct fuse_operations mplayerfs_oper;

void runfuse()
{
	char *fuseargs[3];
	fuseargs[0]="mplayerfs";
	fuseargs[1]=mountpoint;
	fuseargs[2]=0;
	int fuseargcnt=2;
	
	printf("FUSE: setting args\n");
	struct fuse_args args = FUSE_ARGS_INIT(fuseargcnt, fuseargs);
	struct fuse_chan *ch;
	struct fuse *f;
	char *mountpoint;
	printf("FUSE: parsing command line\n");
	if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1
		   && (ch = fuse_mount(mountpoint, &args)) != NULL
	   )
	{
		printf("FUSE: new\n");
		f = fuse_new(ch, &args, &mplayerfs_oper, sizeof(mplayerfs_oper), NULL);
		printf("FUSE: loop\n");
		fuse_loop(f);
		printf("FUSE: destroy\n");
		fuse_destroy(f);
	}
	printf("FUSE: free args\n");
	fuse_opt_free_args(&args);

	
	//return fuse_main(fuseargcnt,fuseargs,&nntpfscbk,NULL);	
}

int main(int argc, char *argv[])
{
	int i;
	
	mplayerfs_oper.getattr  = mplayerfs_getattr;
	mplayerfs_oper.fgetattr = mplayerfs_fgetattr;
	mplayerfs_oper.readdir  = mplayerfs_readdir;
	mplayerfs_oper.open     = mplayerfs_open;
	mplayerfs_oper.create   = mplayerfs_create;
	mplayerfs_oper.write    = mplayerfs_write;
	mplayerfs_oper.release  = mplayerfs_release;
	mplayerfs_oper.chmod    = mplayerfs_chmod;
	mplayerfs_oper.chown    = mplayerfs_chown;

	params=new pchar[3];
	params[0]=execn;
	params[1]="-";
	params[2]=0;

	for (i=1; i<argc; i++) if (0==strcmp(argv[i],"--version"));
	{
		printf("mplayerfs version 0.99");
		return 0;
	}
	
	printf("usage: \n");
	printf("   mplayerfs [mount point] \n");
	printf("   mplayerfs /full/path/to/mplayer [mount point] \n");
	printf("   mplayerfs /full/path/to/mplayer <mount point> options\n");
	if (argc>2)
	{
		execp=argv[1];
		mountpoint=argv[2];
		int i=strlen(execp);
		while (i>=0 && execp[i]!='/') i--;		
		execn=strdup(execp+i+1);
		if (argc>3)
		{
			params=new pchar[argc-1];
			params[0]=execn;
			for (i=3; i<argc; i++)
				params[i-2]=argv[i];
			params[argc-2]=0;
		}
	}
	else if (argc>1)
	{
		mountpoint=argv[1];
	}
	signal(SIGPIPE,handler_sigpipe);
	runfuse();
}
