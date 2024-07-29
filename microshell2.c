// #include <string.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <stdio.h>

// int err(char *str)
// {
// 	while (*str)
// 		write(2, str++, 1);
// 	return (1);
// }

// int cd(char **argv, int i)
// {
// 	if (i != 2)
// 		return err("error: cd: bad arguments\n");
// 	if (chdir(argv[1]) == -1)
// 		return err("error: cd: cannot change directory to "), err(argv[1]), err("\n");
// 	return (0);
// }

// int	exec(char **argv, int i, char **envp)
// {
// 	int	fd[2];
// 	int	status;
// 	int	has_pipe = argv[i] && !strcmp(argv[i], "|");

// 	if (!has_pipe && !strcmp(*argv, "cd"))
// 		return cd(argv, i);

// 	if (has_pipe && pipe(fd) == -1)
// 		return err("error: fatal\n");
// 	printf("DEBUG-exec!!! Pipes: %d\n", has_pipe);
// 	int	pid = fork();

// 	//Child process
// 	if (!pid)
// 	{
// 		printf("DEBUG-exec!!! argv = %s\n", argv[i]);
// 		argv[i] = 0;
// 		printf("DEBUG-exec!!! Assigning to zero, argv = %s\n", argv[i]);
// 		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
// 			return err("error: fatal\n");

// 		if (!strcmp(*argv, "cd"))
// 			return cd(argv, i);
// 		printf("DEBUG-exec!!! Execve: %s, %s, %s", argv[i], *argv, *envp);
// 		execve(*argv, argv, envp);
// 		return err("error: cannot execute "), err(*argv), err("\n");
// 	}

// 	waitpid(pid, &status, 0);
// 	//Parent process
// 	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
// 		return err("error: fatal\n");
// 	return WIFEXITED(status) && WEXITSTATUS(status);
// }

// int	main(int argc, char **argv, char **envp)
// {
// 	int	i = 0;
// 	int	status = 0;

// 	if (argc > 1)
// 	{
// 		while (argv[i] && argv[++i])
// 		{
// 			printf("i = %d, argv[i] = %s\n", i, argv[i]);
// 			argv += i;
// 			i = 0;
// 			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";")){
// 				printf("DEBUG-main!!! argv[i] = %d -> %s\n",i, argv[i]);
// 				i++;
// 			}
// 			if (i){
// 				printf("DEBUG-main!!! How many arguments are there %d\n", i);
// 				printf("DEBUG-main!!! %s, %d, %s\n", *argv, i, *envp);
// 				status = exec(argv, i, envp);
// 			}
// 		}
// 	}
// 	return (status);
// }

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int	err(char *str)
{
	while (*str)
		write(2, str++, 1);
	return (1);
}

int	cd(char **argv, int i)
{
	if (i != 2)
		return err("error: cd: bad arguments\n");
	printf("DEBUG-cd!!! argv[1] = %s\n", argv[1]);
	if (chdir(argv[1]) == -1)
		return err("error: cd: cannot change directory to"), err(argv[1]), err("\n");
	return (0);
}

int	exec(char **argv, int i, char **envp)
{
	int	fd[2];
	int	status;
	int	has_pipe = argv[i] && !strcmp(argv[i], "|");

	if (!has_pipe && !strcmp(*argv, "cd"))
		return cd(argv, i);
	
	if (has_pipe && pipe(fd) == -1)
		return err("error: fatal\n");
	
	int	pid = fork();
	if (!pid)
	{
		argv[i] = 0;
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return err("error: fatal\n");
		if (!strcmp(*argv, "cd"))
			return cd(argv, i);
		execve(*argv, argv, envp);
		return err("error: cannot execute "), err(*argv), err("\n");
	}
	waitpid(pid, &status, 0);
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return err("error: fatal\n");
	return WIFEXITED(status) && WEXITSTATUS(status);
}

int	main(int argc, char **argv, char **envp)
{
	int	i = 0;
	int	status = 0;

	if (argc > 1)
	{
		while (argv[i] && argv[++i])
		{
			argv += i;
			i = 0;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;
			if (i)
				status = exec(argv, i, envp);
		}
	}
	return (status);
}