#include <windows.h>


//
// this program continually checks for a window
// and if finds it, presses a button on it
//
// what lives we lead...
//
// set info of window/button below
// edit: now with more buttons, press as many as you want
//


struct ButtonInfo {
    char *winClass;
    char *winTitle;
    char *buttonText;
    int msDelay;
};

static ButtonInfo info[2] =
{
    {
        "#32770",  // #32770 = dialog window
        "Windows Update",
        "&Close",
        0
    },
    {
        "#32770",  // #32770 = dialog window
        "This is an unregistered copy",
        "Cancel",
        0
    },
};


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
                // MsgBox("Found matching window");
                return allDialogs[i];
            }
        }
    }
    // MsgBox("Didn't find window");
    return false;
}





// maybe recursively call findwindowex to avoid this callback?
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    int infoIndex = (int)lParam;

    char buttonText[256];
    int resultLength = GetWindowText(hwnd, buttonText, 256);

    if (resultLength)
    {
        if (StringBeginsWith(buttonText, info[infoIndex].buttonText))
        {
            // MsgBox("pressing button");
            SendMessage(hwnd, BM_CLICK, 0, 0);

            // don't check for this one again for a while
            // so we don't spam the button (probably not really needed)
            info[infoIndex].msDelay = 500;

            return false;
        }
    }

    return true; // return true to contiue searching
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

        for (int i = 0; i < sizeof(info)/sizeof(info[0]); i++)
        {
            if (info[i].msDelay <= 0)
            {
                HWND dialogBox = CheckForWindow(info[i].winClass, info[i].winTitle);
                if (dialogBox)
                {
                    EnumChildWindows(dialogBox, EnumChildProc, i);
                }
            }
            else
            {
                info[i].msDelay -= POLLING_FREQUENCY_MS;
            }
        }

        Sleep(POLLING_FREQUENCY_MS);
    }


    return 0;
}