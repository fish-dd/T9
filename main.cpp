//g++ main.cpp -o main.exe -L C:\\Python312\\libs -lpython312 -I C:\\Python312\\include
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
#include <Python.h>
//#include </usr/include/python3.12/pyconfig-64.h>

using namespace std;

PyObject *funcs_file = NULL, *funcs = NULL;
PyObject *funcs_dict = NULL, *pObjct = NULL, *pVal = NULL;
PyObject* sys = NULL;
PyObject* sys_path = NULL;
PyObject* folder_path = NULL;

PyObject *python_init() {
    // Инициализировать интерпретатор Python
    Py_Initialize();
    //PyRun_SimpleString("import serial\nimport keyboard\nSerial = serial.Serial('/dev/ttyUSB0', 9600)");

    do {
        // Загрузка модуля sys
        sys = PyImport_ImportModule("sys");
        sys_path = PyObject_GetAttrString(sys, "path");
        // Путь до наших исходников Python
        folder_path = PyUnicode_FromString((const char*) "./");
        PyList_Append(sys_path, folder_path);

        // Загрузка functions.py
        funcs_file = PyUnicode_FromString("functions");
        if (!funcs_file) {
            break;
        }

        // Загрузить объект модуля
        funcs = PyImport_Import(funcs_file);
        if (!funcs) {
            break;
        }

        // Словарь объектов содержащихся в модуле
        funcs_dict = PyModule_GetDict(funcs);
        if (!funcs_dict) {
            break;
        }

        return funcs_dict;

    } while (0);

    // Печать ошибки
    PyErr_Print();
}

void writing(int clear, const char *word) {
    // Загрузка объекта get_value из func.py
    pObjct = PyDict_GetItemString(funcs_dict, (const char *) "writing");
    if (!pObjct) {
        return;
    }

    do {
        // Проверка pObjct на годность.
        if (!PyCallable_Check(pObjct)) {
            break;
        }

        PyObject_CallFunction(pObjct, (char *) "(is)", clear, word);
    } while (0);

    return;
}

char *reading() {
    char *ret = NULL;

    // Загрузка объекта get_value из func.py
    pObjct = PyDict_GetItemString(funcs_dict, (const char *) "reading");
    /*if (!pObjct) {
        return ret;
    }*/

    do {
        // Проверка pObjct на годность.
        if (!PyCallable_Check(pObjct)) {
            break;
        }

        pVal = PyObject_CallFunction(pObjct, (const char *) "()");
        if (pVal != NULL) {
            PyObject* pResultRepr = PyObject_Repr(pVal);

            // Если полученную строку не скопировать, то после очистки ресурсов Python её не будет.
            // Для начала pResultRepr нужно привести к массиву байтов.
            ret = strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));

            Py_XDECREF(pResultRepr);
            Py_XDECREF(pVal);
        } else {
            PyErr_Print();
        }
    } while (0);

    return ret;
}

void python_clear() {
    PyRun_SimpleString("Serial.close()");
    // Вернуть ресурсы системе
    Py_XDECREF(funcs_dict);
    Py_XDECREF(funcs);
    Py_XDECREF(funcs_file);

    Py_XDECREF(folder_path);
    Py_XDECREF(sys_path);
    Py_XDECREF(sys);

    // Выгрузка интерпретатора Python
    Py_Finalize();
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "RUS");
    python_init();

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
    printf("Loading dicts...\n");
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
    printf("Loading dicts complete.\n");

    string input;
    string pnput;
    char old_response[1024];
    memset(old_response, '\0', sizeof old_response);
    bool run = true;
    int index = 0;
    vector<string> words;
    map<string, int> is_ltr{
        {"", 0},
        {"1", 1},
        {"2", 1},
        {"3", 1},
        {"4", 1},
        {"5", 1},
        {"6", 1},
        {"7", 1},
        {"8", 1},
        {"9", 1},
        {"10", 0},
        {"11", 0},
        {"12", 0},
        {"13", 0}
    };
    //char ltrs[input.size() + 1];

    while (run) {
        char response[1024];
        strcpy(response, reading());
        //response[16] = '\000';
        //cout << response << endl;

        if (string(old_response).find('1') == string(response).find('1')) {
            //nothing
            //cout << response << endl;
        }
        else if (string(response).find('1') < 100) {
            string input = response;
            int raw_key = input.find('1');
            if (raw_key > 100) {
                raw_key = 0;
            }
            string key = key_binds[raw_key];

            if (is_ltr[key]){
                //cout << "not key 10!" << key << endl;
                pnput += key;
                int index = 0;
                words = {};
                
                for (int i = 0; i < ptrn_ru_array.size(); i++) {
                    if ((ptrn_ru_array[i].rfind(pnput, 0) == 0) and (ptrn_ru_array[i].size() == pnput.size())) {
                        words.push_back(dict_ru_array[i]);
                        //cout << dict_ru_array[i] << ptrn_ru_array[i] << endl;
                    }
                }
                if (words.size() > 0) {
                    cout << words[index] << endl;
                    if (pnput.size() > 1) {
                        writing(1, words[index].c_str());
                    }
                    else {
                        writing(0, words[index].c_str());
                    }
                }
                //cout << "not key 10!" << endl;
            }
            else if (key == "10") {
                //cout << "key 10!" << words.size() << endl;
                if ((index + 1) < words.size()) {
                    //cout << "1" << endl;
                    index++;
                }
                else {
                    //cout << "2" << endl;
                    int index = 0;
                }
                if (words.size() > 0) {
                    //cout << "3" << endl;
                    cout << words[index] << endl;
                    writing(1, words[index].c_str());
                }
                //cout << "key 10!" << endl;
            }
            else if (key == "13") {
                if (pnput.size() > 0) {
                    pnput.pop_back();
                    int index = 0;
                    words = {};
                    for (int i = 0; i < ptrn_ru_array.size(); i++) {
                        if ((ptrn_ru_array[i].rfind(pnput, 0) == 0) and (ptrn_ru_array[i].size() == pnput.size())) {
                            words.push_back(dict_ru_array[i]);
                            //cout << dict_ru_array[i] << ptrn_ru_array[i] << endl;
                        }
                    }
                    cout << endl;
                }
                    if (pnput.size() == 0) {
                        writing(1, "");
                    }
                    else {
                        writing(1, words[index].c_str());
                    }
            }
            else if (key == "11") {
                pnput = "";
                int index = 0;
                words = {};
                writing(0, " ");
            }
        }
        strcpy(old_response, response);
    }
    python_clear();
    return 0;
}
