package main

import "fmt"

<<<<<<< Updated upstream
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
=======
type Point struct {
	x, y int
}

func main() {
	r1 := Point{0, 1}

	r2 := r1

	p1 := &r1

	r2.x, r2.y = 2, 4

	fmt.Println(r1, r2)

	p1.x, p1.y = 3, 6
	fmt.Println(r1)

	fmt.Println(p1)
>>>>>>> Stashed changes
}
