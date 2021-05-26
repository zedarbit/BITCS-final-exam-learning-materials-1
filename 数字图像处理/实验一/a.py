import numpy as np

if __name__ == '__main__':

    if True:
        print("Hello World.")
    else:
        pass

    i = 0
    while True:
        print(i)
        if i > 10:
            break
        i += 1
    
    def WTF(i):
        return i ** 3

    print(WTF(i))