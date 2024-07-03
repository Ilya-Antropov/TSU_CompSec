 using System;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Enter array elements separated by spaces:");

        string input = Console.ReadLine();

        string[] numbers = input.Split(' ');

        int[] array = new int[numbers.Length];

        for (int i = 0; i < numbers.Length; i++)
        {
            array[i] = int.Parse(numbers[i]);
        }
        
        int maxLength = 0;
        int maxSequenceStartIndex = 0;
        int currentLength = 0;
        int sequenceStartIndex = 0;

        for (int i = 1; i < array.Length; i++)
        {
            if (array[i] > array[i - 1])
            {
                currentLength++;
                if (currentLength >= maxLength)
                {
                    maxLength = currentLength;
                    maxSequenceStartIndex = sequenceStartIndex;
                }
            }
            else
            {
                currentLength = 0;
                sequenceStartIndex = i;
            }
        }
        int sequenceEndIndex = maxSequenceStartIndex + maxLength;
        Console.WriteLine("Start and end indices of the longest ascending sequence:");
        Console.WriteLine($"Start index: {maxSequenceStartIndex}");
        Console.WriteLine($"End index: {sequenceEndIndex}");
        Console.WriteLine("Longest ascending sequence:");
        for (int i = maxSequenceStartIndex; i <= sequenceEndIndex; i++)
        {
            Console.Write($"{array[i]} ");
        }
    }
}


/* Найти в массиве самую длинную возрастающую последовательность, расположенную
 после максимального элемента включительно. Вывести на экран номера ее первого и последнего элементов */
 
 
 
