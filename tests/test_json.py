import json
import pickle
import msgpack
import msgpack_numpy
msgpack_numpy.patch()

def json_dumpb(v):
    return json.dumps(v).encode()

def json_loadb(v):
    return json.loads(v.decode())

pack_funcs = {
    'json': (json_dumpb, json_loadb),
    'pickle': (pickle.dumps, pickle.loads),
    'msgpack': (msgpack.packb, msgpack.unpackb),
}

def rpc_encode_req(msgid, meth, args, kws):
    return [0, msgid, meth, args, kws]

def rpc_decode_req(data, check_id=None):
    typ, msgid, meth, args, kws = data
    assert typ == 0
    if check_id:
        assert msgid == check_id

    return msgid, meth, args, kws

def rpc_encode_rep(msgid, error=None, result=None):
    if error:
        error = pack_exc(error)
    return [1, msgid, error, result]

def rpc_decode_rep(data, check_id=None):
    typ, msgid, error, result = data
    assert typ == 1
    if check_id:
        assert msgid == check_id

    if error:
        try:
            error = unpack_exc(error)
        except TypeError:
            error = None
    return error, result

def rpc_encode_noti(meth, params):
    return [2, meth, params]

def rpc_decode_noti(data):
    typ, meth, params = data
    assert typ == 2
    return meth, params

def main():
    # strings = """[0, "corr-ff32ff9f-7281-4fc5-a502-a2e94acf37d2", "echo", [1, "ss", [1, 2, "ss"]], {"v": 5}]"""
    strings = """[1,"corr-64095c2e-966a-4ac0-bdeb-56d62e0ed933",null,3]"""
    str_bytes = strings.encode()
    b = json.loads(str_bytes.decode())
    print(b)

if __name__ == "__main__":
    main()