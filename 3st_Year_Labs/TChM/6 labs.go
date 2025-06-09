package main

import (
	"crypto/rand"
	"fmt"
	"math/big"
	"time"
)

func powMod(x, y, n *big.Int) *big.Int {
	result := big.NewInt(1)
	zero := big.NewInt(0)
	one := big.NewInt(1)
	two := big.NewInt(2)

	xMod := new(big.Int).Mod(x, n)
	yTmp := new(big.Int).Set(y)

	for yTmp.Cmp(zero) > 0 {
		if new(big.Int).Mod(yTmp, two).Cmp(one) == 0 {
			result.Mul(result, xMod)
			result.Mod(result, n)
		}
		yTmp.Rsh(yTmp, 1)
		xMod.Mul(xMod, xMod)
		xMod.Mod(xMod, n)
	}
	return result
}

// Функция для факторизации числа
func factorize(n *big.Int) map[string]*big.Int {
	factors := make(map[string]*big.Int)
	if n.Cmp(big.NewInt(1)) <= 0 {
		return factors
	}

	tmp := new(big.Int).Set(n)
	two := big.NewInt(2)
	zero := big.NewInt(0)

	for new(big.Int).Mod(tmp, two).Cmp(zero) == 0 {
		if _, ok := factors["2"]; ok {
			factors["2"].Add(factors["2"], big.NewInt(1))
		} else {
			factors["2"] = big.NewInt(1)
		}
		tmp.Div(tmp, two)
	}

	i := big.NewInt(3)
	sqrt := new(big.Int).Sqrt(tmp)
	for i.Cmp(sqrt) <= 0 {
		for new(big.Int).Mod(tmp, i).Cmp(zero) == 0 {
			if _, ok := factors[i.String()]; ok {
				factors[i.String()].Add(factors[i.String()], big.NewInt(1))
			} else {
				factors[i.String()] = big.NewInt(1)
			}
			tmp.Div(tmp, i)
			sqrt = new(big.Int).Sqrt(tmp)
		}
		i.Add(i, two)
	}

	if tmp.Cmp(big.NewInt(1)) > 0 {
		factors[tmp.String()] = big.NewInt(1)
	}

	return factors
}

// Тест простоты Люка
func lucasPrimalityTest(n *big.Int, t int, factorization map[string]*big.Int) bool {
	if n.Cmp(big.NewInt(2)) < 0 {
		return false
	}
	if n.Cmp(big.NewInt(2)) == 0 || n.Cmp(big.NewInt(3)) == 0 {
		return true
	}
	if new(big.Int).Mod(n, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
		return false
	}

	var primeDivs []*big.Int
	nMinus1 := new(big.Int).Sub(n, big.NewInt(1))

	if factorization == nil {
		factorization = factorize(nMinus1)
	}

	for pStr := range factorization {
		p, _ := new(big.Int).SetString(pStr, 10)
		primeDivs = append(primeDivs, p)
	}

	for i := 0; i < t; i++ {
		a, err := rand.Int(rand.Reader, new(big.Int).Sub(n, big.NewInt(3)))
		if err != nil {
			panic(err)
		}
		a.Add(a, big.NewInt(2))

		if powMod(a, nMinus1, n).Cmp(big.NewInt(1)) != 0 {
			return false
		}

		isPrimitive := true
		for _, p := range primeDivs {
			exponent := new(big.Int).Div(nMinus1, p)
			if powMod(a, exponent, n).Cmp(big.NewInt(1)) == 0 {
				isPrimitive = false
				break
			}
		}

		if isPrimitive {
			return true
		}
	}

	return false
}

// Вероятность ошибки метода Люка
func likelihoodErrorMethodLuke(n *big.Int, t int) float64 {
	nMinus1 := new(big.Int).Sub(n, big.NewInt(1))
	phi := eulerTotient(nMinus1)
	phiFloat := new(big.Float).SetInt(phi)
	nMinus1Float := new(big.Float).SetInt(nMinus1)

	ratio := new(big.Float).Quo(phiFloat, nMinus1Float)
	one := big.NewFloat(1)
	ratio.Sub(one, ratio)

	result := new(big.Float).SetFloat64(1)
	for i := 0; i < t; i++ {
		result.Mul(result, ratio)
	}

	res, _ := result.Float64()
	return res
}

// Функция Эйлера
func eulerTotient(n *big.Int) *big.Int {
	if n.Cmp(big.NewInt(1)) == 0 {
		return big.NewInt(1)
	}

	result := new(big.Int).Set(n)
	factors := factorize(n)

	for pStr := range factors {
		p, _ := new(big.Int).SetString(pStr, 10)
		result.Div(result, p)
		result.Mul(result, new(big.Int).Sub(p, big.NewInt(1)))
	}

	return result
}

func formatDuration(d time.Duration) string {
	if d < time.Microsecond {
		return fmt.Sprintf("%d ns", d.Nanoseconds())
	}
	if d < time.Millisecond {
		return fmt.Sprintf("%.2f µs", float64(d.Nanoseconds())/1000)
	}
	if d < time.Second {
		return fmt.Sprintf("%.2f ms", float64(d.Nanoseconds())/1000000)
	}
	return fmt.Sprintf("%.2f s", d.Seconds())
}

func main() {
	var nStr string
	fmt.Print("Введите число для проверки на простоту: ")
	fmt.Scan(&nStr)

	n, ok := new(big.Int).SetString(nStr, 10)
	if !ok {
		fmt.Println("Ошибка ввода числа")
		return
	}

	var k int
	fmt.Print("Введите количество итераций теста: ")
	fmt.Scan(&k)

	start := time.Now()
	result := lucasPrimalityTest(n, k, nil)
	elapsed := time.Since(start)

	if result {
		fmt.Println("Простое по критерию Люка.")
		fmt.Printf("Время выполнения: %s\n", formatDuration(elapsed))
	} else {
		fmt.Println("Составное по критерию Люка.")
		fmt.Printf("Время выполнения: %s\n", formatDuration(elapsed))
		fmt.Printf("Вероятность ошибки: %.10f\n", likelihoodErrorMethodLuke(n, k))
	}
}