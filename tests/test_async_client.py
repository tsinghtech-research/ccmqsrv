import os
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.client import make_client
from greenthread.green import green_spawn, tpool_execute

def loop_call_add(caller, x, y, n):
    # for _ in range(n):
    #     caller.add(x, y)
    #     time.sleep(0.001)
    [caller.add(x, y) for _ in range(n)]
        
def loop_call_str(caller, s, n):
    # for _ in range(n):
    #     caller.test_str(s)
    #     time.sleep(0.001)
    [caller.test_str(s) for _ in range(n)]

def main():
    client = make_client(conn_pool_maxsize=10)
    caller = client.get_caller("test_rpc_queue")

    loop_count = 100000
    green_spawn(tpool_execute, loop_call_add, caller, 1, 2, loop_count)
    green_spawn(tpool_execute, loop_call_str, caller, "Nirvana", loop_count)
    
    time.sleep(3600)
    
if __name__ == "__main__":
    main()
