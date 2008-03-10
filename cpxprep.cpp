#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>

using namespace std;

class list
{
public:
    char *txt;
    list *next;
};

char *exename="cpxprep";

static int listadd(const char *c);
list *root=NULL;

char *namespacename(char *filename);

int listadd(const char *c)
{
    int l=strlen(c);
    list *nxt,*p;
    nxt=root;
    if (root) {
	p=root;
	while (p)
	{
	    if (!strcmp(p->txt,c)) return 0;
	    p=p->next;
	}
    }
    root=new list();
    root->next=nxt;
    root->txt=(char *) malloc(l+1);
    strcpy(root->txt,c);
    return 1;
}

void assume(int b,char *c,char *extra="")
{
    if (b) return;
    fprintf(stderr,"Error: %s.\nErrno:%x\n%s\n",c,errno,extra);
    exit(1);
}

int strxcmp(const char *a, const char *b)
{
	char c;
	int r;
	int len=strlen(a);
	r=strncmp(a,b,len);
	if (r) return r;
	//    printf("b[len]-%c-%i-",b[len],b[len]);
	c=b[len];
	if (c!=' ' && c!='\t' && c!=0 && c!=0 && c!=10 && c!=13 && c!=9 && c!=8) return -1;
	return 0;
}

char *namespacename(const char *filename)
{

	char result[1024],c;
	int i,j;
	strcpy(result,"__CPXNS_");
	j=strlen(result);
	for (i=0; filename[i]!=0 ; i++)
	{
		if (j>100) break;
		c=filename[i];
		if (isalnum(c)) result[j++]=toupper(c);
		else if (c=='/') result[j++]='_';
		else 
		{
			sprintf(result+j,"__%02X_",c);
			j+=4;
		}
	}
	result[j]=0;
	//printf("---%s\n",result);
	return strdup(result);

}

int isfilenamechar(int v)
{
	return isalnum(v)
		|| v=='.' || v=='_' || v=='-'
			;
}

void process(FILE *f,const char *name)
{
	char txt[32768],*p,*p2;
	char *nsn;
	int counter=1;
	int inimplementation=0,innamespace=0;
	printf("# 1 \"%s\"\n",name);
	nsn=namespacename(name);	
	while (!feof(f))
	{
		txt[0]=0;
		fgets(txt,32767,f);
		p=txt;
		while (p[0]==' ' || p[0]=='\t') p++;
		if (!strxcmp("implementation",p))
		{
			inimplementation=1;
			printf("\n");
		} else if (!strxcmp("interface",p))
		{
			printf("\n");
		} else if (!strxcmp("uses",p))
		{
			p+=5;
			while (1)
			{
				// we allow multiple included files in an uses clause				
				while (isblank(*p)) p++;
				//skip leading white spaces
				if (p[0]==';') break;
				if (p[0]==',') { p++; continue; }
				if (p[0]==0 || p[0]==10 || p[0]==13) 
				{
					fprintf(stderr,"%s:%s:%d error\n",exename,name,counter);
					fprintf(stderr,"*** End of line encountered in uses clause, expecting semicolon. ***");
					exit(1);
				}
				p2=p;
				while (isfilenamechar(*p2)) p2++;
				string nameufile(p);
				nameufile=nameufile.substr(0,p2-p)+".cpx";
				p=p2;
				if (listadd(nameufile.c_str()))
				{
					FILE *f=fopen(nameufile.c_str(),"r");
					if (f==NULL)
					{
						fprintf(stderr,"%s:%s:%d error\n",exename,name,counter);
						fprintf(stderr,"*** File to be included, %s, was not found. ***",nameufile.c_str());
						exit(1);
					}
					
					//if (innamespace) printf("}//namespace\n");
					innamespace=0;
					process(f,nameufile.c_str());		
					printf("# %d \"%s\"\n",counter,name);
				};
			}
			printf("\n");
		} else {
			if (inimplementation && !innamespace && txt[0]!=10 && txt[0]!=13)
			{
				innamespace=1;
				//printf("namespace %s {\n",nsn);
				//printf("# %d \"%s\"\n",counter+1,name);			
			}
			printf("%s",txt);
		}
		counter++;
	}
	//if (innamespace) printf("}//namespace\n");
	free(nsn);
	printf("\n\n");
}

int main(int argc, char *argv[])
{
    FILE *f;
    assume(argc==2,"incorrect usage: normal is cpxprep <filename> OR cpxprep -c");
    if (!strcmp("-c",argv[1])) 
    {
	f=fdopen(1,"r");
	process(f,"<console-input>");	
    } else {
	f=fopen(argv[1],"r");
	listadd(argv[1]);
        assume((int) f,"Could not open input file");
	process(f,argv[1]);
    }
    fclose(f);
    return 0;
}
