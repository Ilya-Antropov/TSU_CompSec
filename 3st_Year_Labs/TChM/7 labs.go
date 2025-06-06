package main

import (
	"crypto/rand"
	"fmt"
	"math/big"
)

func GenerateStrongPrime(bits int) (*big.Int, error) {
	s, err := rand.Prime(rand.Reader, bits/2)
	if err != nil {
		return nil, fmt.Errorf("failed to generate prime s: %v", err)
	}

	t, err := rand.Prime(rand.Reader, bits/2)
	if err != nil {
		return nil, fmt.Errorf("failed to generate prime t: %v", err)
	}

	r, err := findPrimeInSequence(t, func(i *big.Int) *big.Int {
		r := new(big.Int).Mul(i, t)
		r.Mul(r, big.NewInt(2))
		r.Add(r, big.NewInt(1))
		return r
	})
	if err != nil {
		return nil, fmt.Errorf("failed to find prime r: %v", err)
	}

	p0, err := computeP0(s, r)
	if err != nil {
		return nil, fmt.Errorf("failed to compute p0: %v", err)
	}

	p, err := findPrimeInSequence(new(big.Int).Mul(r, s), func(j *big.Int) *big.Int {
		p := new(big.Int).Mul(j, r)
		p.Mul(p, s)
		p.Mul(p, big.NewInt(2))
		p.Add(p, p0)
		return p
	})
	if err != nil {
		return nil, fmt.Errorf("failed to find prime p: %v", err)
	}

	return p, nil
}

func findPrimeInSequence(coeff *big.Int, sequence func(*big.Int) *big.Int) (*big.Int, error) {
	i0, err := rand.Int(rand.Reader, new(big.Int).Exp(big.NewInt(2), big.NewInt(32), nil))
	if err != nil {
		return nil, err
	}

	i := new(big.Int).Set(i0)

	for {
		candidate := sequence(i)
		if candidate.ProbablyPrime(20) {
			return candidate, nil
		}
		i.Add(i, big.NewInt(1))
	}
}

func computeP0(s, r *big.Int) (*big.Int, error) {
	rMinus2 := new(big.Int).Sub(r, big.NewInt(2))

	exp := new(big.Int).Exp(s, rMinus2, r)

	twoS := new(big.Int).Mul(big.NewInt(2), s)
	result := new(big.Int).Mul(exp, twoS)

	result.Sub(result, big.NewInt(1))

	return result, nil
}

func main() {
	bits := 120
	p, err := GenerateStrongPrime(bits)
	if err != nil {
		fmt.Println("Error generating strong prime:", err)
		return
	}

	fmt.Printf("Generated strong prime (%d bits):\n%v\n", p.BitLen(), p)
}