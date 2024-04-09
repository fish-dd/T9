#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

int main() {
    int USB = open( "/dev/ttyUSB0", O_RDWR | O_NOCTTY );

    Display *dpy;
    Window win;
    GC gc;
    int scr;
    Atom WM_DELETE_WINDOW;
    XEvent ev;
    XEvent ev2;
    KeySym keysym;
    int loop;

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fputs("Cannot open display", stderr);
        exit(1);
    }
    scr = XDefaultScreen(dpy);

    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr(USB, &tty) != 0 ) {
        std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    tty_old = tty;

    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    cfmakeraw(&tty);

    /* Flush Port, then applies attributes */
    tcflush( USB, TCIFLUSH );
    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
    std::cout << "Error " << errno << " from tcsetattr" << std::endl;
    }

    cfsetospeed (&tty, (speed_t)B9600);
    cfsetispeed (&tty, (speed_t)B9600);

    map<char, string> table_ru{
        {'а', "2"},
        {'б', "2"},
        {'в', "2"},
        {'г', "2"},
        {'д', "3"},
        {'е', "3"},
        {'ё', "3"},
        {'ж', "3"},
        {'з', "3"},
        {'и', "4"},
        {'й', "4"},
        {'к', "4"},
        {'л', "4"},
        {'м', "5"},
        {'н', "5"},
        {'о', "5"},
        {'п', "5"},
        {'р', "6"},
        {'с', "6"},
        {'т', "6"},
        {'у', "6"},
        {'ф', "7"},
        {'х', "7"},
        {'ц', "7"},
        {'ч', "7"},
        {'ш', "8"},
        {'щ', "8"},
        {'ъ', "8"},
        {'ы', "8"},
        {'ь', "9"},
        {'э', "9"},
        {'ю', "9"},
        {'я', "9"},
        {'А', "2"},
        {'Б', "2"},
        {'В', "2"},
        {'Г', "2"},
        {'Д', "3"},
        {'Е', "3"},
        {'Ё', "3"},
        {'Ж', "3"},
        {'З', "3"},
        {'И', "4"},
        {'Й', "4"},
        {'К', "4"},
        {'Л', "4"},
        {'М', "5"},
        {'Н', "5"},
        {'О', "5"},
        {'П', "5"},
        {'Р', "6"},
        {'С', "6"},
        {'Т', "6"},
        {'У', "6"},
        {'Ф', "7"},
        {'Х', "7"},
        {'Ц', "7"},
        {'Ч', "7"},
        {'Ш', "8"},
        {'Щ', "8"},
        {'Ъ', "8"},
        {'Ы', "8"},
        {'Ь', "9"},
        {'Э', "9"},
        {'Ю', "9"},
        {'Я', "9"},
        {'-', "1"}
    };
    map<int, string> key_binds{
        {12, "1"},
        {10, "2"},
        {11, "3"},
        {6, "4"},
        {3, "5"},
        {7, "6"},
        {5, "7"},
        {1, "8"},
        {2, "9"},
        {4, "10"},
        {8, "11"},
        {9, "12"},
        {14, "13"},
        {0, ""}
    };
    vector<string> dict_ru_array;
    vector<string> ptrn_ru_array;

    ifstream dict;
    ifstream pattern;

    string dict_ru;
    string pattern_ru;

    dict.open("ru-utf8.txt" , ios::binary | ios::in);
    pattern.open("pattern_ru.txt" , ios::binary | ios::in);

    string line;
    cout << "Загрузка словарей..." << endl;
    while (getline(dict, line)) {
        dict_ru += line;
        dict_ru += "\n";
        dict_ru_array.push_back(line);
    }
    while (getline(pattern, line)) {
        pattern_ru += line;
        pattern_ru += "\n";
        ptrn_ru_array.push_back(line);
    }
    dict_ru.pop_back();
    pattern_ru.pop_back();
    cout << "Загрузка словарей завершена." << endl;

    string input;
    string pnput;
    char old_response[1024];
    memset(old_response, '\0', sizeof old_response);
    //char ltrs[input.size() + 1];

    while (true) {
        int n = 0,
            spot = 0;
        char buf = '\0';
        /* Whole response*/
        char response[1024];
        memset(response, '\0', sizeof response);

        do {
            n = read( USB, &buf, 1 );
            sprintf( &response[spot], "%c", buf );
            spot += n;
        } while( buf != '\r' && n > 0);

        if (n < 0 or n == 0) {
            cout << "Error reading: " << strerror(errno) << endl;
            //cout << response << endl;
        } 
        else {
            if (string(old_response).find("1") == string(response).find("1")) {
                //nothing
            } 
            else {
                string input = response;
                //cout << input << old_response << endl;
                input.pop_back();
                int raw_key = input.find("1");
                if (raw_key > 100) {
                    raw_key = 0;
                };
                string key = key_binds[raw_key];
                cout << key << endl;
                //char ltrs[input.size() + 1];
                //strcpy(old_response, input.c_str());

                if ((key != "1") or (key != "10") or (key != "11") or (key != "12") or (key != "13")){
                    pnput += key;
                };
                
                for (int i = 0; i < ptrn_ru_array.size(); i++) {
                    if ((ptrn_ru_array[i].rfind(pnput, 0) == 0) and (ptrn_ru_array[i].size() == pnput.size())) {
                        cout << ptrn_ru_array[i]<< " " << dict_ru_array[i] << endl;
                    }
                }
            }
        }
        //char old_response[1024];
        strcpy(old_response, response);
        //cout << old_response << endl;
    }

    //cout << dict_ru_array.size() << endl;

    return 0;
}
