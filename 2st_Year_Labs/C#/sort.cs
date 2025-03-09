using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection.Emit;

namespace Сортировка_файлов
{
    class Program1
    {
        // Метод для инициализации файла случайными числами
        static void InitFile(string path, int n, int MinVal, int MaxVal)
        {
            if (!File.Exists(path))  // Проверка, существует ли файл по указанному пути
            {
                Random rand = new Random();  // Создание экземпляра класса Random для генерации случайных чисел
                using (StreamWriter fs = new StreamWriter(path)) 
                {
                    for (int i = 0; i < n; i++)  // Цикл для записи n случайных чисел в файл
                    {
                        fs.Write(rand.Next(MinVal, MaxVal + 1) + " "); 
                    }
                }
            }
        }

        // Метод для чтения элемента (числа) из файла
        static int ReadElement(StreamReader fs)
        {
            if (fs.Peek() >= 0)  // Проверка, не достигнут ли конец файла
            {
                int curNum = 0; 
                while (true)  
                {
                    int charRead = fs.Read();  
                    if (charRead == -1 || (char)charRead == ' ')  
                    {
                        return curNum;  
                    }
                    char c = (char)charRead;  // Преобразование прочитанного символа в char
                    if (c >= '0' && c <= '9')  // Проверка, является ли символ цифрой
                    {
                        curNum = curNum * 10 + (c - '0');  // Добавление цифры к текущему числу
                    }
                }
            }
            return -1; 
        }

        // Метод для проверки, отсортирован ли файл по возрастанию
        static bool Test(string path)
        {
            StreamReader fs = new StreamReader(path); 
            if (fs.Peek() == -1) return false;  // Проверка, не пуст ли файл

            int n = ReadElement(fs);  // 1
            int x = ReadElement(fs);  // 2
            if (x == -1) return true; 

            while (x != -1)  // Цикл для проверки всех элементов файла
            {
                if (x < n){ 
                    Console.WriteLine("ne otsort.");
                    return false;  // Если текущий элемент меньше предыдущего, файл не отсортирован
                            }
                else {
                    n = x;  // Обновление предыдущего элемента
                    x = ReadElement(fs);  // Чтение следующего элемента
                }
            }
            Console.WriteLine("otsort.");
            return true;  
        }

        // Метод для сортировки файла
        static void FileSort(string file)
        {
            int step = 1;  // Инициализация шага для сортировки
            StreamReader fs = new StreamReader(file); 
            StreamWriter streamWriter1 = new StreamWriter("fa.txt");  
            int x = ReadElement(fs);  // 1 isxod

            while (x != -1)  // Цикл для копирования всех элементов в временный файл
            {
                streamWriter1.Write(x + " ");  
                x = ReadElement(fs);  
            }
            streamWriter1.Close();  
            bool flag1 = true;  // Флаг для продолжения сортировки
            bool flag2 = false;  // Флаг для чередования временных файлов

            while (flag1)  
            {
                if (!flag2)  // Если flag2 ложен, чтение из fa.txt и запись в fb.txt и fc.txt
                {
                    StreamReader fs0 = new StreamReader("fa.txt");  
                    StreamWriter writer1 = new StreamWriter("fb.txt");  
                    StreamWriter writer2 = new StreamWriter("fc.txt");  

                    int x1 = ReadElement(fs0);  // 1 из  fa.txt
                    int n = 0;  // Инициализация переменной для чередования записи в fb.txt и fc.txt

                    while (x1 != -1)  // Цикл для чтения всех элементов из fa.txt
                    {
                        int i = 0; 

                        while (x1 != -1 && i < step)  // Цикл для записи элементов с текущим шагом ( fb,fc, n -> fa )
                        {
                            if (n == 0) writer1.Write(x1 + " ");  
                            else writer2.Write(x1 + " ");  
                            x1 = ReadElement(fs0);  
                            i += 1;  
                        }
                        n = 1 - n;  // fb swap fc
                    }
                    fs0.Close();  //  fa.txt
                    writer1.Close();  // fb.txt
                    writer2.Close();  // fc.txt
                    flag2 = true;  // Установка flag2 в true для следующего этапа сортировки
                }
                else { // 
                    StreamReader fs0 = new StreamReader("fb.txt");  
                    StreamReader fs1 = new StreamReader("fc.txt");  
                    StreamWriter writer1 = new StreamWriter("fa.txt");  

                    if (fs1.Peek() == -1)  // Проверка, пуст ли fc.txt
                    {
                        flag1 = false;  // Если пуст, завершение сортировки
                        break;
                    }
                    else {
                        int x1 = ReadElement(fs0);  // 1 из fb.txt
                        int x2 = ReadElement(fs1);  // 1 из fc.txt

                        while (x1 != -1 && x2 != -1)  // Цикл для сравнения и слияния элементов
                        {
                            int i = 0;  //  fb.txt
                            int j = 0;  //  fc.txt

                            while (x1 != -1 && x2 != -1 && i < step && j < step)  // Цикл для сравнения и записи элементов
                            {
                                if (x1 < x2)  // ( fb < fc )
                                {
                                    writer1.Write(x1 + " ");  // ( fb -> fa )
                                    x1 = ReadElement(fs0);  
                                    i += 1;  
                                }
                                else
                                {
                                    writer1.Write(x2 + " ");  //  ( fc -> fa)
                                    x2 = ReadElement(fs1); 
                                    j += 1;  
                                }
                            }
                            while (x1 != -1 && i < step)  //  ( fb -> fa )
                            {
                                writer1.Write(x1 + " ");  
                                x1 = ReadElement(fs0);  
                                i += 1; 
                            }
                            while (x2 != -1 && j < step)  //  ( fc -> fa )
                            {
                                writer1.Write(x2 + " ");  
                                x2 = ReadElement(fs1);  
                                j += 1;  
                            }
                        }
                        while (x1 != -1)  // Запись оставшихся элементов из fb.txt ( fb -> fa )
                        {
                            writer1.Write(x1 + " ");  
                            x1 = ReadElement(fs0);  
                        }
                        while (x2 != -1)  // оставшиеся из fc.txt ( fc -> fa )
                        {
                            writer1.Write(x2 + " ");  
                            x2 = ReadElement(fs1);  
                        }
                    }
                    fs0.Close(); 
                    fs1.Close();  
                    writer1.Close();  
                    flag2 = false;  // Установка flag2 в false для следующего этапа сортировки
                    step *= 2;  
                }
            }
        }

        // Метод для чтения и вывода содержимого файла
        static void PrintFile(string path)
        {
            using (StreamReader fs = new StreamReader(path))  // Создание StreamReader для чтения из файла
            {
                string content = fs.ReadToEnd();  
                Console.WriteLine(content);  
            }
        }

       
        static void Main(string[] args)
        {
            Console.WriteLine("kol-vo el: "); 
            int n = int.Parse(Console.ReadLine()); 

            Console.WriteLine("name file");  
            string file = Console.ReadLine() + ".txt";  
            InitFile(file, n, 1, 100); 
            
            FileSort(file);  // Сортировка файла
            Test("fb.txt");
        }
    }
}
