#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#pragma warning(disable : 4996)

//Name given to the pipe
#define g_szPipeName "\\\\.\\Pipe\\MyNamedPipe"
#define BUFFER_SIZE 1024 //1k

int main(int argc, char* argv[])
{
	int pid = 0;
	char* message = "";
	HANDLE hPipe;

	if (argc != 2)
	{
		printf("\nerror in child process...now exiting %d\n", argv[0]);
		system("pause");
		exit(0);
	}

	pid = atoi(argv[0]);
	message = argv[1];


	hPipe = CreateFile(
		g_szPipeName,   // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if (INVALID_HANDLE_VALUE == hPipe)
	{
		printf("\nError occurred while connecting"
			" to the parent: %d", GetLastError());
		
		return 1;  //Error
	}
	else
	{
		printf("\nCreateFile() was successful.");
	}

	//We are done connecting to the parent pipe, 
	//we can start communicating with 
	//the parent using ReadFile()/WriteFile() 
	//on handle - hPipe

	float szBuffer[BUFFER_SIZE];
	
	char rslt[2];
	snprintf(rslt, 2, "%d", pid + 1);
	char msg[100] = "P";
	char info[100] = " started.";
	strcat(msg, rslt);
	strcat(msg, info);
	
	strcpy(szBuffer, msg);
	
	DWORD cbBytes;

	//Send the message to parent
	BOOL bResult = WriteFile(
		hPipe,                // handle to pipe 
		szBuffer,             // buffer to write from 
		strlen(szBuffer) + 1,   // number of bytes to write, include the NULL
		&cbBytes,             // number of bytes written 
		NULL);                // not overlapped I/O 

	if ((!bResult) || (strlen(szBuffer) + 1 != cbBytes))
	{
		printf("\nError occurred while writing"
			" to the parent: %d", GetLastError());
		CloseHandle(hPipe);
		return 1;  //Error
	}
	else
	{
		printf("\nWriteFile() was successful.");
	}

	//Read parent response
		
		bResult = ReadFile(
		hPipe,                // handle to pipe 
		szBuffer,             // buffer to receive data 
		sizeof(szBuffer),     // size of buffer 
		&cbBytes,             // number of bytes read 
		NULL);                // not overlapped I/O 

	if ((!bResult) || (0 == cbBytes))
	{
		printf("\nError occurred while reading"
			" from the parent: %d", GetLastError());
		CloseHandle(hPipe);
		return 1;  //Error
	}
	else
	{
		printf("\nReadFile() was successful.");
	}

	
	printf("\nProcess actula length ( Sent from parent ): %s", szBuffer);
	

	char res[2];
	snprintf(res, 2, "%d", pid + 1);
	char msg2[100] = "P";
	char info2[100] = " ended.";
	strcat(msg2, res);
	strcat(msg2, info2);
	
	strcpy(szBuffer, msg2);
	
	CloseHandle(hPipe);
	return 1;
}



