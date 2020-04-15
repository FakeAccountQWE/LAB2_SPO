#define AMOUNT 10

#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>

using namespace std;

char str[10][30] = {{"\n1) First"}, {"\n2) Second"}, {"\n3) Third"}, {"\n4) Fourth"}, {"\n5) Fifth"},
{"\n6) Sixth"}, {"\n7) Seventh"}, {"\n8) Eighth"}, {"\n9) Ninth"}, {"\n10) Tenth"}};

PROCESS_INFORMATION CreateNewProcess(char* path, char *commandline)
{
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	PROCESS_INFORMATION info;
	ZeroMemory( &info, sizeof(info) );
	if(!CreateProcess(path, commandline, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &info)) 
		printf( "Create Process failed (%d)\n", GetLastError());
	return info;
}
int input() 
{
    if (_kbhit())
        return getch();
    else
        return -1;
}
void Init(char *path)
{
	char choise;
	int n = 0;
	bool empty = false;
																	
	vector<HANDLE> closeEvents;
	vector<HANDLE> CanPrintEvents;
	char EventID[30];

	PROCESS_INFORMATION inf[AMOUNT];

	while(true)
	{
		Sleep(1);
		choise = input();
		switch(choise)
		{			
		case '+':
			{
				if(closeEvents.size() < AMOUNT)
				{					
					sprintf(EventID," %d", closeEvents.size()+1);
					closeEvents.push_back(CreateEvent(NULL ,FALSE, FALSE, EventID )); 

					sprintf(EventID," %dp", CanPrintEvents.size()+1);
					CanPrintEvents.push_back(CreateEvent(NULL ,TRUE, FALSE, EventID ));

					inf[closeEvents.size()-1] = CreateNewProcess( path, EventID );
				}	
				break;
			}

		case '-':
			{
				if(closeEvents.size() > 0)
				{ 
					SetEvent(closeEvents.back()); 
					WaitForSingleObject(inf[closeEvents.size()-1].hProcess,INFINITE);

					if(!CloseHandle(closeEvents.back()))
						printf( "Close Handle failed (%d)\n", GetLastError());
					if(!CloseHandle(CanPrintEvents.back()))
						printf( "Close Handle failed (%d)\n", GetLastError());

					closeEvents.pop_back();
					CanPrintEvents.pop_back();
					if (n >= closeEvents.size()) 
					{
						n = 0;
						empty = true;
					}
				}
				break;
			}
		case 'q':
			{
				if(closeEvents.size() > 0)
				{							
					while(closeEvents.size() > 0)
					{							
						SetEvent(closeEvents.back()); 
						WaitForSingleObject(inf[closeEvents.size()-1].hProcess,INFINITE);

						if(!CloseHandle(closeEvents.back()) || !CloseHandle(CanPrintEvents.back()) || !CloseHandle(inf[closeEvents.size()-1].hProcess)
							|| !CloseHandle(inf[closeEvents.size()-1].hThread))
							printf( "Close Handle failed (%d)\n", GetLastError());

						closeEvents.pop_back();
						CanPrintEvents.pop_back();
					}	
					n = 0;
				}
				printf("\n");
				return;
			}

		}
		if(CanPrintEvents.size() > 0 && WaitForSingleObject(CanPrintEvents[n],0) == WAIT_TIMEOUT )
		{
			if(n >= CanPrintEvents.size()-1 ) n = 0;
			else if(!empty) n++;
			empty = false;

			SetEvent(CanPrintEvents[n]);
		}
	}	
}
void Output(int processNumber)
{
	char EventID[30];
	sprintf(EventID," %d", processNumber);
	HANDLE closeEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,EventID);
	sprintf(EventID," %dp", processNumber);
	HANDLE CanPrintEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,EventID);
	while(true)
	{
		if(WaitForSingleObject(CanPrintEvent, 1) == WAIT_OBJECT_0) 
		{ 			
			for(int i=0; i<strlen(str[processNumber-1]); i++)        
			{
				if(WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0) 
				{
					CloseHandle(closeEvent); CloseHandle(CanPrintEvent); 
					return;
				}				
				printf("%c",str[processNumber-1][i]);
				Sleep(50);
			}
			ResetEvent(CanPrintEvent);
		}

		if(WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)
		{
			CloseHandle(closeEvent); CloseHandle(CanPrintEvent); 
			return;
		}
	}
	return;
}

void main(int argc, char* argv[])
{
    if(argc == 2) { Output(atoi(argv[1])); }
    else Init(argv[0]);						
    return;
}