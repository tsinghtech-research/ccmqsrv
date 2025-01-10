import os
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.client import make_client

def main():
    client = make_client()
    caller = client.get_caller("test_rpc_queue")
    
    while True:
        _, res = caller.add(1, 2)
        print(res)
        _, res = caller.foo()
        print(res)
        _, res = caller.sub(3, 4)
        print(res)
        # print(type(res))
        _, res = caller.test_str("Nirvana")
        print(res)
        _, res = caller.test_vec([1, 2, 3])
        print(res)
        # print(type(res))
        _,res = caller.test_map({"a":1, "b":2})
        print(res)
        # print(type(res))
        # _,res = caller.test_err()
        # print(res)

if __name__ == "__main__":
    main()
