import inspect

def func():
    print("This is a function.")

if __name__ == "__main__":
    print(hasattr(func, '__name__'))
    print(func.__name__)
    # print(inspect.getmembers(func))
    print(inspect.isfunction(func))