// Sender.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <tchar.h>
#include <conio.h>

using namespace std;

typedef struct FHWndsArg
{
	vector<HWND> *vecHWnds;
	DWORD dwProcessId;
}FHWndsArg, *LPHWndsArg;

wchar_t *CustomMultiByteToWideChar(const string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	//第二次调用将单字节字符串转换成双字节字符串
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

DWORD GetProcessIDByName(string pName)
{
	wchar_t* sReceiverName = CustomMultiByteToWideChar(pName.c_str());
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) 
	{
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	DWORD dProcessID = 0;
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (_tcscmp(pe.szExeFile, sReceiverName) == 0) {
			CloseHandle(hSnapshot);
			dProcessID = pe.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);
	//使用完wchar_t*后delete[]释放内存
	delete sReceiverName;
	return dProcessID;
}

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	FHWndsArg *pArg = (LPHWndsArg)lParam;
	DWORD  processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (processId == pArg->dwProcessId)
	{
		pArg->vecHWnds->push_back(hwnd);
	}
	return TRUE;
}

void GetHWndsByProcessID(DWORD processID, vector<HWND> &vecHWnds)
{
	FHWndsArg wi;
	wi.dwProcessId = processID;
	wi.vecHWnds = &vecHWnds;
	EnumWindows(lpEnumFunc, (LPARAM)&wi);
}

void SendCustomMessage(HWND hWnd, string sMsg)
{
	if (hWnd != NULL)
	{
		//准备发送消息内容
		COPYDATASTRUCT CopyData;
		CopyData.dwData = WM_USER + 100;
		CopyData.cbData = sMsg.length();
		CopyData.lpData = const_cast<char *>(sMsg.c_str());
		SendMessageW(hWnd, WM_COPYDATA, 0, (LPARAM)&CopyData);
	}
};

int main()
{
	string sReceiverName = "ProcessCommunication-Win64-DebugGame.exe";
	//找到进程PID
	DWORD pid = GetProcessIDByName(sReceiverName);

	HWND hMainWnd = NULL;
	if (pid != 0)
	{
		//找到进程主窗口句柄
		vector<HWND> vecHWnds;
		GetHWndsByProcessID(pid, vecHWnds);
		
		for (const HWND &h : vecHWnds)
		{
			HWND parent = GetParent(h);
			if (parent == NULL)
			{
				hMainWnd = h;
				break;
			}
		}	
	}

	if (hMainWnd != NULL)
	{
		string sInput;
		while (true)
		{
			if (_kbhit())//非阻塞获取用户输入
			{
				char cTake = _getche();//获取输入字符,并回显		
				if (cTake == 'q' || cTake == 'Q')
				{
					break;
				}
				if (cTake == '\r')
				{
					cout << endl;
					cout << "Send message: " << sInput << endl;
					SendCustomMessage(hMainWnd, sInput);
					sInput.clear();
					continue;
				}
				sInput = sInput + cTake;
			}
		}
	}
	
	return 0;
}