 package main

import (
	"fmt"
	"math"
	"math/big"
	"math/rand"
	"strconv"
	"time"
)

const (
	iterations = 25
)

func powMod(a, b, m *big.Int) *big.Int {
	return new(big.Int).Exp(a, b, m)
}

func randomInRange(min, max *big.Int) *big.Int {
	randGen := rand.New(rand.NewSource(time.Now().UnixNano()))
	diff := new(big.Int).Sub(max, min)
	randNum := new(big.Int).Rand(randGen, diff)
	return randNum.Add(randNum, min)
}

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

// Быстрое возведение в квадрат (1)
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

// Дихотомический алгоритм возведения в степень (2)
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

// Алгоритм Барретта приведения чисел по модулю. (3)
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

	xDiv := new(big.Int).Div(x, exponentiation(base, big.NewInt(k-1)))
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

func eulerPhi(n *big.Int) *big.Int {
	if n.ProbablyPrime(5) {
		return new(big.Int).Sub(n, big.NewInt(1))
	}

	result := new(big.Int).Set(n)
	m := new(big.Int).Set(n)

	if m.Bit(0) == 0 {
		result.Sub(result, new(big.Int).Div(result, big.NewInt(2)))
		for m.Bit(0) == 0 {
			m.Rsh(m, 1)
		}
	}

	i := big.NewInt(3)
	sqrt := new(big.Int).Sqrt(m)
	for i.Cmp(sqrt) <= 0 {
		mod := new(big.Int)
		if mod.Mod(m, i).Cmp(big.NewInt(0)) == 0 {
			result.Sub(result, new(big.Int).Div(result, i))
			for mod.Mod(m, i).Cmp(big.NewInt(0)) == 0 {
				m.Div(m, i)
			}
			sqrt.Sqrt(m)
		}
		i.Add(i, big.NewInt(2))
	}

	if m.Cmp(big.NewInt(1)) > 0 {
		result.Sub(result, new(big.Int).Div(result, m))
	}

	return result
}

// Тест Ферма проверки числа на простоту. Оценить вероятность ошибки. (4)
func testFerma(num *big.Int) (string, time.Duration) {
	start := time.Now()
	defer func() {
		elapsed := time.Since(start)
		fmt.Printf("Время выполнения теста Ферма: %v\n", elapsed)
	}()

	if num.Bit(0) == 0 {
		return "Составное (чётное)", 0
	}

	const t = 5
	randGen := rand.New(rand.NewSource(time.Now().UnixNano()))

	for i := 0; i < t; i++ {
		a := new(big.Int)
		max := new(big.Int).Sub(num, big.NewInt(2))
		a.Rand(randGen, max).Add(a, big.NewInt(2))

		if new(big.Int).Exp(a, new(big.Int).Sub(num, big.NewInt(1)), num).Cmp(big.NewInt(1)) != 0 {
			return "Составное", 0
		}
	}

	phi := eulerPhi(num)
	ratio := new(big.Rat).SetFrac(phi, num)
	floatRatio, _ := ratio.Float64()
	prError := math.Pow(floatRatio, float64(t))

	return fmt.Sprintf("Простое (вероятность ошибки: %.6f)", prError), 0
}

func jacobiSymbol(a, n *big.Int) int {
	if a.Cmp(big.NewInt(0)) == 0 {
		return 0
	}
	if a.Cmp(big.NewInt(1)) == 0 {
		return 1
	}

	k := big.NewInt(0)
	aCopy := new(big.Int).Set(a)
	two := big.NewInt(2)
	zero := big.NewInt(0)

	for new(big.Int).Mod(aCopy, two).Cmp(zero) == 0 {
		aCopy = new(big.Int).Div(aCopy, two)
		k.Add(k, big.NewInt(1))
	}

	s := 1
	if new(big.Int).Mod(k, two).Cmp(zero) != 0 {
		nMod8 := new(big.Int).Mod(n, big.NewInt(8))
		if nMod8.Cmp(big.NewInt(1)) == 0 || nMod8.Cmp(big.NewInt(7)) == 0 {
			s = 1
		} else {
			s = -1
		}
	}

	if new(big.Int).Mod(n, big.NewInt(4)).Cmp(big.NewInt(3)) == 0 &&
		new(big.Int).Mod(aCopy, big.NewInt(4)).Cmp(big.NewInt(3)) == 0 {
		s = -s
	}

	if aCopy.Cmp(big.NewInt(1)) == 0 {
		return s
	} else {
		return s * jacobiSymbol(new(big.Int).Mod(n, aCopy), aCopy)
	}
}

