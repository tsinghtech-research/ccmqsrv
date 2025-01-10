import os
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.client import make_client

from greenthread.green import green_spawn
from gevent import joinall

def main():
    client = make_client(conn_pool_maxsize=2)
    caller = client.get_caller("test_rpc_queue")
    t_ = []
    t1 = time.time()
    for _ in range(100):
        t = green_spawn(caller.add, 1, 2)
        t_.append(t)
        print("call")
        time.sleep(0.001)
        # _, res = caller.add(1, 2)
        # print(res)
        # _, res = caller.foo()
        # print(res)
        # _, res = caller.sub(3, 4)
        # print(res)
        # # print(type(res))
        # _, res = caller.test_str("Nirvana")
        # print(res)
        # _, res = caller.test_vec([1, 2, 3])
        # print(res)
        # # print(type(res))
        # _,res = caller.test_map({"a":1, "b":2})
        # print(res)
        # print(type(res))
        # _,res = caller.test_err()
        # print(res)
        
    joinall(t_)
    print("used time:", time.time()-t1)

if __name__ == "__main__":
    main()
