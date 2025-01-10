import time
import os
import sys
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.client import make_client
from loguru import logger

class Test:
    def __init__(self):
        print("Test class")

def main():
    client = make_client()
    caller = client.get_caller("test_rpc_queue")
    
    t= Test()
    a = [1, "ss", 3.45]
    b = (1, 3, 5, "sa", 4.56) 
    s = {"a", 1, 3}
    # _, res = caller.fo2o("test", a, b, True)
    # _, res = caller.bar("test2")
    exc, res = caller.test("test3")
    # raise exc
    print(exc)
    print(res)
    _, res = caller.add(1, 2)
    print(res)
    
if __name__ == "__main__":
    main()
