#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>
#pragma warning(disable : 4996)

#define NO_OF_PROCESS 5
//Name given to the pipe
#define g_szPipeName "\\\\.\\Pipe\\MyNamedPipe"
//Pipe name format - \\.\pipe\pipename

#define BUFFER_SIZE 1024 //1k


int cmpfunc(const void * a, const void * b) {
	return (*(int*)a - *(int*)b);
}

int main(int argc, char* argv[])
{
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	HANDLE hPipe[NO_OF_PROCESS];
	float burst_time_arr[NO_OF_PROCESS] = { 300.0,220.0,180.0,45.0,255.0 };
	float p1_burst_times[5];
	float p2_burst_times[5];
	float p3_burst_times[5];
	float p4_burst_times[5];
	float p5_burst_times[5];

	float temp1[5];
	float temp2[5];
	float temp3[5];
	float temp4[5];
	float temp5[5];


	char* lpCommandLine[NO_OF_PROCESS] = { "child.exe 1", "child.exe 2", "child.exe 3","child.exe 4","child.exe 5" };
	int i = 0;
	

	for (i = 0; i < NO_OF_PROCESS; i++)
	{

		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

		hPipe[i] = CreateNamedPipe(
			g_szPipeName,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFFER_SIZE,              // output buffer size 
			BUFFER_SIZE,              // input buffer size 
			NMPWAIT_USE_DEFAULT_WAIT, // child time-out 
			NULL);                    // default security attribute 


		if (INVALID_HANDLE_VALUE == hPipe[i])
		{
			printf("\nError occurred while "
				"creating the pipe: %d", GetLastError());
			return 1;  //Error
		}
		else
		{
			printf("\nCreateNamedPipe() was successful.");
		}

		if (!CreateProcess(NULL,
			lpCommandLine[i],
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&si[i],
			&pi[i]))
		{
			printf("unable to create process: %d\n", i);
			system("pause");
			ExitProcess(0);
		}
		else
		{
			processHandles[i] = pi[i].hProcess;
			printf("\nParent has created process number %d\n", i+1);
		}

		BOOL bClientConnected = ConnectNamedPipe(hPipe[i], NULL);

		if (FALSE == bClientConnected)
		{
			printf("\nError occurred while connecting"
				" to the child: %d", GetLastError());
			CloseHandle(hPipe[i]);
			return 1;  //Error
		}
		else
		{
			printf("\nConnectNamedPipe() was successful.");
		}

		char szBuffer[BUFFER_SIZE];
		DWORD cbBytes;


		//Read child message
		BOOL bResult = ReadFile(
			hPipe[i],                // handle to pipe 
			szBuffer,             // buffer to receive data 
			sizeof(szBuffer),     // size of buffer 
			&cbBytes,             // number of bytes read 
			NULL);                // not overlapped I/O 

		if ((!bResult) || (0 == cbBytes))
		{
			printf("\nError occurred while reading "
				"from the child: %d", GetLastError());
			CloseHandle(hPipe[i]);
			return 1;  //Error
		}
		else
		{
			printf("\nReadFile() was successful.");
		}

		printf("\nSent from child : %s ", szBuffer);
		int j;
		float next_burst_time;
		float t_n;
		float alfa = 0.5;
		for (j = 0; j < 5; j++) {
			t_n = (rand() % 251) + 50;
			next_burst_time = alfa * t_n + (1 - alfa)*burst_time_arr[i];
			printf("\nPROCESS %d INFORMATION FOR EXECUTAION %d\n", i + 1, j + 1);
			printf("Burst time : %f\n", burst_time_arr[i]);
			printf("Actual length : %f\n", t_n);
			printf("Next brust time : %f\n", next_burst_time);
			burst_time_arr[i] = next_burst_time;

			char t_n_str[100];
			snprintf(t_n_str, 100, "%f", t_n);

			strcpy(szBuffer, t_n_str); // Parent sent to child its actual length via named pipe
			
			if (i == 0) {
				temp1[j] = next_burst_time;

			}

			else if (i == 1) {
				temp2[j] = next_burst_time;

			}

			else if (i == 2) {
				temp3[j] = next_burst_time;

			}
			else if (i == 3) {
				temp4[j] = next_burst_time;

			}
			if (i == 4) {
				temp5[j] = next_burst_time;

			}


		}


		//Reply to child
		bResult = WriteFile(
			hPipe[i],                // handle to pipe 
			szBuffer,             // buffer to write from 
			strlen(szBuffer) + 1,   // number of bytes to write, include the NULL 
			&cbBytes,             // number of bytes written 
			NULL);                // not overlapped I/O 


		if ((!bResult) || (strlen(szBuffer) + 1 != cbBytes))
		{
			printf("\nError occurred while writing"
				" to the child: %d", GetLastError());
			CloseHandle(hPipe[i]);
			return 1;  //Error
		}
		else
		{
			printf("\nWriteFile() was successful.");
		}

	}


	//Print execution orders to parent screen 
	p1_burst_times[0] = 300.0;
	p1_burst_times[1] = temp1[0];
	p1_burst_times[2] = temp1[1];
	p1_burst_times[3] = temp1[2];
	p1_burst_times[4] = temp1[3];

	p2_burst_times[0] = 220.0;
	p2_burst_times[1] = temp2[0];
	p2_burst_times[2] = temp2[1];
	p2_burst_times[3] = temp2[2];
	p2_burst_times[4] = temp2[3];

	p3_burst_times[0] = 180.0;
	p3_burst_times[1] = temp3[0];
	p3_burst_times[2] = temp3[1];
	p3_burst_times[3] = temp3[2];
	p3_burst_times[4] = temp3[3];

	p4_burst_times[0] = 45.0;
	p4_burst_times[1] = temp4[0];
	p4_burst_times[2] = temp4[1];
	p4_burst_times[3] = temp4[2];
	p4_burst_times[4] = temp4[3];

	p5_burst_times[0] = 255.0;
	p5_burst_times[1] = temp5[0];
	p5_burst_times[2] = temp5[1];
	p5_burst_times[3] = temp5[2];
	p5_burst_times[4] = temp5[3];

	float first_exe[5];
	first_exe[0] = p1_burst_times[0];
	first_exe[1] = p2_burst_times[0];
	first_exe[2] = p3_burst_times[0];
	first_exe[3] = p4_burst_times[0];
	first_exe[4] = p5_burst_times[0];

	float second_exe[5];
	second_exe[0] = p1_burst_times[1];
	second_exe[1] = p2_burst_times[1];
	second_exe[2] = p3_burst_times[1];
	second_exe[3] = p4_burst_times[1];
	second_exe[4] = p5_burst_times[1];

	float third_exe[5];
	third_exe[0] = p1_burst_times[2];
	third_exe[1] = p2_burst_times[2];
	third_exe[2] = p3_burst_times[2];
	third_exe[3] = p4_burst_times[2];
	third_exe[4] = p5_burst_times[2];

	float fourth_exe[5];
	fourth_exe[0] = p1_burst_times[3];
	fourth_exe[1] = p2_burst_times[3];
	fourth_exe[2] = p3_burst_times[3];
	fourth_exe[3] = p4_burst_times[3];
	fourth_exe[4] = p5_burst_times[3];

	float fifth_exe[5];
	fifth_exe[0] = p1_burst_times[4];
	fifth_exe[1] = p2_burst_times[4];
	fifth_exe[2] = p3_burst_times[4];
	fifth_exe[3] = p4_burst_times[4];
	fifth_exe[4] = p5_burst_times[4];

	qsort(first_exe, 5, sizeof(float), cmpfunc);
	char* first_1 = "";
	char* second_1 = "";
	char* third_1 = "";
	char* fourth_1 = "";
	char* fifth_1 = "";

	
	if (first_exe[0] == p1_burst_times[0]) { 
		first_1 = "P1";
		
	}
	else if (first_exe[0] == p2_burst_times[0]) {
		first_1 = "P2";
		
	}
	else if (first_exe[0] == p3_burst_times[0]) {
		first_1 = "P3";
		

	}
	else if (first_exe[0] == p4_burst_times[0]) {
		first_1 = "P4";
		

	}
	else if (first_exe[0] == p5_burst_times[0]) {
		first_1 = "P5";
		
	}

	if (first_exe[1] == p1_burst_times[0]) {
		second_1 = "P1";
		

	}
	else if (first_exe[1] == p2_burst_times[0]) {
		second_1 = "P2";
		
	}
	else if (first_exe[1] == p3_burst_times[0]) {
		second_1 = "P3";
		

	}
	else if (first_exe[1] == p4_burst_times[0]) {
		second_1 = "P4";
		

	}
	else if (first_exe[1] == p5_burst_times[0]) {
		second_1 = "P5";
		

	}

	if (first_exe[2] == p1_burst_times[0]) {
		third_1 = "P1";
		

	}
	else if (first_exe[2] == p2_burst_times[0]) {
		third_1 = "P2";
	}
	else if (first_exe[2] == p3_burst_times[0]) {
		third_1 = "P3";
	}
	else if (first_exe[2] == p4_burst_times[0]) {
		third_1 = "P4";
	}
	else if (first_exe[2] == p5_burst_times[0]) {
		third_1 = "P5";
	}

	if (first_exe[3] == p1_burst_times[0]) {
		fourth_1 = "P1";
	}
	else if (first_exe[3] == p2_burst_times[0]) {
		fourth_1 = "P2";
	}
	else if (first_exe[3] == p3_burst_times[0]) {
		fourth_1 = "P3";
	}
	else if (first_exe[3] == p4_burst_times[0]) {
		fourth_1 = "P4";
	}
	else if (first_exe[3] == p5_burst_times[0]) {
		fourth_1 = "P5";
	}

	if (first_exe[4] == p1_burst_times[0]) {
		fifth_1 = "P1";
	}
	else if (first_exe[4] == p2_burst_times[0]) {
		fifth_1 = "P2";
	}
	else if (first_exe[4] == p3_burst_times[0]) {
		fifth_1 = "P3";
	}
	else if (first_exe[4] == p4_burst_times[0]) {
		fifth_1 = "P4";
	}
	else if (first_exe[4] == p5_burst_times[0]) {
		fifth_1 = "P5";
	}

	printf("\nBURST TIMES (ms) OF PROCESS FOR FIRST EXECUTION \nP1 : %f ,P2 : %f ,P3 : %f ,P4 : %f ,P5 : %f \n", p1_burst_times[0], p2_burst_times[0], p3_burst_times[0], p4_burst_times[0], p5_burst_times[0]);
	printf("FIRST EXECUTION ORDER : %s,%s,%s,%s,%s \n ", first_1, second_1, third_1, fourth_1, fifth_1);



	qsort(second_exe, 5, sizeof(float), cmpfunc);
	char* first_2 = "";
	char* second_2 = "";
	char* third_2 = "";
	char* fourth_2 = "";
	char* fifth_2 = "";

	if (second_exe[0] == p1_burst_times[1]) {
		first_2 = "P1";
	}
	else if (second_exe[0] == p2_burst_times[1]) {
		first_2 = "P2";
		
	}
	else if (second_exe[0] == p3_burst_times[1]) {
		first_2 = "P3";
		

	}
	else if (second_exe[0] == p4_burst_times[1]) {
		first_2 = "P4";
		

	}
	else if (second_exe[0] == p5_burst_times[1]) {
		first_2 = "P5";
		

	}

	if (second_exe[1] == p1_burst_times[1]) {
		second_2 = "P1";
		

	}
	else if (second_exe[1] == p2_burst_times[1]) {
		second_2 = "P2";
		

	}
	else if (second_exe[1] == p3_burst_times[1]) {
		second_2 = "P3";
		

	}
	else if (second_exe[1] == p4_burst_times[1]) {
		second_2 = "P4";
		

	}
	else if (second_exe[1] == p5_burst_times[1]) {
		second_2 = "P5";
		

	}

	if (second_exe[2] == p1_burst_times[1]) {
		third_2 = "P1";
		

	}
	else if (second_exe[2] == p2_burst_times[1]) {
		third_2 = "P2";
		
	}
	else if (second_exe[2] == p3_burst_times[1]) {
		third_2 = "P3";
		

	}
	else if (second_exe[2] == p4_burst_times[1]) {
		third_2 = "P4";
		

	}
	else if (second_exe[2] == p5_burst_times[1]) {
		third_2 = "P5";
		

	}

	if (second_exe[3] == p1_burst_times[1]) {
		fourth_2 = "P1";
		

	}
	else if (second_exe[3] == p2_burst_times[1]) {
		fourth_2 = "P2";
		
	}
	else if (second_exe[3] == p3_burst_times[1]) {
		fourth_2 = "P3";
		

	}
	else if (second_exe[3] == p4_burst_times[1]) {
		fourth_2 = "P4";
		

	}
	else if (second_exe[3] == p5_burst_times[1]) {
		fourth_2 = "P5";
		

	}

	if (second_exe[4] == p1_burst_times[1]) {
		fifth_2 = "P1";
		
	}
	else if (second_exe[4] == p2_burst_times[1]) {
		fifth_2 = "P2";
		

	}
	else if (second_exe[4] == p3_burst_times[1]) {
		fifth_2 = "P3";
		

	}
	else if (second_exe[4] == p4_burst_times[1]) {
		fifth_2 = "P4";
		

	}
	else if (second_exe[4] == p5_burst_times[1]) {
		fifth_2 = "P5";
		

	}

	printf("\nBURST TIMES (ms) OF PROCESS FOR SECOND EXECUTION \nP1 : %f ,P2 : %f ,P3 : %f ,P4 : %f ,P5 : %f \n", p1_burst_times[1], p2_burst_times[1], p3_burst_times[1], p4_burst_times[1], p5_burst_times[1]);
	printf("SECOND EXECUTION ORDER : %s,%s,%s,%s,%s \n ", first_2, second_2, third_2, fourth_2, fifth_2);

	qsort(third_exe, 5, sizeof(float), cmpfunc);
	char* first_3 = "";
	char* second_3 = "";
	char* third_3 = "";
	char* fourth_3 = "";
	char* fifth_3 = "";

	if (third_exe[0] == p1_burst_times[2]) {
		first_3 = "P1";
	}
	else if (third_exe[0] == p2_burst_times[2]) {
		first_3 = "P2";

	}
	else if (third_exe[0] == p3_burst_times[2]) {
		first_3 = "P3";


	}
	else if (third_exe[0] == p4_burst_times[2]) {
		first_3 = "P4";


	}
	else if (third_exe[0] == p5_burst_times[2]) {
		first_3 = "P5";


	}

	if (third_exe[1] == p1_burst_times[2]) {
		second_3 = "P1";


	}
	else if (third_exe[1] == p2_burst_times[2]) {
		second_3 = "P2";


	}
	else if (third_exe[1] == p3_burst_times[2]) {
		second_3 = "P3";


	}
	else if (third_exe[1] == p4_burst_times[2]) {
		second_3 = "P4";


	}
	else if (third_exe[1] == p5_burst_times[2]) {
		second_3 = "P5";


	}

	if (third_exe[2] == p1_burst_times[2]) {
		third_3 = "P1";


	}
	else if (third_exe[2] == p2_burst_times[2]) {
		third_3 = "P2";

	}
	else if (third_exe[2] == p3_burst_times[2]) {
		third_3 = "P3";


	}
	else if (third_exe[2] == p4_burst_times[2]) {
		third_3 = "P4";


	}
	else if (third_exe[2] == p5_burst_times[2]) {
		third_3 = "P5";


	}

	if (third_exe[3] == p1_burst_times[2]) {
		fourth_3 = "P1";


	}
	else if (third_exe[3] == p2_burst_times[2]) {
		fourth_3 = "P2";

	}
	else if (third_exe[3] == p3_burst_times[2]) {
		fourth_3 = "P3";


	}
	else if (third_exe[3] == p4_burst_times[2]) {
		fourth_3 = "P4";


	}
	else if (third_exe[3] == p5_burst_times[2]) {
		fourth_3 = "P5";


	}

	if (third_exe[4] == p1_burst_times[2]) {
		fifth_3 = "P1";

	}
	else if (third_exe[4] == p2_burst_times[2]) {
		fifth_3 = "P2";


	}
	else if (third_exe[4] == p3_burst_times[2]) {
		fifth_3 = "P3";


	}
	else if (third_exe[4] == p4_burst_times[2]) {
		fifth_3 = "P4";


	}
	else if (third_exe[4] == p5_burst_times[2]) {
		fifth_3 = "P5";


	}

	printf("\nBURST TIMES (ms) OF PROCESS FOR THIRD EXECUTION \nP1 : %f ,P2 : %f ,P3 : %f ,P4 : %f ,P5 : %f \n", p1_burst_times[2], p2_burst_times[2], p3_burst_times[2], p4_burst_times[2], p5_burst_times[2]);
	printf("THIRD EXECUTION ORDER : %s,%s,%s,%s,%s \n ", first_3, second_3, third_3, fourth_3, fifth_3);



	qsort(fourth_exe, 5, sizeof(float), cmpfunc);
	char* first_4 = "";
	char* second_4 = "";
	char* third_4 = "";
	char* fourth_4 = "";
	char* fifth_4 = "";

	if (fourth_exe[0] == p1_burst_times[3]) {
		first_4 = "P1";
	}
	else if (fourth_exe[0] == p2_burst_times[3]) {
		first_4 = "P2";

	}
	else if (fourth_exe[0] == p3_burst_times[3]) {
		first_4 = "P3";


	}
	else if (fourth_exe[0] == p4_burst_times[3]) {
		first_4 = "P4";


	}
	else if (fourth_exe[0] == p5_burst_times[3]) {
		first_4 = "P5";


	}

	if (fourth_exe[1] == p1_burst_times[3]) {
		second_4 = "P1";


	}
	else if (fourth_exe[1] == p2_burst_times[3]) {
		second_4 = "P2";


	}
	else if (fourth_exe[1] == p3_burst_times[3]) {
		second_4 = "P3";


	}
	else if (fourth_exe[1] == p4_burst_times[3]) {
		second_4 = "P4";


	}
	else if (fourth_exe[1] == p5_burst_times[3]) {
		second_4 = "P5";


	}

	if (fourth_exe[2] == p1_burst_times[3]) {
		third_4 = "P1";


	}
	else if (fourth_exe[2] == p2_burst_times[3]) {
		third_4 = "P2";

	}
	else if (fourth_exe[2] == p3_burst_times[3]) {
		third_4 = "P3";


	}
	else if (fourth_exe[2] == p4_burst_times[3]) {
		third_4 = "P4";


	}
	else if (fourth_exe[2] == p5_burst_times[3]) {
		third_4 = "P5";


	}

	if (fourth_exe[3] == p1_burst_times[3]) {
		fourth_4 = "P1";


	}
	else if (fourth_exe[3] == p2_burst_times[3]) {
		fourth_4 = "P2";

	}
	else if (fourth_exe[3] == p3_burst_times[3]) {
		fourth_4 = "P3";


	}
	else if (fourth_exe[3] == p4_burst_times[3]) {
		fourth_4 = "P4";


	}
	else if (fourth_exe[3] == p5_burst_times[3]) {
		fourth_4 = "P5";


	}

	if (fourth_exe[4] == p1_burst_times[3]) {
		fifth_4 = "P1";

	}
	else if (fourth_exe[4] == p2_burst_times[3]) {
		fifth_4 = "P2";


	}
	else if (fourth_exe[4] == p3_burst_times[3]) {
		fifth_4 = "P3";


	}
	else if (fourth_exe[4] == p4_burst_times[3]) {
		fifth_4 = "P4";


	}
	else if (fourth_exe[4] == p5_burst_times[3]) {
		fifth_4 = "P5";


	}

	printf("\nBURST TIMES (ms) OF PROCESS FOR FOURTH EXECUTION \nP1 : %f ,P2 : %f ,P3 : %f ,P4 : %f ,P5 : %f \n", p1_burst_times[3], p2_burst_times[3], p3_burst_times[3], p4_burst_times[3], p5_burst_times[3]);
	printf("FOURTH EXECUTION ORDER : %s,%s,%s,%s,%s \n ", first_4, second_4, third_4, fourth_4, fifth_4);

	qsort(fifth_exe, 5, sizeof(float), cmpfunc);
	char* first_5 = "";
	char* second_5 = "";
	char* third_5 = "";
	char* fourth_5 = "";
	char* fifth_5 = "";

	if (fifth_exe[0] == p1_burst_times[4]) {
		first_5 = "P1";
	}
	else if (fifth_exe[0] == p2_burst_times[4]) {
		first_5 = "P2";

	}
	else if (fifth_exe[0] == p3_burst_times[4]) {
		first_5 = "P3";


	}
	else if (fifth_exe[0] == p4_burst_times[4]) {
		first_5 = "P4";


	}
	else if (fifth_exe[0] == p5_burst_times[4]) {
		first_5 = "P5";


	}

	if (fifth_exe[1] == p1_burst_times[4]) {
		second_5 = "P1";


	}
	else if (fifth_exe[1] == p2_burst_times[4]) {
		second_5 = "P2";


	}
	else if (fifth_exe[1] == p3_burst_times[4]) {
		second_5 = "P3";


	}
	else if (fifth_exe[1] == p4_burst_times[4]) {
		second_5 = "P4";


	}
	else if (fifth_exe[1] == p5_burst_times[4]) {
		second_5 = "P5";


	}

	if (fifth_exe[2] == p1_burst_times[4]) {
		third_5 = "P1";


	}
	else if (fifth_exe[2] == p2_burst_times[4]) {
		third_5 = "P2";

	}
	else if (fifth_exe[2] == p3_burst_times[4]) {
		third_5 = "P3";


	}
	else if (fifth_exe[2] == p4_burst_times[4]) {
		third_5 = "P4";


	}
	else if (fifth_exe[2] == p5_burst_times[4]) {
		third_5 = "P5";


	}

	if (fifth_exe[3] == p1_burst_times[4]) {
		fourth_5 = "P1";


	}
	else if (fifth_exe[3] == p2_burst_times[4]) {
		fourth_5 = "P2";

	}
	else if (fifth_exe[3] == p3_burst_times[4]) {
		fourth_5 = "P3";


	}
	else if (fifth_exe[3] == p4_burst_times[4]) {
		fourth_5 = "P4";


	}
	else if (fifth_exe[3] == p5_burst_times[4]) {
		fourth_5 = "P5";


	}

	if (fifth_exe[4] == p1_burst_times[4]) {
		fifth_5 = "P1";

	}
	else if (fifth_exe[4] == p2_burst_times[4]) {
		fifth_5 = "P2";


	}
	else if (fifth_exe[4] == p3_burst_times[4]) {
		fifth_5 = "P3";


	}
	else if (fifth_exe[4] == p4_burst_times[4]) {
		fifth_5 = "P4";


	}
	else if (fifth_exe[4] == p5_burst_times[4]) {
		fifth_5 = "P5";


	}

	printf("\nBURST TIMES (ms) OF PROCESS FOR FIFTH EXECUTION \nP1 : %f ,P2 : %f ,P3 : %f ,P4 : %f ,P5 : %f \n", p1_burst_times[4], p2_burst_times[4], p3_burst_times[4], p4_burst_times[4], p5_burst_times[4]);
	printf("FIFTH EXECUTION ORDER : %s,%s,%s,%s,%s \n ", first_5, second_5, third_5, fourth_5, fifth_5);



	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);


	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
		CloseHandle(hPipe[i]);
	}

	system("pause");
	return 1;
}



//C:\\WINDOWS\\system32\\mspaint.exe