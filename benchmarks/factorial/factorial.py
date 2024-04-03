def factorial(n):
    if n == 0:
        return 1
    else:
        return factorial(n - 1) * n


start = 250
while start >= 0:
    factorial(start)
    # No printing for benchmark
    # print(factorial(start))
    start = start - 1
