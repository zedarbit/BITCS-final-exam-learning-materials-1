from time import ctime, sleep
import socket
import sys
from UILib.MainWindow import MainWindow
from PyQt5.QtCore import QTimer, QThread, pyqtSignal
import qdarkstyle
from PyQt5.QtWidgets import QApplication, QMessageBox
from UILib.Database import KEYS
from PyQt5.QtGui import QImage, QPixmap


class mythread(QThread):  # 步骤1.创建一个线程实例
    mysignal = pyqtSignal(str)  # 创建一个自定义信号，元组参数

    def __init__(self, ipText, portValue):
        super(mythread, self).__init__()
        self.ipText = ipText
        self.portValue = portValue

    def close_thread(self):
        self.ser.close()
        # self.ser.shutdown(0)
        self.quit()

    def run(self):
        # 套接字类型AF_INET, socket.SOCK_STREAM   tcp协议，基于流式的协议
        self.ser = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 对socket的配置重用ip和端口号
        self.ser.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        # 绑定端口号
        self.ser.bind((self.ipText, self.portValue))  # 写哪个ip就要运行在哪台机器上
        # 设置半连接池
        self.ser.listen(3)  # 最多可以连接多少个客户端
        print('【Sever启动成功】{}\n目标IP：{}\n端口号：{}'.format(
            ctime(), self.ipText, self.portValue))
        while 1:
            # 阻塞等待，创建连接
            print("等待连接中..")
            con, address = self.ser.accept()  # 在这个位置进行等待，监听端口号
            print("等待消息中...")
            while 1:
                try:
                    # 接受套接字的大小，怎么发就怎么收
                    msg = con.recv(1024)
                    meg = msg.decode('utf-8')
                    if meg == 'quit':
                        # 断开连接
                        con.close()
                    self.mysignal.emit(meg)  # 发射自定义信号
                except Exception as e:
                    print(e)
                    break


class MainSeverUI(MainWindow):

    def __init__(self):
        """
        初始化界面 ，连接槽函数，以及设置校验器
        """
        super(MainSeverUI, self).__init__()
        self.values_max_num = 50
        self.face_pt = None
        self.my_thread = [None, None, None]

    def warning_box(self, title, message):
        QMessageBox.about(self, title, message)

    def close_tcp_server(self, index=0):
        print('【当前线程是否开启】', not self.my_thread[index] is None)
        if not self.my_thread[index] is None:
            self.my_thread[index].close_thread()
            print('【关闭线程】成功关闭线程', index+1)
 
    def start_tcp_server(self, index=None):
        # 实例化一个socket
        if index == 0:
            if self.address1.text() == '待输入' or self.target_ip_1.text() == '待输入':
                print('【错误】请输入正确的IP地址和端口号')
                self.warning_box('【错误】', '请输入正确的IP地址和端口号')
                return
            else:
                portValue = int(self.address1.text())
                ipText = self.target_ip_1.text()
        elif index == 1:
            if self.address2.text() == '待输入' or self.target_ip_2.text() == '待输入':
                print('【错误】请输入正确的IP地址和端口号')
                self.warning_box('【错误】', '请输入正确的IP地址和端口号')
                return
            else:
                portValue = int(self.address2.text())
                ipText = self.target_ip_2.text()
        else:
            if self.address3.text() == '待输入' or self.target_ip_3.text() == '待输入':
                print('【错误】请输入正确的IP地址和端口号')
                self.warning_box('【错误】', '请输入正确的IP地址和端口号')
                return
            else:
                portValue = int(self.address3.text())
                ipText = self.target_ip_3.text()

        self.my_thread[index] = mythread(ipText, portValue)  # 主线程连接子线
        print('【当前线程是否开启】', not self.my_thread[index] is None)
        if index == 0:
            self.my_thread[index].mysignal.connect(
                self.update_values_1)  # 自定义信号连接
        elif index == 1:
            self.my_thread[index].mysignal.connect(
                self.update_values_2)  # 自定义信号连接
        else:
            self.my_thread[index].mysignal.connect(
                self.update_values_3)  # 自定义信号连接
        print('【线程启动】{}\n目标IP：{}\n端口号：{}'.format(
            ctime(), ipText, portValue))
        self.my_thread[index].start()  # 子线程开始执行run函数


def main():
    '''
    启动PyQt5程序，打开GUI界面。
    '''
    app = QApplication(sys.argv)
    main_window = MainSeverUI()
    app.setStyleSheet(qdarkstyle.load_stylesheet_pyqt5())
    main_window.generate_image()
    main_window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':

    main()
