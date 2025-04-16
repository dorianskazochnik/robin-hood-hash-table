#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <exception>
#include <chrono>
#include <iterator>
#include <random>

using namespace std;

template <class Ty>
class Hash
{
private:
    Ty val;
public:
    Hash(const Ty& v = NULL) : val(v) {}
    size_t operator()(const Hash<Ty>& s) const noexcept
    {
        size_t h = hash<Ty>{}(s.val);
        return h;
    }
    bool operator != (const Hash& right) const
    {
        return val != right.val;
    }
    bool operator == (const Hash& right) const
    {
        return val == right.val;
    }
    Hash& operator = (const Hash& right)
    {
        if (this != &right)
            val = right.val;
        return *this;
    }
};

template <class T>
class RobinHoodHashTable
{
private:
    vector<Hash<T> > table;
    size_t size;
    int collisions;
    int find_count;
    int insert_count;
    int delete_count;
public:
    RobinHoodHashTable(size_t size) : size(size), collisions(0), find_count(0), insert_count(0), delete_count(0)
    {
        if (size == 0)
            throw runtime_error("Wrong size");
        table.vector<Hash<T> >::resize(size);
        for (auto it : table)
            it = Hash<T>();
    }

    // Метод для поиска значения в хеш-таблице
    size_t find(Hash<T> val)
    {
        size_t key = Hash<T>{}(val);
        find_count++; // Увеличиваем счетчик операций поиска
        size_t index = key % size; // Вычисляем индекс ячейки
        size_t distance = 0; // Расстояние до искомого значения
        while (table[index] != Hash<T>() && Hash<T>{}(table[index]) != key && distance <= size)
        {
            // Продолжаем искать, пока не найдем ячейку с искомым значением или пустую ячейку
            index = (index + 1) % size; // Переходим к следующей ячейке
            distance++;
        }
        if (distance > size)
            return -1;
        return (Hash<T>{}(table[index]) == key) ? index : -1;
    }

    // Метод для вставки значения в хеш-таблицу
    void insert(Hash<T> val)
    {
        size_t key = Hash<T>{}(val);
        insert_count++; // Увеличиваем счетчик операций вставки
        size_t index = key % size; // Вычисляем индекс ячейки
        size_t distance = 0; // Расстояние до текущей ячейки
        while (table[index] != Hash<T>() && Hash<T>{}(table[index]) != key)
        {
            if (distance > Hash<T>{}(table[index]) - index)
            {
                // Robin Hood: обменять текущее значение с новым, если новое значение имеет меньшее расстояние
                swap(table[index], val);
                distance = Hash<T>{}(table[index]) - index;
            }
            index = (index + 1) % size; // Переходим к следующей ячейке
            distance++;
            collisions++; // Увеличиваем счетчик коллизий
        }
        // Вставляем новое значение в пустую ячейку
        swap(table[index], val);
    }

    // Метод для удаления значения из хеш-таблицы
    void remove(Hash<T> val)
    {
        size_t index = find(val); // Находим индекс ячейки с искомым ключом
        delete_count += index != size_t(-1) ? 1 : 0; // Увеличиваем счетчик операций удаления
        table[index * (index != size_t(-1))] = (index != size_t(-1)) ? Hash<T>() : table[index * (index != size_t(-1))];
    }

    // Метод для вычисления коэффициента заполнения таблицы
    double loadFactor() const
    {
        size_t count = 0;
        for (auto it : table)
            count += int(it != Hash<T>());
        return static_cast<double>(count) / (size - count);
    }

    int getCollisions() const
    {
        return collisions;
    }

    int getFindCount() const
    {
        return find_count;
    }

    int getInsertCount() const
    {
        return insert_count;
    }

    int getDeleteCount() const
    {
        return delete_count;
    }

    size_t getSize() const
    {
        return size;
    }

