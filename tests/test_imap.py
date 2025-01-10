import gevent
from gevent.pool import Pool

def task(number):
    # 模拟耗时操作
    gevent.sleep(1)
    result = number * 2
    return result

def main():
    numbers = [1, 2, 3, 4, 5]

    pool = Pool(2)

    # 使用 gevent.pool.imap 并发执行任务
    for result in pool.imap(task, numbers):
        print(result)

    pool.join()

if __name__ == '__main__':
    main()
