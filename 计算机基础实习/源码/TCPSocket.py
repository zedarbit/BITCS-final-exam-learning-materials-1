from time import ctime, sleep
import socket
import sys
import cv2

from UILib.SonWindow import MainWindow
from PyQt5.QtCore import QTimer, QThread, pyqtSignal
from PyQt5.QtWidgets import QSizePolicy, QMessageBox
from processor.MainProcessor import MainProcessor


class MainSocketUI(MainWindow):

    def __init__(self):
        """
        初始化界面 ，连接槽函数，以及设置校验器
        """
        super(MainSocketUI, self).__init__()
        self.label.setText('智能医疗监控云平台——终端')
        self.resize(450, 600)
        self.p = None

        self.vs = cv2.VideoCapture(0)
        self.video = None
        ret, _ = self.vs.read()
        if not ret:
            print('【进程错误】摄像头被占用，启用默认视频')
            self.vs = cv2.VideoCapture('data/peng.mp4')
            self.video = 'data/peng.mp4'
        self.address.setText('待输入')

    def warning_box(self, title, message):
        QMessageBox.about(self, title, message)

    def start_tcp_server(self):
        # 实例化一个socket
        if self.address.text() == '待输入' or self.target_ip.text() == '待输入':
            print('【错误】请输入正确的IP地址和端口号')
            return
        else:
            portValue = int(self.address.text())
            ipText = self.target_ip.text()

        self.p = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        try:
            self.p.connect((ipText, portValue))
        except Exception as e:
            print('【连接错误】端口被占用或者未启动')
            self.warning_box('【连接错误】', '端口被占用或者未启动')
        else:
            print('【连接成功】{}\n目标IP：{}\n端口号：{}'.format(
                ctime(), ipText, portValue))
            self.processor = MainProcessor(None, False, p=self.p)

def main():
    '''
    启动PyQt5程序，打开GUI界面。
    '''
    app = QApplication(sys.argv)
    main_window = MainSocketUI()
    app.setStyleSheet(qdarkstyle.load_stylesheet_pyqt5())
    main_window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':

    import qdarkstyle
    from PyQt5.QtWidgets import QApplication

    main()
