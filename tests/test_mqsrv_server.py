import os
import os.path as osp
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.server import make_server, run_server
from mqsrv.exc import BaseException as BE

def foo(arg1, *args, **kwargs):
    print(arg1)
    return "ok"

def bar(arg1, *args, **kwargs):
    print(arg1)
    return "ok"

def add(x, y):
    return x + y

class Test:
    def __init__(self) -> None:
        pass

    def test(self, arg1, *args, **kwargs):
        print(arg1)
        # time.sleep(5)
        raise BE
    
def main():
    server = make_server(
        rpc_routing_key="test_rpc_queue"
    )
    t = Test()
    server.register_rpc(add, "add")
    server.register_rpc(foo, "foo")
    server.register_rpc(bar, "bar")
    server.register_rpc(t.test, "test")
    run_server(server)
    
if __name__ == "__main__":
    main()
