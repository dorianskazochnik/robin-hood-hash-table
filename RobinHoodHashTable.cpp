#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <string>

using namespace std;

//название класса поменять.
template <class Ty>
class Hash
{
public:
    Ty val;
    Hash()
    {
        val = NULL;
    }
    Hash(const Ty& v)
    {
        val = v;
    }
    size_t operator()(const Hash<Ty>& s) noexcept
    {
        size_t h = hash<Ty>{}(s.val);
        return h;
    }
    bool operator != (const Hash& right)
    {
        return val != right.val;
    }
    bool operator == (const Hash& right)
    {
        return val == right.val;
    }
    Hash& operator = (const Hash& right)
    {
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
        table.vector<Hash<T> >::resize(size);
        for (size_t i = 0; i < size; i++)
        {
            table[i] = Hash<T>();
        }
    }

    // Метод для поиска значения в хеш-таблице
    size_t find(Hash<T> val)
    {
        size_t key = Hash<T>{}(val);
        find_count++; // Увеличиваем счетчик операций поиска

        size_t index = key % size; // Вычисляем индекс ячейки
        size_t distance = 0; // Расстояние до искомого значения

        while (table[index] != Hash<T>() && Hash<T>{}(table[index]) != key)
        {
            // Продолжаем искать, пока не найдем ячейку с искомым значением или пустую ячейку
            index = (index + 1) % size; // Переходим к следующей ячейке
            distance++;

            if (distance > size)
            {
                // Если превышено максимальное расстояние, значит значение отсутствует в таблице
                return -1;
            }
        }

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
        delete_count++; // Увеличиваем счетчик операций удаления

        size_t index = find(val); // Находим индекс ячейки с искомым ключом

        if (index != -1)
        {
            table[index] = Hash<T>(); // Устанавливаем значение NULL, чтобы указать, что ячейка пустая
        }
    }

    // Метод для вычисления коэффициента заполнения таблицы
    double loadFactor()
    {
        size_t count = 0;

        for (size_t i = 0; i < size; i++)
        {
            count += int(table[i] != Hash<T>());
        }

        return static_cast<double>(count) / size;
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

    // Метод для сохранения статистики в файл
    void save(const string& filename)
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


//string
int main()
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

    if (index1 != -1)
    {
        cout << "Key test 1 found at index: " << index1 << endl;
    }
    else
    {
        cout << "Key test 1 not found" << endl;
    }

    if (index2 != -1)
    {
        cout << "Key 7 found at index: " << index2 << endl;
    }
    else
    {
        cout << "Key 7 not found" << endl;
    }

    if (index3 != -1)
    {
        cout << "Key first found at index: " << index3 << endl;
    }
    else
    {
        cout << "Key first not found" << endl;
    }

    // Удаление ключа
    hashTable.remove("7");

    // Сохранение статистики в файл
    hashTable.save("statistics.txt");

    return 0;
}

/*
//int
int main()
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

    if (index1 != -1)
    {
        cout << "Key 15 found at index: " << index1 << endl;
    }
    else
    {
        cout << "Key 15 not found" << endl;
    }

    if (index2 != -1)
    {
        cout << "Key 7 found at index: " << index2 << endl;
    }
    else
    {
        cout << "Key 7 not found" << endl;
    }

    if (index3 != -1)
    {
        cout << "Key 10 found at index: " << index3 << endl;
    }
    else
    {
        cout << "Key 10 not found" << endl;
    }

    // Удаление ключа
    hashTable.remove(15);

    // Сохранение статистики в файл
    hashTable.save("statistics.txt");

    return 0;
}
*/