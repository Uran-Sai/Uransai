#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
int main(){
	char c,*str=NULL;
	int** fd;
	int i=0,count=4,count2=4,count3=4,l=0,j=0,count_com=1,flag_out=0,flag_in=0,flag_fon=0,m,first_com,vvod=dup(0),vyvod=dup(1);
	char **str2=malloc(sizeof(char *)*count),***str3;
	char *str_in=malloc(sizeof(char)),*str_out=malloc(sizeof(char));
	while (1){
	str=NULL;
	i=0;
	str_out=NULL;
	count=4;
	count2=4;
	count3=4;
	l=0;j=0;count_com=1;flag_out=0;flag_in=0;flag_fon=0;
	printf("Введите команды :\n");
		while((c=getchar())!='\n'){
		str=realloc(str,sizeof(char)*(i+1));
		str[i++]=c;
		if (c=='|')
			count_com++;
		if (c=='&')
			flag_fon=1;
		if (c=='>')
			flag_out++;
		if (c=='<')
			flag_in=1;

	}
	//printf("flag_in=%d,flag_out=%d,flag_fon=%d,count_com=%d\n",flag_in,flag_out,flag_fon,count_com);
	str3=malloc(sizeof(char **)*count_com+1);
	str[i]='\0';
	//printf("str=%s\n",str);
	i=0;
	count=4;
	l=0;
	while(str[i]!='\0'&&str[i]!='&'){//создание трехмерного массива
		int k=0;
		str3[l]=malloc(sizeof(char **)*count3);
		while (str[i]!='|'&&str[i]!='<'&&str[i]!='>'&&str[i]!='&'&&str[i]!='\0'){ 
			str3[l][j]=malloc(sizeof(char)*count2);
			while (str[i]!=' '&&str[i]!='|'&&str[i]!='&'&&str[i]!='>'&&str[i]!='<'&&str[i]!='\0'){ 
				if (j==count3){
					count3+=4;
					str3[l]=realloc(str3[l],sizeof(char *)*count3);
				}
				if (k==count2){
					count2+=4;
					str3[l][j]=realloc(str3[l][j],sizeof(char)*count2);
				}
				str3[l][j][k++]=str[i];
				i++;

			}
			count2=4;
			if (k){
				str3[l][j][k]='\0';
				//printf("str3[%d]=%s\n",l,str3[l][j]);
				j++;
			}
			if (str[i]=='\0'||str[i]=='&')
				str3[l][j]=NULL;
			if (str[i]=='\0'||str[i]=='|'||str[i]=='&'||str[i]=='<'||str[i]=='>')
				break;
			i++;
			k=0;
		}
		/*for (m=0;m<=j;m++)
			printf("str3[%d][%d]=%s\n",l,m,str3[l][m]);*/
	        if (str[i]=='<'){
			i++;
			int j=0;
			while (str[i]==' ')
				i++;
			//printf("here\n");
			while(str[i]!=' '&&str[i]!='>'&&str[i]!='&'&&str[i]!='\0'){
				str_in=realloc(str_in,sizeof(char)*(j+1));
				str_in[j++]=str[i];
				i++;
			}
		}
		if (str[i]=='>'){
			i++;
			int j=0;
			while(str[i]==' ')
				i++;
			if (str[i]=='>')
				i++;
			while (str[i]==' ')
				i++;
                	while(str[i]!=' '&&str[i]!='&'&&str[i]!='\0'){
				str_out=realloc(str_out,sizeof(char)*(j+1));
				str_out[j++]=str[i];
				i++;
			}
		}
		if (j>=count){
			count+=4;
			str3[l]=realloc(str3[l],sizeof(char *)*count);
		}
		if((!l)&&flag_in){
			str3[l][j]=str_in;
			first_com=j;
			str3[l][j+1]=NULL;
		}
		else
			str3[l][j]=NULL;
	       /* for (m=0;m<=j;m++)
			printf("str3[%d][%d]=%s\n",l,m,str3[l][m]);*/
        	if (str[i]=='\0'||str[i]=='&'||str[i]=='<'||str[i]=='>')
				break;
		j=0;
		i++;
		if (i>=strlen(str))
			break;
		l++;
		count3=4;

	}
	free(str);
	if (flag_in){// <
		if ((m=open(str_in,O_RDONLY))==-1)
			return 0;
		//dup2(m,0);
		close(m);
	}
	if (flag_out==1){// > 
		if ((m=open(str_out,O_WRONLY|O_TRUNC|O_CREAT,0660))==-1)
			return 0;
		dup2(m,1);
		close(m);
	}
	if (flag_out==2){// >>
		printf("str_out=%s\n",str_out);
		if((m=open(str_out,O_WRONLY|O_CREAT|O_APPEND,0660))==-1){
			printf("llllll\n");
			return 0;
		}
		dup2(m,1);
		close(m);
	}
	if (flag_fon){// &
		if ((m=fork())==-1)
				return 0;
		if (m){
			wait(NULL);
			wait (NULL);
			continue;			
		}
		else {
		    if ((m=fork())==-1)
			return 0;
		if (m)
			exit(0);
		signal(SIGINT,SIG_IGN);
		m=open("/dev/null",O_RDONLY);
		dup2(m,0);
		}
	}
	fd=(int **)malloc(sizeof(int *)*count_com);// массив каналов
	for(i=0;i<(count_com);i++){
		fd[i]=malloc(2*sizeof(int));
		pipe(fd[i]);
		if((j=fork())==-1)
			return 1;
		if(j==0){
			if(i){
				dup2(fd[i-1][0],0);
				close(fd[i-1][0]);
			}
			if(i!=(count_com-1))
				dup2(fd[i][1],1);
			close (fd[i][0]);
			close (fd[i][1]);
			execvp(str3[i][0],str3[i]);
			int j;
			//while(str3[i][j]!=NULL)
			//	free(str3[i][j]);
			exit(0);
		}
		close (fd[i][1]);
		if(i)
			close (fd[i-1][0]);
	}
	if (count_com!=1)
		close (fd[i-1][0]);
	while(wait(NULL)!=-1){
		;
	}
		//printf ("process finished\n");
	dup2(vvod,0);
	dup2(vyvod,1);
	if (flag_fon)
		exit(0);
	free (str_out);
	}
}



		


