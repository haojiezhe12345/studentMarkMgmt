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

#define button_loadDB 204
#define button_saveDB 205

#define button_prev 205
#define button_next 205

// student add window
HWND g_hwndStudentAdd;
#define radio_stuAdd_undergraduate 301
#define radio_stuAdd_postgraduate 302
HWND g_hwndStudentAdd_Undergraduate;
HWND g_hwndStudentAdd_Postgraduate;

#define label_stuAdd_name 401
#define input_stuAdd_name 402
HWND g_hwndStudentAdd_Name;

#define label_stuAdd_gender 403
#define radio_stuAdd_gender_male 404
#define radio_stuAdd_gender_female 405
HWND g_hwndStudentAdd_Male;
HWND g_hwndStudentAdd_Female;

#define label_stuAdd_major 406
#define input_stuAdd_major 407
HWND g_hwndStudentAdd_Major;

#define label_stuAdd_class 408
#define input_stuAdd_class 409
HWND g_hwndStudentAdd_Class;

#define label_stuAdd_direction 410
#define input_stuAdd_direction 411
HWND g_hwndStudentAdd_Direction_Label;
HWND g_hwndStudentAdd_Direction;

#define label_stuAdd_tutor 411
#define input_stuAdd_tutor 412
HWND g_hwndStudentAdd_Tutor_Label;
HWND g_hwndStudentAdd_Tutor;

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

void loadDB()
{
    loadNodes(db_undergraduate, &stu_undergraduate);
    loadNodes(db_postgraduate, &stu_postgraduate);
    table_reload();
}

