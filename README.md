# RealTime OS Final Project : Tic-Tac-Toe

A simple TicTacToe game build on MicroC/OS-II environment.


   Unzip the ZIPed files (whole directory tree of Ports) into directory C:\SOFTWARE\UCOS-II
   ![alt text](https://github.com/quochuy2204/TicTacToe-RealTimeOS/blob/master/a.png)


In this exercise directory there is ready made makefile (MAKEFILE.vc) for compiling this exercise
Make modifications to makefile if nessesary, so the path is pointing to exact location of your files
Check that filename in variable EXAMPLE have value rtos_1 also check that UCOS_PORT_EX point to the same directory that this rtos_1.c is located
       ![alt text](https://github.com/quochuy2204/TicTacToe-RealTimeOS/blob/master/b.png)


From Windows menu Open Visual Studio Command Prompt from All Programs/Visual Studio 20xx/Visual Studio Tools
Move to the working directory to place where you have the makefile

"nmake /f makefile.vc all"

   ![alt text](https://github.com/quochuy2204/TicTacToe-RealTimeOS/blob/master/c.png)

"rtos_1.exe"
