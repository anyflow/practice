package main

import "fmt"

func sum(s []int, c chan int) {
	sum := 0
	for _, v := range s {
		sum += v
		fmt.Println(s)
	}
	c <- sum // send sum to c
}

func main() {
	var s [100]int
	for i := 0; i < 100; i++ {
		s[i] = i
	}

	c := make(chan int)
	go sum(s[:len(s)/2], c)
	go sum(s[len(s)/2:], c)
	x, y := <-c, <-c // receive from c

	fmt.Println(x, y, x+y)
}
