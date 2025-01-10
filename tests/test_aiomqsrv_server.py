#!/usr/bin/env python
import sys
import os
import os.path as osp
import asyncio

cur_dir = osp.abspath(osp.dirname(__file__))
sys.path.insert(0, cur_dir+'/../')

import toml
import signal

from loguru import logger
import time
from kombu import Connection, Exchange
from aiomqsrv.base import get_rpc_exchange
from aiomqsrv.server import MessageQueueServer, run_server, make_server

async def echo(a, b, c, **kwargs):
    await asyncio.sleep(5)
    return a

def run():
    addr = "amqp://guest:guest@0.0.0.0:5672/"
    rpc_queue = 'server_rpc_routing_key'
    server = make_server(
        conn = addr,
        rpc_routing_key=rpc_queue,
    )

    server.register_rpc(echo)
    
    loop = asyncio.get_event_loop()
    loop.create_task(run_server(server, block=False))
    loop.run_forever()


if __name__ == "__main__":
    run()