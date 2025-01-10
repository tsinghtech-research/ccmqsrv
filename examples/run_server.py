import os
import os.path as osp
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.server import make_server, run_server
from mqsrv.exc import BaseException as BE

def foo():
    print("foo")

def add(x, y):
    print(f"add: {x}+{y}")
    return x + y

def test_str(s):
    print(s)
    return "test_str"+s

def test_vec(lst):
    print(lst)
    return [1, 2, 3, 4, 5]

def test_map(d):
    print(d)
    return {"a": 1, "b": 2, "c": 3}

def test_err():
    raise BE("test_err")

class Test:
    def __init__(self) -> None:
        pass

    def sub(self, a, b):
        print(f"sub: {a}-{b}")
        return a - b
    
def main():
    server = make_server(
        rpc_routing_key="test_rpc_queue"
    )
    t = Test()
    server.register_rpc(add, "add")
    server.register_rpc(foo, "foo")
    server.register_rpc(t.sub, "sub")
    server.register_rpc(test_str, "test_str")
    server.register_rpc(test_vec, "test_vec")
    server.register_rpc(test_map, "test_map")
    server.register_rpc(test_err, "test_err")
    run_server(server)
    
if __name__ == "__main__":
    main()
