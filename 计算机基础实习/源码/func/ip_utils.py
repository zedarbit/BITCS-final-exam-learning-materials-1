import socket


def click_get_ip():
    # 获取本机ip
    sucess = True
    my_addr = None
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        my_addr = s.getsockname()[0]
    except Exception as ret:
        # 若无法连接互联网使用，会调用以下方法
        try:
            my_addr = socket.gethostbyname(socket.gethostname())
        except Exception as ret_e:
            sucess = False
    finally:
        s.close()
        return sucess, my_addr
