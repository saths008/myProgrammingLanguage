
long factorial(int n) {
  if (n == 0) {
    return 1;
  } else {
    return factorial(n - 1) * n;
  }
}

int main() {
  int start = 250;
  while (start >= 0) {
    factorial(start);
    // No printing for benchmark
    //  std::cout << factorial(start) << "\n";
    start = start - 1;
  }

  return 0;
}