void saveDB()
{
    saveNodes(db_undergraduate, stu_undergraduate);
    saveNodes(db_postgraduate, stu_postgraduate);
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
            (HMENU)listview_table, NULL, NULL);

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
            (HMENU)button_add, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Edit",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            128, 16, 96, 24,
            hwnd,
            (HMENU)button_edit, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Delete",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            240, 16, 96, 24,
            hwnd,
            (HMENU)button_delete, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Reload database",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            16, 400, 128, 24,
            hwnd,
            (HMENU)button_loadDB, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Save database",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 400, 128, 24,
            hwnd,
            (HMENU)button_saveDB, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "< Prev",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            444, 400, 72, 24,
            hwnd,
            (HMENU)button_prev, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Next >",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            530, 400, 72, 24,
            hwnd,
            (HMENU)button_next, NULL, NULL);

        loadDB();

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
                CW_USEDEFAULT, CW_USEDEFAULT, 270, 300,
                hwnd, NULL, NULL, NULL);
            break;

        case button_edit:
            unsigned long long id = table_getSelectedStudentID();
            if (id)
            {
                g_hwndStudentAdd = CreateWindowEx(
                    0,
                    g_StudentAddClass,
                    "Edit Student",
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                    CW_USEDEFAULT, CW_USEDEFAULT, 270, 300,
                    hwnd, NULL, NULL, &id);
            }
            break;

        case button_delete:
            students_forEach(student_delete, table_getSelectedStudentID());
            table_reload();
            break;

        case button_loadDB:
            loadDB();
            break;

        case button_saveDB:
            saveDB();
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
        g_hwndStudentAdd_Undergraduate = CreateWindowEx(
            0,
            "BUTTON",
            "Undergraduate",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
            8, 8, 120, 24,
            hwnd, (HMENU)radio_stuAdd_undergraduate, NULL, NULL);

        CheckDlgButton(hwnd, radio_stuAdd_undergraduate, BST_CHECKED);

        g_hwndStudentAdd_Postgraduate = CreateWindowEx(
            0,
            "BUTTON",
            "Postgraduate",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            136, 8, 120, 24,
            hwnd, (HMENU)radio_stuAdd_postgraduate, NULL, NULL);

        CreateWindowEx(
            0,
            "STATIC",
            "Name",
            WS_VISIBLE | WS_CHILD,
            8, 40, 56, 20,
            hwnd, (HMENU)label_stuAdd_name, NULL, NULL);

        g_hwndStudentAdd_Name = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            72, 40, 184, 20,
            hwnd, (HMENU)input_stuAdd_name, NULL, NULL);

        CreateWindowEx(
            0,
            "STATIC",
            "Gender",
            WS_VISIBLE | WS_CHILD,
            8, 70, 56, 20,
            hwnd, (HMENU)label_stuAdd_gender, NULL, NULL);

        g_hwndStudentAdd_Male = CreateWindowEx(
            0,
            "BUTTON",
            "Male",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
            72, 70, 72, 20,
            hwnd, (HMENU)radio_stuAdd_gender_male, NULL, NULL);

        g_hwndStudentAdd_Female = CreateWindowEx(
            0,
            "BUTTON",
            "Female",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            152, 70, 72, 20,
            hwnd, (HMENU)radio_stuAdd_gender_female, NULL, NULL);

        CreateWindowEx(
            0,
            "STATIC",
            "Major",
            WS_VISIBLE | WS_CHILD,
            8, 100, 56, 20,
            hwnd, (HMENU)label_stuAdd_major, NULL, NULL);

        g_hwndStudentAdd_Major = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            72, 100, 184, 20,
            hwnd, (HMENU)input_stuAdd_major, NULL, NULL);

        CreateWindowEx(
            0,
            "STATIC",
            "Class",
            WS_VISIBLE | WS_CHILD,
            8, 130, 56, 20,
            hwnd, (HMENU)label_stuAdd_class, NULL, NULL);

        g_hwndStudentAdd_Class = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            72, 130, 184, 20,
            hwnd, (HMENU)input_stuAdd_class, NULL, NULL);

        g_hwndStudentAdd_Direction_Label = CreateWindowEx(
            0,
            "STATIC",
            "Direction",
            WS_CHILD,
            8, 160, 56, 20,
            hwnd, (HMENU)label_stuAdd_direction, NULL, NULL);

        g_hwndStudentAdd_Direction = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_TABSTOP | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            72, 160, 184, 20,
            hwnd, (HMENU)input_stuAdd_direction, NULL, NULL);

        g_hwndStudentAdd_Tutor_Label = CreateWindowEx(
            0,
            "STATIC",
            "Tutor",
            WS_CHILD,
            8, 190, 56, 20,
            hwnd, (HMENU)label_stuAdd_tutor, NULL, NULL);

        g_hwndStudentAdd_Tutor = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_TABSTOP | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            72, 190, 184, 20,
            hwnd, (HMENU)input_stuAdd_tutor, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "Cancel",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 238, 64, 24,
            hwnd,
            (HMENU)IDCANCEL, NULL, NULL);

        CreateWindowEx(
            0,
            "BUTTON",
            "OK",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            192, 238, 64, 24,
            hwnd,
            (HMENU)IDOK, NULL, NULL);

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case radio_stuAdd_undergraduate:
        case radio_stuAdd_postgraduate:
            if (IsDlgButtonChecked(hwnd, radio_stuAdd_postgraduate))
            {
                ShowWindow(g_hwndStudentAdd_Direction_Label, SW_SHOW);
                ShowWindow(g_hwndStudentAdd_Direction, SW_SHOW);
                ShowWindow(g_hwndStudentAdd_Tutor_Label, SW_SHOW);
                ShowWindow(g_hwndStudentAdd_Tutor, SW_SHOW);
            }
            else
            {
                ShowWindow(g_hwndStudentAdd_Direction_Label, SW_HIDE);
                ShowWindow(g_hwndStudentAdd_Direction, SW_HIDE);
                ShowWindow(g_hwndStudentAdd_Tutor_Label, SW_HIDE);
                ShowWindow(g_hwndStudentAdd_Tutor, SW_HIDE);
            }
            break;

        case IDOK:
            int stuType;
            if (IsDlgButtonChecked(hwnd, radio_stuAdd_postgraduate))
            {
                stuType = 2;
            }
            else
            {
                stuType = 1;
            }

            nodeValue stu;

            // name
            GetWindowText(g_hwndStudentAdd_Name, stu.name, 32);

            // gender
            if (IsDlgButtonChecked(hwnd, radio_stuAdd_gender_male))
            {
                stu.gender = 1;
            }
            else if (IsDlgButtonChecked(hwnd, radio_stuAdd_gender_female))
            {
                stu.gender = 2;
            }
            else
            {
                MessageBox(hwnd, "Please select gender!", "", MB_ICONWARNING);
                break;
            }

            // major
            GetWindowText(g_hwndStudentAdd_Major, stu.major, 64);

            // classid
            char classStr[32];
            GetWindowText(g_hwndStudentAdd_Class, classStr, 64);
            sscanf(classStr, "%d", &stu.classid);
            if (stu.classid < 1 || stu.classid > 99)
            {
                MessageBox(hwnd, "Class should be between 1 and 99!", "", MB_ICONWARNING);
                break;
            }

            // direction
            GetWindowText(g_hwndStudentAdd_Direction, stu.direction, 64);
            // tutor
            GetWindowText(g_hwndStudentAdd_Tutor, stu.tutor, 32);

            // generate id
            stu.id = generateStudentID(stuType, stu.major, stu.classid);

            // init marks
            stu.mark_math = -1;
            stu.mark_eng = -1;
            stu.mark_c = -1;
            stu.mark_overall = -1;
            stu.mark_paper = -1;

            if (stuType == 1)
            {
                appendNode(&stu_undergraduate, stu);
                saveNodes(db_undergraduate, stu_undergraduate);
            }
            else
            {
                appendNode(&stu_postgraduate, stu);
                saveNodes(db_postgraduate, stu_postgraduate);
            }

            table_reload();
            ShowWindow(hwnd, SW_HIDE);

            break;

        case IDCANCEL:
            ShowWindow(hwnd, SW_HIDE);
            break;

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
        if (!IsDialogMessage(g_hwndMainWindow, &Msg) && !IsDialogMessage(g_hwndStudentAdd, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    return Msg.wParam;
}