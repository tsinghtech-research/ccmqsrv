#!/usr/bin/env python
import sys
import os.path as osp
import asyncio
cur_d = osp.dirname(__file__)
sys.path.insert(0, cur_d+'/../')

from aiomqsrv.client import make_client

async def main(broker_url):
    client = make_client(broker_url)

    caller = client.get_caller('server_rpc_routing_key')
    
    # 若不写await client.connect()，首次用asyncio.gather同时发送多个request，会出现 RESOURCE_LOCKED
    # 若不是首次则不会出现问题
    await client.connect()
    
    '''rpc'''
    # 最简单的rpc调用
    await caller.echo(1,"ss", [1, 2, "ss"], v=5, timeout=1)

    await client.release()

if __name__ == '__main__':
    asyncio.run(main('pyamqp://'))
