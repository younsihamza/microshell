#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int ft_strlen2d(char **list)
{
    int i =  0;
    while(list[i])
        i++;
    return i;
}
void cd (char **cmd_text)
{
	int a;
	int i = 0;
	if(cmd_text[1] == NULL)
	{
		write(2,"error: cd: bad arguments\n",26);
		return;
	}else if(cmd_text[1][0] == '-')
		{
			write(2,"error: cd: bad arguments\n",26);
			return;
		}
	a = chdir(cmd_text[1]);
	if(a == -1)
	{
		write(2,"cd: cannot change directory to ",32);
		while(cmd_text[1][i])
		{
			write(2,&cmd_text[1][i],1);
			i++;
		}
		write(2,"\n",1);
		return ;
	}
}
void *ft_calloc(int size ,int len)
{
    void *str = malloc(size*len);
    char *s ;
    s = (char *)str;
    int i = 0;
    while(i < size*len)
       s[i++] = '\0';
    return(str);
}
char **ft_join2d(char **list,char *new)
{
    int i = 0;
    int len;
    char **newlist;
    if(list == NULL)
    {
        newlist = ft_calloc(sizeof(char *),2);
        *newlist = new;
        return newlist;
    }
    len = ft_strlen2d(list);
    newlist = ft_calloc(sizeof(char *),len + 2);
    while(list[i])
    {
        newlist[i] = list[i];
        i++;
    }
    newlist[i] = new; 
    return newlist;
}
int ft_strlen(char *str)
{
    int i = 0;
    while(str[i])
        i++;
    return(i);
}
void cmd(char **cmd_text,char **env)
{
    execve(cmd_text[0],cmd_text,env);
    write(2,"error: cannot execute ",23);
    write(2,cmd_text[0],ft_strlen(cmd_text[0]));
    write(2,"\n",1);
}
int number_of_pipe(char **table_op)
{
    int i = 0;
    int number_pipe = 0;
    if(table_op == NULL)
        return(0);
    while(table_op[i])
    {
        if(strcmp(table_op[i],"|") == 0)
            number_pipe++;
        i++;
    }
    return(number_pipe);
}
void close_file(int **pipes,int  number_pipe)
{
    int i = 0;
    while(i < number_pipe)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
        i++;
    }
}
void execute_cmd(char ***table_cmd,char **table_op,char **env,int operator)
{
    int i = 0;
    int id;
    int number_pipe;
    int **pipes  = NULL;
    number_pipe = number_of_pipe(table_op);
    if(number_pipe != 0)
    {
        pipes  = ft_calloc(sizeof(int *),number_pipe+1);
        while(i < number_pipe)
        {
            pipes[i] = ft_calloc(sizeof(int),number_pipe);
            pipe(pipes[i]);
            i++;
        }
    }
    i = 0;
    while(i <= operator)
    {
        if(table_cmd[i] != NULL)
        {

			if(strcmp(table_cmd[i][0],"cd") == 0)
			{
				if(table_op[0] == NULL)
					cd(table_cmd[i]);
				else if(strcmp(table_op[i],"|") != 0)
					cd(table_cmd[i]);
				else if(i != 0  && table_op[i - 1] != NULL)
					{
						if(strcmp(table_op[i - 1],"|") != 0)
						cd(table_cmd[i]);
					}
			}
			else 
			{
				id = fork();
				if(id == 0)
				{
					if(table_op[i] != NULL)
						if(strcmp(table_op[i],"|") == 0)
							dup2(pipes[i][1],1);

					if(i != 0  && table_op[i - 1] != NULL)
						if(strcmp(table_op[i - 1],"|") == 0)
							dup2(pipes[i - 1][0],0);
					close_file(pipes,number_pipe);
					cmd(table_cmd[i],env);
					exit(0);
				}
        }
		}
        if(table_op[i] != NULL)
            if(strcmp(table_op[i],"|") != 0)
                waitpid(0,NULL,0);
        i++;
    }
    while(waitpid(0,NULL,0)!= -1 )
    {
        close_file(pipes,number_pipe);
    }
	i = 0;
	while(i < number_pipe)
	{
		free(pipes[i++]);
	}
	free(pipes);
}
void build_cmd(char **argv,char **env)
{
    int i = 1;
    int increment_cmd = 0;
    int increment_op = 0;
    int operateur = 0;
    char ***table_cmd = NULL;
    char **table_op = NULL;
    char **tmp = NULL;
	char **tmp1 = NULL;
    while(argv[i])
    {
        if(strcmp(argv[i],";") == 0 || strcmp(argv[i],"|") ==0)
            operateur++;
        i++;
    }
    i = 1;
    table_op = ft_calloc(sizeof(char *),operateur + 1);
    while(argv[i])
    {
         if(strcmp(argv[i],";") == 0 || strcmp(argv[i],"|") == 0)
            table_op[increment_op++] = argv[i];
        i++;
    }

    i = 1;
    table_cmd = ft_calloc(sizeof(char**),operateur + 2);
    while(argv[i])
    {
        tmp = NULL;
        while(argv[i])
        {
            if(strcmp(argv[i],";") == 0 || strcmp(argv[i],"|") == 0)
                break;
			tmp1 = tmp;
            tmp = ft_join2d(tmp,argv[i]);
			if(tmp1 != NULL)
				free(tmp1);
            i++;
        }
        table_cmd[increment_cmd++] = tmp;
        if(argv[i]!= NULL)
            i++;
    }
    execute_cmd(table_cmd,table_op,env,operateur);
	i = 0;
	while(table_cmd[i])
	{
		free(table_cmd[i]);
		i++;
	}
	free(table_cmd);
	free(table_op);
}
int main(int argc,char **argv,char **env)
{
    (void)argc;
    build_cmd(argv,env);
    
}
