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

/*func exponentiationLeftToRightClassic(x, y *big.Int) *big.Int {
	binaryY := y.Text(2)
	z := new(big.Int).Set(x) // z = x, так как yₙ₋₁ = 1 по условию

	// Итерируемся по битам от второго слева до последнего (вправо)
	for i := 1; i < len(binaryY); i++ {
		z = quickSquare(z) // z = z^2

		if binaryY[i] == '1' {
			z.Mul(z, x) // z = z * x
		}
	}

	return z
}*/

// Алгоритм Барретта приведения чисел по модулю.
func barrett(x, m *big.Int, b int64) (*big.Int, error) {
	if m.Sign() == 0 {
		return nil, fmt.Errorf("модуль не может быть нулём")
	}
	if b <= 3 {
		return nil, fmt.Errorf("основание должно быть больше 3")
	}

	k := int64(len(m.String()))
	base := big.NewInt(b)

	if int64(len(x.String())) > 2*k {
		return nil, fmt.Errorf("error")
	}

	bPow2k := exponentiation(base, big.NewInt(2*k))
	z := new(big.Int).Div(bPow2k, m)

	xDiv := new(big.Int).Div(x, exponentiation(base, big.NewInt(k-1))) // часть q'
	qMul := new(big.Int).Mul(xDiv, z)
	qPrime := new(big.Int).Div(qMul, exponentiation(base, big.NewInt(k+1)))

	r1 := new(big.Int).Mod(x, exponentiation(base, big.NewInt(k+1)))
	qm := new(big.Int).Mul(qPrime, m)
	r2 := new(big.Int).Mod(qm, exponentiation(base, big.NewInt(k+1)))

	rPrime := new(big.Int).Sub(r1, r2)
	if rPrime.Sign() < 0 {
		rPrime.Add(rPrime, exponentiation(base, big.NewInt(k+1)))
	}

	for rPrime.Cmp(m) >= 0 {
		rPrime.Sub(rPrime, m)
	}

	return rPrime, nil
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

	fmt.Print("Введите модуль m: ")
	var mStr string
	_, errM := fmt.Scan(&mStr)
	if errM != nil {
		fmt.Println("Ошибка ввода модуля:", errM)
		return
	}

	mod := new(big.Int)
	mod, ok := mod.SetString(mStr, 10)
	if !ok {
		fmt.Println("Ошибка преобразования модуля.")
		return
	}

	power := new(big.Int)
	power, ok = power.SetString(y, 10)
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

	startTime = time.Now()
	barrettRes, err := barrett(quickPow, mod, 10)
	barrettTime := time.Since(startTime)

	startTime = time.Now()
	nativeMod := new(big.Int).Mod(quickPow, mod)
	nativeModTime := time.Since(startTime)

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

	fmt.Println("\n--- Результат Барретта ---")
	if err != nil {
		fmt.Println("Ошибка в алгоритме Барретта:", err)
	} else {
		fmt.Println("Барретт:", barrettRes.String(), "| Время:", barrettTime)
		fmt.Println("Обычный x mod m:", nativeMod.String(), "| Время:", nativeModTime)
		if barrettRes.Cmp(nativeMod) == 0 {
			fmt.Println("✅ Результаты совпадают.")
		} else {
			fmt.Println("❌ Результаты не совпадают.")
		}
	}

}