    // Метод для сохранения статистики в файл
    void save(const string& filename) const
    {
        ofstream file(filename);
        if (!file)
        {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        file << "Find operations: " << getFindCount() << endl;
        file << "Insert operations: " << getInsertCount() << endl;
        file << "Delete operations: " << getDeleteCount() << endl;
        file << "Collisions: " << getCollisions() << endl;
        file << "Load factor: " << loadFactor() << endl;

        file.close();
        cout << "Statistics saved to file: " << filename << endl;
    }
};

void test1()
{
    // Создаем хеш-таблицу размером 10
    RobinHoodHashTable<const char*> hashTable(10);
    // Вставляем некоторые ключи
    hashTable.insert("test 1");
    hashTable.insert("meow");
    hashTable.insert("second");
    hashTable.insert("7");
    // Поиск ключей
    size_t index1 = hashTable.find("test 1");
    size_t index2 = hashTable.find("7");
    size_t index3 = hashTable.find("first");
    switch (index1)
    {
    case(-1):
        cout << "Key test 1 not found" << endl;
        break;
    default:
        cout << "Key test 1 found at index: " << index1 << endl;
        break;
    }
    switch (index2)
    {
    case(-1):
        cout << "Key 7 not found" << endl;
        break;
    default:
        cout << "Key 7 found at index: " << index2 << endl;
        break;
    }
    switch (index3)
    {
    case(-1):
        cout << "Key first not found" << endl;
        break;
    default:
        cout << "Key first found at index: " << index3 << endl;
        break;
    }
    // Удаление ключа
    hashTable.remove("7");
    // Сохранение статистики в файл
    hashTable.save("statistics1.txt");
}

void test2()
{
    // Создаем хеш-таблицу размером 10
    RobinHoodHashTable<int> hashTable(10);
    // Вставляем некоторые ключи
    hashTable.insert(5);
    hashTable.insert(15);
    hashTable.insert(25);
    hashTable.insert(7);
    // Поиск ключей
    size_t index1 = hashTable.find(15);
    size_t index2 = hashTable.find(7);
    size_t index3 = hashTable.find(10);
    switch (index1)
    {
    case(-1):
        cout << "Key 15 not found" << endl;
        break;
    default:
        cout << "Key 15 found at index: " << index1 << endl;
        break;
    }
    switch (index2)
    {
    case(-1):
        cout << "Key 7 not found" << endl;
        break;
    default:
        cout << "Key 7 found at index: " << index2 << endl;
        break;
    }
    switch (index3)
    {
    case(-1):
        cout << "Key 10 not found" << endl;
        break;
    default:
        cout << "Key 10 found at index: " << index3 << endl;
        break;
    }
    // Удаление ключа
    hashTable.remove(15);
    // Сохранение статистики в файл
    hashTable.save("statistics2.txt");
}

class TestClass
{
private:
    int a;
    double f;
public:
    double getSum() const { return a + f; }
    TestClass(int _a = 0, double _f = 0) : a(_a), f(_f) {}
    inline friend ostream& operator << (ostream& stream, const TestClass& t)
    {
        stream << t.a << t.f;
        return stream;
    }
    bool operator != (const TestClass& right) const
    {
        return a != right.a && f != right.f;
    }
    bool operator == (const TestClass& right) const
    {
        return a == right.a && f == right.f;
    }
    TestClass& operator = (const TestClass& right)
    {
        if (this == &right)
            return *this;
        a = right.a;
        f = right.f;
        return *this;
    }
};

namespace std {
    template <> struct hash<TestClass>
    {
        size_t operator()(const TestClass& x) const
        {
            return hash<int>()(x.getSum());
        }
    };
}

void test3()
{
    // Создаем хеш-таблицу размером 10
    RobinHoodHashTable<TestClass> hashTable(10);
    TestClass s1(2, 22.2);
    TestClass s2(0, 3.9);
    TestClass s3(4);
    TestClass s4(0);
    TestClass s(5, 22.2);
    // Вставляем некоторые ключи
    hashTable.insert(s1);
    hashTable.insert(s2);
    hashTable.insert(s3);
    hashTable.insert(s4);
    // Поиск ключей
    size_t index1 = hashTable.find(s2);
    size_t index2 = hashTable.find(s3);
    size_t index3 = hashTable.find(s);
    switch (index1)
    {
    case(-1):
        cout << "Key 3.9 not found" << endl;
        break;
    default:
        cout << "Key 3.9 found at index: " << index1 << endl;
        break;
    }
    switch (index2)
    {
    case(-1):
        cout << "Key 4 not found" << endl;
        break;
    default:
        cout << "Key 4 found at index: " << index2 << endl;
        break;
    }
    switch (index3)
    {
    case(-1):
        cout << "Key 5, 22.2 not found" << endl;
        break;
    default:
        cout << "Key 5, 22.2 found at index: " << index3 << endl;
        break;
    }
    // Удаление ключа
    hashTable.remove(s1);
    // Сохранение статистики в файл
    hashTable.save("statistics3.txt");
}

void test_for_graphics(int a)
{
    //a - кол-во действий
    RobinHoodHashTable<int> hashTable(a);
    srand(a);
    //цикл, который на рандоме делает ту или иную операцию в хэш таблице
    for (int i = 0; i < a; i++)
    {
        int what_to_do = 1 + rand() % 3;
        switch (what_to_do)
        {
        case 1: // insert 
            hashTable.insert(rand() % a);
            break;
        case 2: // remove 
            hashTable.remove(rand() % a);
            break;
        case 3: //find 
            hashTable.find(rand() % a);
            break;
        default:
            break;
        }
    }
    hashTable.save("statistics_for_graphics.txt");
    //вероятности
}

void test_time(int a)
{
    //a - кол-во элементов
    RobinHoodHashTable<int> hashTable(a);

    // Предварительно заполняем таблицу на 0.7
    for (int i = 0; hashTable.loadFactor() <= 0.7; i++)
        hashTable.insert(i * (-1));
    int c = hashTable.getInsertCount();

    srand(a);

    // Тест скорости операции find. a * 0.1 - формальное ограничение количества операций
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < a * 0.1; i++)
        hashTable.find(rand() % a);
    auto end = chrono::high_resolution_clock::now();
    chrono::microseconds elapsedTime2 = chrono::duration_cast<chrono::microseconds>(end - start);
    int f = hashTable.getFindCount();

