#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/*
RAII - захват ресурса есть инициализация. При инициализации объекта происходит захват ресурса (файла).
При удалении объекта - ресурс освобождается. Исключается копирование объекта.
*/

class FileHandler {
private:
    string fpath;
    fstream* fs = 0;
    
    FileHandler(FileHandler& fh) = delete;  //Удаление копирующего конструктора    
    FileHandler& operator=(FileHandler& fh) = delete;   //Удаление копирующего оператора присваивания   

public:
    class FileException{
    private:
        string errmsg;
    public:
        FileException(string msg) :errmsg(msg) {}
        string what() {
            return errmsg;
        }
    };

    //Конструктор по умолчанию
    FileHandler(string path) : fpath(path) {
        try{
            fs = new fstream();
            fs->open(fpath, ios_base::in | ios_base::app);
            if (!fs->is_open()) {
                throw FileException("\n--File opening error!\n");
            }
            cout << "File " << fpath << " has been opened" << endl;
            
        }
        catch (FileException& ex) {
            cerr << ex.what();
        }

        cout << "[" << this << "]" << "DEFAULT constructor has been called" << endl;
    }

    //Перемещающий конструктор
    FileHandler(FileHandler&& other) noexcept {

        if (&other == this)
            return;

        fpath = other.fpath;
        fs = other.fs;

        other.fpath.clear();
        other.fs = nullptr;

        cout << "[" << this << "]" <<"MOVE constructor has been called" << endl;
    }

    //Перемещающий оператор присваивания
    FileHandler& operator=(FileHandler&& other) noexcept {
        
        if (&other == this)
            return *this;

        fpath = other.fpath;
        fs = other.fs;

        other.fpath.clear();
        other.fs = nullptr;

        cout << "[" << this << "]" << "Moving OPERATOR = has been called" << endl;
    }

    void writeln(const string& str) {
        try {
            if (fs->is_open()) {
                fs->clear();
                fs->flush();
                *fs << str << endl;
            }
            else
                throw FileException("\n--Error writing file. Probably file is not open!\n");
        }
        catch (FileException& ex) {
            cerr << ex.what();
        }
    }

    void read_all() {
        string str;
        if (fs == nullptr) {
            cout << "Object has no initialized fstream!" << endl;
            return;
        }
        try {
            if (fs->is_open()) {
                fs->clear();
                fs->flush();
                fs->seekg(0, ios::beg);
                while (!fs->eof()) {
                    getline(*fs, str);
                    cout << str << endl;
                    str.clear();
                }  
                /*cout << "--INFO: good: " << fs->good() << ", bad: " << fs->bad() << ", fail: " 
                    << fs->fail() << ", rdstate:" << fs->rdstate() << endl; */
            }
            else
                throw FileException("\n--Error reading file. Probably file is not open!\n");
        }
        catch (FileException& ex) {
            cerr << ex.what();
        }
    }

    //Деструктор
    ~FileHandler() noexcept {
        cout << "[" << this << "]" << "DESTRUCTOR has been called: ";
        if (fs != nullptr) {
            fs->close();
            delete fs;
            cout << "file pointer isn't nullptr: ";
        }
        cout << "Obj deleted" << endl;
    }
};

int main()
{
    //Создаем объект и захватываем ресурс, читаем данные из файла и записываем в него строку
    FileHandler fh("read.txt");
    fh.read_all();
    fh.writeln("Hello, my friend!");

    cout << endl << "--Trying move the object" << endl;

    //Создаем другой объект и перемещаем в него данные первого
    FileHandler fm = move(fh);

    //Пытаемся получить доступ к файлу через первый объект
    fh.read_all();

    //Читаем файл через второй объект, записываем в него строку и снова читаем
    fm.read_all();
    fm.writeln("I'm writing new string through a new object!");
    fm.read_all();

    //Возвращаем право владения файлом првому объекту через перегруженный оператор =
    fh = move(fm);

    //Пробуем читать файл через второй объект
    fm.read_all();

    //Пробуем прочитать файл через первый объект
    fh.read_all();

    system("pause>nul");
    return 0;
}
