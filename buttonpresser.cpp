#include <windows.h>


//
// this program continually checks for a window
// and if finds it, presses a button on it
//
// what lives we lead...
//
// set info of window/button below
//


const char *WINDOW_CLASS = "#32770";  // #32770 = dialog window
const char *WINDOW_TITLE = "Windows Update";
const char *BUTTON_TEXT = "&Close";


// turn up to 100 if you want to see the window flash
const int POLLING_FREQUENCY_MS = 30;



void MsgBox(char* s) {
    MessageBox(0, s, "button presser", MB_OK);
}


// this is case sensitive
bool StringBeginsWith(const char *str, const char *front)
{
    while (*front)
    {
        if (!*str)
            return false;

        if (*front != *str)
            return false;

        front++;
        str++;
    }
    return true;
}

// no bounds checking
bool CopyStrTo(const char *s, char *d)
{
    while (*s)
    {
        *d = *s;
        s++;
        d++;
    }
    return true;
}



// just returns the first one found
HWND CheckForWindow(const char *className, const char *titleStartsWith)
{
    const int MAX_DIALOGS = 100;

    HWND dialog = FindWindow(className, 0);

    HWND allDialogs[MAX_DIALOGS];
    int numDialogs = 0;
    while (dialog && numDialogs < MAX_DIALOGS)
    {
        allDialogs[numDialogs++] = dialog;
        dialog = FindWindowEx(0, dialog, className, 0);
    }

    for (int i = 0; i < numDialogs; i++)
    {
        char title[256];
        int resultLength = GetWindowText(allDialogs[i], title, 256);

        if (resultLength)
        {
            if (StringBeginsWith(title, titleStartsWith))
            {
                return allDialogs[i];
            }
        }
    }
    return false;
}



static bool globalAlreadySearching = false;

static char globalButtonStartsWith[256];


BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    char buttonText[256];
    int resultLength = GetWindowText(hwnd, buttonText, 256);

    if (resultLength)
    {
        if (StringBeginsWith(buttonText, globalButtonStartsWith))
        {
            // MsgBox("pressing button");
            SendMessage(hwnd, BM_CLICK, 0, 0);
            return false;
        }
    }

    globalAlreadySearching = true;

    return true; // return true to contiue searching
}




bool CheckForButtonAndPress(
    const char *className,
    const char *titleStartsWith,
    const char *buttonStartsWith)
{
    HWND dialog = CheckForWindow(className, titleStartsWith);
    if (dialog)
    {
        // MsgBox("Found matching window");

        if (!globalAlreadySearching)
        {
            CopyStrTo(buttonStartsWith, globalButtonStartsWith);
            EnumChildWindows(dialog, EnumChildProc, 0);
        }

        // couldn't find button in window
        return false;
    }
    else
    {
        //MsgBox("Didn't find window");
        return false;
    }
}


int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{

    timeBeginPeriod(1);  // sets the granularity of Sleep (in ms)


    while (true)
    {
        MSG Message;
        // if we never create a window, do we never get any messages?
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        if (CheckForButtonAndPress(WINDOW_CLASS, WINDOW_TITLE, BUTTON_TEXT))
        {
            Sleep(1000); // if we press it, wait for a while so we don't spam it
        }

        Sleep(POLLING_FREQUENCY_MS);
    }


    return 0;
}