#include <iostream>
long fib(int n) {
  if (n <= 1) {
    return n;
  } else {
    return (fib(n - 1) + fib(n - 2));
  }
}
int main() {

  // No printing for benchmarking
  //  std::cout << fib(25) << "\n";
  fib(25);
  return 0;
}
