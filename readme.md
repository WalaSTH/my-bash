readme

# Introduction to the Operating Systems Laboratory

### Group:

Virtualized, named after the OS technique of virtualization.

### Members:

- Leonardo Torres, (leo.torres@mi.unc.edu.ar)
- Matías Scantamburlo, (matias.scantamburlo@mi.unc.edu.ar)
- Maciel Salguero, (maciel.salguero@mi.unc.edu.ar)

# Modularization

### General Idea:

In this laboratory, we will create a shell in the style of the Linux bash, which must have the characteristic properties of a shell, which are:

- Execution of commands in both foreground and background modes.
- Input and output redirection, and piping between commands.

### Modules:

The project is divided into four parts. These are:

1.  Command:
    This will contain the implementation of the abstract data type scommand and pipeline.
2.  Execute:
    This will contain the instructions for executing the commands themselves.
3.  Parser:
    This consists of the parser that interprets the text entered via the keyboard.
4.  Builtin
    This is where the shell's internal commands will be found.

### ADT's (abstract data type):

- ADT scommand: It will consist of the following structure:

!\[scommand\](./img/Captura de pantalla de 2021-09-12 21-19-45.png)

Which is a triple composed of a list of strings where the arguments will go, a string for the in redirection, and a string for the out redirection.

For example, the following command:

```
wc -w < words.txt > numerOfWords.txt
```

It would be stored in a variable of type scommand as follows;

The string list will first contain the main command "wc" along with its arguments, in this case -w, so the list would look like:

```
[“wc”, “-w”]
```

In the string redir_in, the string "words.txt" will be stored, and in the string redir_out, "numberOfWords.txt" will be stored.

Giving as result:

```
([“wc”, “-w”] ,“words.txt”,“numberOfWords.txt”)
```

- ADT Pipeline will consist of the following structure::

!\[pipeline\](./img/Captura de pantalla de 2021-09-12 22-07-01.png)

This is a tuple consisting of scmds, which is a list of scommand, and wait where the boolean indicating whether to wait or not is located.
For example, in the command:

```
ls -lt | grep notes -i | wc -l > notesOnTheList.txt
```

Each simple command will be stored in a variable of type scommand, thus forming a list of tuples. In this case, it would look as follows:

```
[([“ls”,”-lt”],””,””),([“grep”,”notes”,”-i”],””,””),[([“wc”,”-l”],””,”notesOnTheList.txt”)]]
```

As we mentioned earlier, being a tuple, the recently defined element will become the first element, while wait will be the second. In the following manner:

```
([([“ls”,”-lt”],””,””),([“grep”,”notes”,”-i”],””,””),[([“wc”,”-l”],””,”notesOnTheList.txt”)]],1)
```

# Developement

In this laboratory, work mainly took place in 3 files:

1.  command.c
2.  execute.c
3.  builtin.c
    Below, we will describe what was done in each file, the problems that arose, and how they were solved.

### command.c:

- General Idea:
  Our first objective at this stage was to program the scommand ADT commands, as they were the basis for implementing the pipeline ADT and the overall project development.
  To do this, first and foremost, we had to familiarize ourselves with the structure of simple commands, so that we could then study and understand the external libraries better. Once this was done, we proceeded to use the glib-2.0 libraries for the initial instructions, which did not cause us major complications. We then continued with the implementation of pipeline instructions, which required a new study of the libraries, but similar to the previous ADTs, we didn't encounter many obstacles.

- Problems encountered:

1. Initially, we struggled to make good use of the external library.
2. In the implementation of **scommand_to_string()**, we made a mistake in using **g_slist_nth()** to call the nth argument of the scommand.
3. We had issues with memory deallocation as we opted for using **g_slist_free_full()**.

- Solutions provided:

1. We thoroughly read each of the functions and closely followed their structure.
2. We used the **g_slist_nth_data()** function and avoided the practice of casting to the node.
3. As a solution, we decided to free memory element by element using a while loop.

### builtin.c:

In this second stage, our objective was to implement the internal commands of our mybash:

