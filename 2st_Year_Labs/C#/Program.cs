using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("Enter the number of matrix rows:");
        int rows = int.Parse(Console.ReadLine());

        Console.WriteLine("Enter the number of matrix columns:");
        int columns = int.Parse(Console.ReadLine());

        double[,] matrix = new double[rows, columns];
        Console.WriteLine("Enter the matrix elements:");
        for (int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                matrix[i, j] = double.Parse(Console.ReadLine());
            }
        }

        Console.WriteLine("Original matrix:");
        PrintMatrix(matrix);

        TransformMatrix(matrix);

        Console.WriteLine("Converted matrix:");
        PrintMatrix(matrix);
    }

    static void TransformMatrix(double[,] matrix){
        int rows = matrix.GetLength(0);
        int columns = matrix.GetLength(1);

        for (int i = 0; i < rows; i++){
            double sum = 0;
            for (int j = 0; j < columns; j++){
                sum += matrix[i, j];
                Console.Write(matrix[i, j] + " ");
            }
            double average = sum / columns;
            average = Math.Round(average, 2);
            Console.WriteLine($"=> Average: {average:F2}");
            for (int j = 0; j < columns; j++){
                if (matrix[i, j] == 0){
                    matrix[i, j] = average;
                }
            }
        }
    }
    
    static void PrintMatrix(double[,] matrix) {
        int rows = matrix.GetLength(0);
        int columns = matrix.GetLength(1);
        for (int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                Console.Write(matrix[i, j].ToString("F2") + "\t");
            }
            Console.WriteLine();
        }
    }
}
/* 
Преобразовать исходную матрицу так, чтобы равный нулю элемент каждый строки был
 заменен средним арифметическим элементов этой строки. */
