import os
import time
from gevent.pool import Pool

class Test:
    def __init__(self):
        self.pool = Pool(10)
    
    def func(self):
        print("func...")

global a
a = "sss"
ctx = Test()
ctx_pool = Pool(10)
def apply_ctx(meth, *args, **kwargs):
    # def worker(ctx):
    #     getattr(ctx, meth)(*args, **kws)
    global a
    a += "a"
    # time.sleep(10)
    try:
        getattr(ctx, meth)(*args, **kwargs)
    except AttributeError:
        print("no attr")
    return 1
    
print(a)
# apply_ctx("func")
# for res in ctx_pool.imap(apply_ctx, ["func"]):
#     print(res)
res = ctx_pool.imap(apply_ctx, ["func", "set"])
print(res.next())
ctx_pool.join()
# time.sleep(2)
print(a)
    