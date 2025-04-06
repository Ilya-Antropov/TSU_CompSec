package main

import (
	"fmt"
	"math/big"
	"strconv"
	"time"
)

func numInArray(x *big.Int) []int {
	strNum := x.String()
	n := len(strNum)
	num := make([]int, n)

	for i := 0; i < n; i++ {
		num[i] = int(strNum[n-1-i] - '0')
	}

	return num
}

func numNorm(result []int) *big.Int {
	n := len(result)
	numberStr := ""
	for i := n - 1; i >= 0; i-- {
		numberStr += strconv.Itoa(result[i])
	}

	number := new(big.Int)
	number, _ = number.SetString(numberStr, 10)
	return number
}

// Быстрое возведение в квадрат
func quickSquare(x *big.Int) *big.Int {
	num := numInArray(x)

	lenOfNumber := len(num)
	result := make([]int, 2*lenOfNumber+1)

	var uv, cu, tmp int

	for i := 0; i < lenOfNumber; i++ {
		uv = result[2*i] + num[i]*num[i]
		result[2*i] = uv % 10
		cu = uv / 10

		for j := i + 1; j < lenOfNumber; j++ {
			tmp = result[i+j] + 2*num[i]*num[j] + cu
			cu = tmp / 10
			result[i+j] = tmp % 10
		}

		result[i+lenOfNumber] += cu % 10
		result[i+lenOfNumber+1] += cu / 10
	}

	return numNorm(result)
}

// Дихотомический алгоритм возведения в степень
func exponentiation(x, y *big.Int) *big.Int {
	binaryY := y.Text(2)
	q := new(big.Int).Set(x)
	z := big.NewInt(1)

	if binaryY[len(binaryY)-1] == '1' {
		z.Set(x)
	}

	for i := len(binaryY) - 2; i >= 0; i-- {
		q = quickSquare(q)
		if binaryY[i] == '1' {
			z.Mul(z, q)
		}
	}

	return z
}

func main() {
	var x string
	var y string

	fmt.Print("Введите число x: ")
	_, errX := fmt.Scan(&x)
	if errX != nil {
		fmt.Println("Ошибка ввода x:", errX)
		return
	}

	fmt.Print("Введите степень y: ")
	_, errY := fmt.Scan(&y)
	if errY != nil {
		fmt.Println("Ошибка ввода y:", errY)
		return
	}

	power := new(big.Int)
	power, ok := power.SetString(y, 10)
	if !ok {
		fmt.Println("Ошибка преобразования числа.")
		return
	}

	num := new(big.Int)
	num, ok = num.SetString(x, 10)
	if !ok {
		fmt.Println("Ошибка преобразования числа.")
		return
	}

	startTime := time.Now()
	normalSquare := new(big.Int).Mul(num, num)
	normalSquareTime := time.Since(startTime)

	startTime = time.Now()
	quickSquareResult := quickSquare(num)
	quickSquareTime := time.Since(startTime)

	startTime = time.Now()
	nativePow := new(big.Int).Exp(num, power, nil)
	nativePowTime := time.Since(startTime)

	startTime = time.Now()
	quickPow := exponentiation(num, power)
	quickPowTime := time.Since(startTime)

	fmt.Println("\n--- Результаты ---")
	fmt.Println("Обычный квадрат:", normalSquare.String(), "| Время:", normalSquareTime)
	fmt.Println("Быстрый квадрат:", quickSquareResult.String(), "| Время:", quickSquareTime)
	fmt.Println("Обычное возведение в степень:", nativePow.String(), "| Время:", nativePowTime)
	fmt.Println("Быстрое возведение в степень:", quickPow.String(), "| Время:", quickPowTime)

	fmt.Println("\n--- Сравнение скорости ---")
	if quickSquareTime < normalSquareTime {
		fmt.Println("Быстрое возведение в квадрат быстрее ✅")
	} else {
		fmt.Println("Обычное возведение в квадрат быстрее ✅")
	}
	if quickPowTime < nativePowTime {
		fmt.Println("Быстрое возведение в степень быстрее ✅")
	} else {
		fmt.Println("Обычное возведение в степень быстрее ✅")
	}
}
