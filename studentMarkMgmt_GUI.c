#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "linkedList.h"
#include "inputs.h"
#include "student.h"

// db location
#define db_undergraduate "db_undergraduate.bin"
#define db_postgraduate "db_postgraduate.bin"

// window classes
char g_MainWindowClass[] = "studentMarkMgmt";
char g_StudentAddClass[] = "studentMarkMgmt_studentAdd";

// main window
HWND g_hwndMainWindow;
#define listview_table 101
HWND g_hwndListView;

#define button_add 201
#define button_edit 202
#define button_delete 203

#define button_saveDB 204
#define button_loadDB 205

#define button_prev 205
#define button_next 205

// student add window
HWND g_hwndStudentAdd;

int table_addStudent(node **pCurrentList, node *p, int index, va_list args)
{
    // Add some items to the ListView
    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = ListView_GetItemCount(g_hwndListView);
    lvi.iSubItem = 0;
    char idStr[32];
    sprintf(idStr, "%llu", p->value.id);
    lvi.pszText = idStr;
    ListView_InsertItem(g_hwndListView, &lvi);

    lvi.iSubItem = 1;
    lvi.pszText = p->value.name;
    ListView_SetItem(g_hwndListView, &lvi);

    lvi.iSubItem = 2;
    lvi.pszText = p->value.gender == 1 ? "Male" : "Female";
    ListView_SetItem(g_hwndListView, &lvi);

    lvi.iSubItem = 3;
    lvi.pszText = p->value.major;
    ListView_SetItem(g_hwndListView, &lvi);

    lvi.iSubItem = 4;
    char classStr[16];
    sprintf(classStr, "%d", p->value.classid);
    lvi.pszText = classStr;
    ListView_SetItem(g_hwndListView, &lvi);

    return 0;
}

unsigned long long table_getSelectedStudentID()
{
    int selectedIndex = ListView_GetNextItem(g_hwndListView, -1, LVNI_SELECTED);
    if (selectedIndex != -1)
    {
        // Get the value of the first column of the selected row
        char szValue[32];
        ListView_GetItemText(g_hwndListView, selectedIndex, 0, szValue, sizeof(szValue));
        unsigned long long id;
        sscanf(szValue, "%llu", &id);
        return id;
    }
    else
    {
        return 0;
    }
}

void table_reload()
{
    ListView_DeleteAllItems(g_hwndListView);
    students_forEach(table_addStudent);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        // Create the ListView control
        g_hwndListView = CreateWindowEx(
            0,
            WC_LISTVIEW,
            "",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
            16, 54, 588, 320,
            hwnd,
            (HMENU)listview_table,
            GetModuleHandle(NULL),
            NULL);

        // Enable full-row selection
        ListView_SetExtendedListViewStyle(g_hwndListView, LVS_EX_FULLROWSELECT);

        // Set up the columns
        LVCOLUMN lvc;
        lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.cx = 96; // Width of column
        lvc.pszText = "ID";
        lvc.iSubItem = 0;
        ListView_InsertColumn(g_hwndListView, 0, &lvc);

        lvc.cx = 160; // Width of column
        lvc.pszText = "Name";
        lvc.iSubItem = 1;
        ListView_InsertColumn(g_hwndListView, 1, &lvc);

        lvc.cx = 64; // Width of column
        lvc.pszText = "Gender";
        lvc.iSubItem = 2;
        ListView_InsertColumn(g_hwndListView, 2, &lvc);

        lvc.cx = 192; // Width of column
        lvc.pszText = "Major";
        lvc.iSubItem = 3;
        ListView_InsertColumn(g_hwndListView, 3, &lvc);

        lvc.cx = 48; // Width of column
        lvc.pszText = "Class";
        lvc.iSubItem = 4;
        ListView_InsertColumn(g_hwndListView, 4, &lvc);

        CreateWindowEx(
            0,
            "BUTTON",
            "Add",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            16, 16, 96, 24,
            hwnd,
            (HMENU)button_add,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Edit",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            128, 16, 96, 24,
            hwnd,
            (HMENU)button_edit,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Delete",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            240, 16, 96, 24,
            hwnd,
            (HMENU)button_delete,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Save database",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            16, 400, 128, 24,
            hwnd,
            (HMENU)button_saveDB,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Reload database",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 400, 128, 24,
            hwnd,
            (HMENU)button_loadDB,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "< Prev",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            444, 400, 72, 24,
            hwnd,
            (HMENU)button_prev,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Next >",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            530, 400, 72, 24,
            hwnd,
            (HMENU)button_next,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case button_add:
            g_hwndStudentAdd = CreateWindowEx(
                0,
                g_StudentAddClass,
                "Add Student",
                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, 250, 300,
                hwnd, NULL, NULL, NULL);
            break;

        case button_edit:
            g_hwndStudentAdd = CreateWindowEx(
                0,
                g_StudentAddClass,
                "Edit Student",
                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, 250, 300,
                hwnd, NULL, NULL, NULL);
            break;

        case button_delete:
            students_forEach(student_delete, table_getSelectedStudentID());
            table_reload();
            break;

        case button_loadDB:
            loadNodes(db_undergraduate, &stu_undergraduate);
            loadNodes(db_postgraduate, &stu_postgraduate);
            table_reload();
            break;

        default:
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK StudentAddWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        default:
            break;
        }
        break;

    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// register window class
int registerWindow(HINSTANCE hInstance, LRESULT CALLBACK(callback(HWND, UINT, WPARAM, LPARAM)), char *className)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = callback;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // register window classes
    registerWindow(hInstance, MainWndProc, g_MainWindowClass);
    registerWindow(hInstance, StudentAddWndProc, g_StudentAddClass);

    // Step 2: Creating the Window
    g_hwndMainWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_MainWindowClass,
        "Student marks management system",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL);

    if (g_hwndMainWindow == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    ShowWindow(g_hwndMainWindow, nCmdShow);
    UpdateWindow(g_hwndMainWindow);

    // Step 3: The Message Loop
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}