1. **cd()**: First, we defined the body of the function **builtin_is_cd()**, which compares the first element of the pipeline with the string "cd". This function will be used in **builtin_exec()**. If the antecedent (**builtin_is_cd()**) is true, we will take the path following "cd", and using the **chdir()** function, we will move to the specified directory.

2. **exit()**: We defined the function **builtin_is_exit()**, which will take the first simple command of the pipeline and compare it with the string "exit".

### execute.c:

- General idea:

The central objective of this part was to program the function that would execute terminal commands.
To accomplish this, the first step was to differentiate between internal and external commands. For internal commands, we used the functions programmed in the Builtin.c module.

```
Next, we will explain how the first version of this function was implemented, i.e., the one that was prepared only for 2 simple commands. In case you want to read about the version with multiple pipes, please proceed to the section marked with asterisks.
```

Essentially, what was done was to use the **fork()** function to create a process for each command, and through the **pipe()** function in C, the output of the left program was redirected to the input of the right program. To perform the redirection, the **dup2()** function was used. Finally, the respective commands were executed using the **execvp()** function.

Additionally, during implementation, the following considerations were taken into account:

1. Closing the writing/reading ends when they are not used.
2. Checking the return values of the functions mentioned in the paragraph above.
3. Waiting for the completion of the respective processes using the **waitpid()** function.

Regarding redirections, what was done was to check for each command that a redirection was indeed performed, and if so, in addition to what was described above, the file was opened, and using the **dup** function again, the redirection was changed based on whether it was for input or output.

- Problems encountered:

1. One of the problems we encountered when testing the 1st implementation was that the bash worked correctly for pipes with 2 elements but did not work for simple commands.

- Solutions provided:

1. The problem arose from the fact that regardless of whether it was 1 or 2 commands received by the bash, the output was still being redirected. So, what was done was to implement a boolean variable that answers the question "Is only one command to be executed?" If the answer is true, then the command's output was sent to the terminal, and the execution ended. If not, it continued as before.

# Extra points

- Multiple Pipes:
  The central idea was to create, for a number **n** of commands, **n** processes and **n-1** pipes. To do this, we created a function that is responsible for generating the processes, taking the following input parameters:

1. The file descriptor of the previous scommand.
2. A boolean variable that answers the question "Is this the last command to be executed?".
3. The scommand in question.
4. The reading/writing ends that will be used to read and write to the buffer created by **pipe()**.

Now, we will check whether we are at the beginning, in the middle, or at the end of the pipe, since in each case, a different treatment must be performed:

1. If we are at the beginning: we must take the standard input as input and return the command's result to the writing end. For this, the standard output will be redirected to the fourth file descriptor created earlier.
2. If we are in the middle: we must take as input what the previous command left us. To do this, we will read the reading end, redirecting the standard input to the third file descriptor, and similar to the previous case, return the command's result to the writing end.
3. If we are at the end of the pipe, we will take the reading end as input and return the result to the standard output.

Finally, this function returns the PIDs of the child processes so that they can be closed.

- User path and colors:

For aesthetic and convenience, the following were added to the prompt:

1. Colors
2. User name
3. An absolute path to the current directory

To log the user, the **getlogin()** function was called, while to display the current directory, **getcwd()** was used (both functions imported from the **<unistd.h>** library). Finally, for the colors, a code was implemented to assign the user the color green and the directory the color blue.

# Conclusions

- A much better understanding of how a bash works was achieved, including how the connection and execution of its processes function.
- Experience was gained in the practice of not reinventing the wheel, which includes not breaking the abstraction of a library, but rather, making a good reading about its application.
- The journey into the world of git began, after several clashes, and the basic and elementary commands for teamwork were learned.
- The Lab project was delivered with three failures:
  <img src="../_resources/6f4aaff360604f60bef292b2f29fbfe8.png" alt="31377bc6981c3b632e43ddd849c18aa7.png" width="775" height="54">
  However, we have an idea of what could have been:
  These three failures are related to how we are implementing the multiple-pipeline, as we need to connect the input and output of each program, respectively, one after the other. This makes it impossible for us to pass all the tests.
  We're not completely sure, but it's very likely that if we used only two commands in a pipeline, we wouldn't have these failures.
