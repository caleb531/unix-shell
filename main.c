#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// The maximum length allowed for any line in the shell
static const int MAX_LINE_LENGTH = 80;

// Parse the given command into the given pointers for command name and command
// arguments
void parseCmd(char* cmd, char** cmdName, char** cmdArgs) {
	// Split the command using a single space as a delimeter
	char* cmdArg = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
	cmdArg = strtok(cmd, " ");
	// Assume the command name is the first term in the command
	strcpy(*cmdName, cmdArg);
	*cmdName = (char*)cmdArg;
	int i = 0;
	// Assume the arguments comprise every term after the first term
	while (1) {
		if (cmdArg == NULL) {
			break;
		}
		cmdArgs[i] = (char*)malloc((strlen(cmdArg) + 1) * sizeof(char));
		strcpy(cmdArgs[i], cmdArg);
		i += 1;
	}
}

// Execute the given command (or display error if not a real command)
void execCmd(char* cmd) {
	char* cmdName = (char*)malloc(((MAX_LINE_LENGTH / 2) + 1) * sizeof(char));
	char* cmdArgs[(MAX_LINE_LENGTH / 2) + 1];
	parseCmd(cmd, &cmdName, cmdArgs);
	// Log parsed command name and arguments (for testing); since array lengths
	// can't be automatically computed in C, just assume every typed command has
	// three arguments
	printf("Name: %s\n", cmdName);
	printf("Args:\n");
	int i;
	for (i = 0; i < 3; i++) {
		printf("%s\n", cmdArgs[i]);
	}
	// execvp(cmdName, cmdArgs);
}

// Wait for the user to enter a one-line command, returning what was entered if
// possible (or erroring out if not possible)
char* getEnteredCmd() {
	char *cmd = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
	size_t bufferSize;
	size_t cmdLength = getline(&cmd, &bufferSize, stdin);
	if (cmdLength != -1) {
		// Remove trailing newline from entered command (for conveniece)
		cmd[cmdLength - 1] = '\0';
		cmdLength--;
		return cmd;
	} else {
		// Exit if an error occurred while reading the line
		printf("could not read line; exiting...");
		free(cmd);
		exit(1);
	}
}

int main() {

	// Run forever until shell exit
	while (1) {
		printf("osh> ");
		fflush(stdout);
		// Retrieve and execute whatever command was entered (if possible)
		char* cmd = getEnteredCmd();
		// Exit the shell if `exit` command was given
		if (strcmp(cmd, "exit") == 0) {
			return 0;
		}
		// If user entered nothing, continue to next line
		if (strcmp(cmd, "") == 0) {
			continue;
		}
		int pid = fork();
		if (pid == 0) {
			// Execute command within process, then exit child
			execCmd(cmd);
			return 0;
		} else {
			// Make parent process wait for child process to finish
			wait(NULL);
		}
		free(cmd);
	}

	return 0;

}
