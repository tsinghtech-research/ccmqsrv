import os
import os.path as osp
import time
os.environ["GREEN_BACKEND"] = "gevent"
from greenthread.monkey import monkey_patch; monkey_patch()

from mqsrv.server import make_server, run_server

def event(evt_type, evt_data):
    print(f"Received event: {evt_type}, {evt_data}")

def main():
    server = make_server(event_routing_keys = ["test_event_queue"],)
    server.register_event_handler("test_event", event)
    run_server(server)

if __name__ == '__main__':
    main()