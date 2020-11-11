#include <windows.h>
#include <string>

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)  /*стартовая функция, создающая главное окно WinMain() */
{
	ShowWindow(GetConsoleWindow(),SW_HIDE);
	system("pythonw GUI.pyw");
	
	return 0;
}