    // Тест скорости операции insert. a - ограничение количества операций, чтобы не уйти в бесконечный цикл из-за рандомных значений
    start = chrono::high_resolution_clock::now();
    for (int i = 0; hashTable.loadFactor() <= 0.8; i++)
        hashTable.insert(rand() % a);
    end = chrono::high_resolution_clock::now();
    chrono::microseconds elapsedTime3 = chrono::duration_cast<chrono::microseconds>(end - start);

    // Тест скорости операции remove. a - ограничение количества операций, чтобы не уйти в бесконечный цикл из-за рандомных значений
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < a && hashTable.loadFactor() >= 0.7; i++)
        hashTable.remove(rand() % a);
    end = chrono::high_resolution_clock::now();
    chrono::microseconds elapsedTime1 = chrono::duration_cast<chrono::microseconds>(end - start);

    // Сохраняем статистику в файл
    ofstream file("statistics_time.txt");
    if (!file)
    {
        cerr << "Error opening file: " << "statistics_time.txt" << endl;
        return;
    }
    file << "Load Factor: " << hashTable.loadFactor() << endl;
    file << "Find operations: " << elapsedTime2.count() << " microseconds, operation count " << f << endl;
    file << "Insert operations: " << elapsedTime3.count() << " microseconds, operation count " << hashTable.getInsertCount() - c << endl;
    file << "Delete operations: " << elapsedTime1.count() << " microseconds, successful operation count " << hashTable.getDeleteCount() << " of " << hashTable.getFindCount() - f << endl;
    file.close();
    cout << "Statistics saved to file: " << "statistics_time.txt" << endl;
}

void test_for_graphics1(double fact)
{
    RobinHoodHashTable<int> hashTable(1000);
    srand(1000);
    int a = 1000 * fact;
    for (int i = 0; i < a; i++)
    {
        hashTable.insert(rand() % 1000);
    }
    hashTable.save("statistics_for_graphics1.txt");
    //вероятности
}

int main()
{
    //проверки
    test1(); //проверка на const char*
    test2(); //проверка на int
    test3(); //проверка на пользовательском классе
    test_for_graphics1(1); //проверка для графиков на более больших таблицах
    test_time(100);
    return 0;
}