// Тест Миллера-Рабина проверки чилса на простату. Оценить вероятность ошибки.(5.1)
func millerRabinTest(n *big.Int, k int) (string, float64, time.Duration) {
	start := time.Now()
	defer func() {
		elapsed := time.Since(start)
		fmt.Printf("Время выполнения теста Миллера-Рабина: %v\n", elapsed)
	}()

	if n.Cmp(big.NewInt(2)) < 0 {
		return "Составное", 0, 0
	}
	if n.Cmp(big.NewInt(2)) == 0 || n.Cmp(big.NewInt(3)) == 0 {
		return "Простое", 0, 0
	}
	if new(big.Int).Mod(n, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
		return "Составное", 0, 0
	}

	d := new(big.Int).Sub(n, big.NewInt(1))
	s := big.NewInt(0)
	for new(big.Int).Mod(d, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
		d.Div(d, big.NewInt(2))
		s.Add(s, big.NewInt(1))
	}

	for i := 0; i < k; i++ {
		a := randomInRange(big.NewInt(2), new(big.Int).Sub(n, big.NewInt(2)))
		x := powMod(a, d, n)

		if x.Cmp(big.NewInt(1)) == 0 || x.Cmp(new(big.Int).Sub(n, big.NewInt(1))) == 0 {
			continue
		}

		found := false
		for j := big.NewInt(1); j.Cmp(s) < 0; j.Add(j, big.NewInt(1)) {
			x = powMod(x, big.NewInt(2), n)
			if x.Cmp(new(big.Int).Sub(n, big.NewInt(1))) == 0 {
				found = true
				break
			}
			if x.Cmp(big.NewInt(1)) == 0 {
				return "Составное", 0, 0
			}
		}

		if !found {
			return "Составное", 0, 0
		}
	}

	errorProb := math.Pow(0.25, float64(k))
	return "Простое", errorProb, 0
}

// Тест Соловея-Штрассена проверки чилса на простату. Оценить вероятность ошибки.(5.2)
func solovayStrassenTest(n *big.Int, k int) (string, float64, time.Duration) {
	start := time.Now()
	defer func() {
		elapsed := time.Since(start)
		fmt.Printf("Время выполнения теста Соловея-Штрассена: %v\n", elapsed)
	}()

	if n.Cmp(big.NewInt(2)) < 0 {
		return "Составное", 0, 0
	}
	if n.Cmp(big.NewInt(2)) == 0 || n.Cmp(big.NewInt(3)) == 0 {
		return "Простое", 0, 0
	}
	if new(big.Int).Mod(n, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
		return "Составное", 0, 0
	}

	for i := 0; i < k; i++ {
		a := randomInRange(big.NewInt(2), new(big.Int).Sub(n, big.NewInt(2)))
		exp := new(big.Int).Div(new(big.Int).Sub(n, big.NewInt(1)), big.NewInt(2))
		x := powMod(a, exp, n)

		if x.Cmp(big.NewInt(1)) != 0 && x.Cmp(new(big.Int).Sub(n, big.NewInt(1))) != 0 {
			return "Составное", 0, 0
		}

		jacobi := jacobiSymbol(a, n)
		var expected *big.Int
		if jacobi == -1 {
			expected = new(big.Int).Sub(n, big.NewInt(1))
		} else {
			expected = big.NewInt(int64(jacobi))
		}

		if x.Cmp(expected) != 0 {
			return "Составное", 0, 0
		}
	}

	errorProb := math.Pow(0.5, float64(k))
	return "Простое", errorProb, 0
}

func main() {
	var xStr string
	var y string

	fmt.Print("Введите число для проверки на простоту: ")
	_, errX := fmt.Scan(&xStr)
	if errX != nil {
		fmt.Println("Ошибка ввода:", errX)
		return
	}

	x := new(big.Int)
	x, ok := x.SetString(xStr, 10)
	if !ok {
		fmt.Println("Ошибка преобразования числа")
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
	mod, ok = mod.SetString(mStr, 10)
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
	num, ok = num.SetString(xStr, 10)
	if !ok {
		fmt.Println("Ошибка преобразования числа.")
		return
	}
/*
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
		}*/

	fmt.Printf("\nРезультаты тестов (количество итераций: %d):\n", iterations)
	// Тест Ферма
	fermaResult, _ := testFerma(x)
	fmt.Println("- Тест Ферма:", fermaResult)
	// Тест Миллера-Рабина
	mrResult, mrError, _ := millerRabinTest(x, iterations)
	fmt.Printf("- Тест Миллера-Рабина: %s (вероятность ошибки: ≤ %.15f)\n", mrResult, mrError)
	// Тест Соловея-Штрассена
	ssResult, ssError, _ := solovayStrassenTest(x, iterations)
	fmt.Printf("- Тест Соловея-Штрассена: %s (вероятность ошибки: ≤ %.15f)\n", ssResult, ssError)

}
