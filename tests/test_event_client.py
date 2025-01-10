import os
import os.path as osp
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.client import make_client

from datetime import datetime

def main():
    client = make_client()
    pub = client.get_pubber("test_event_queue")

    for i in range(10):
        pub("test_event", {"1":i, "2":i**2, "3":i**3})
        print(datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3])

if __name__ == '__main__':
    main()
