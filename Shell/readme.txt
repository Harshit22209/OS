Group No-14
Group Members
	Harshit Gupta-2022209
	Abhay Kohli-2022015
GitHUB link:- https://github.com/Harshit22209/ASSIGNMENT_2

Assignment 2-> Creating SimpleShell from scratch
        ** Both the bonus are also include **

To Execute the code run the command " make compile " on terminal.
	- It will first compile the simpleShell.c. 
        - then it will execute the executable file ./a.out

Once the code is executed it will ask for username
	->Enter any username or you can simply enter harshit
	->If the user does not exit with given name it will create the directory with that username .
	->After this you will be automaticly redirected to the user directory
	->You will see something like " root@{username}:~$ " like one we have n terminal.This is my custom command prompt.

Commands Supported-
	1)mkdir
	2)rmdir
	3)ls
	4)cd
	5)wc -c
	6)wc -l
	7)echo
	8)sort
	9)grep
	10)uniq
	11)cat
	12)ls -l
	13)ls -R
	14)history
	
* You can also run .sh and c file

Limitations:
* all other commands that are not supported are mainly because it is not feasible to include all the commands(like touch ,vim, other parameters of commands included etc).
* pipe works with these commands cat ,grep,uniq,sort,wc


FILE structure-
	SimpleShell.c
		-it manages user,reading input,creating a process,storing process details etc
	commands.h
		-It contains all the supported commands
	helper.h
		-It contains various funcions like
			-blue,green ->To print in respective color
			-removeNext ->To remove \n
			-getLength  ->To get length of the arrays of strings
			-printLines ->To print all the lines stored in array
			-readInput  ->To read input
			-split	    ->To split the lines based on a parameter


How it is implemented:
	->In main funcion it first ask for usename.If already a directory exist with given username 
	  it conntinue the next step otherwise it will create a directory with that given name.
	->as soon the directory is created,process change it directory to the user directory u.
	->Then a command prompt is shown " root@{username}:~$ " like this.
	->You can now enter the commands that are supported
	->For the commands like executing c file,sh file ,doing sort,cat,grep,uniq,wc a new process 
	  is created through fork
	->But for the commands like cd,mkdir,rmdir,echo ->no new process is created ,it is done within 
	 the parent process. This is done because the effect is needed throughout the process.For instance
	 if we do ' cd ' and create a child process for it ,when it will exit it will automatical come back 
	 to the original directory as the directory change  is done in the child process.
	 That's why we didnt create a seperate command for it.
	->The code also support '&' for executing c file
	->pipe is also supported
	->if you press ctrl+C ,it will show all the commands executed with info like pid,start time and duration.
	->as you change your directory the same will be reflect in the command prompt like we see in the terminal.
	  It will be relative to the userRoot.

Contribution-
	Harshit Gupta:-    
			  -> Handling errors
			  -> Entire simpleShell.c file
			  -> Managing pipe
			  -> Commands Implemented
				-> mkdir, rmdir, ch, ls, grep, cat, echo, sort ,executing c and sh ,history
			  -> Creating all the helper function
	Abhay Kohli:-	
			  ->Commands implemented-
				-wc -c,-wc -l,-uniq,-grep logic,-cat
			  ->Testing
			  ->Helped in creating readme File
				
				
