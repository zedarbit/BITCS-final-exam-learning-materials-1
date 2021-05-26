# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification.
# No part of this specification may be reproduced in any form or means,
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice.
# Yingping Liang assumes no responsibility for any errors contained herein.

from PyQt5.QtWidgets import QFileDialog

from UILib.Database import Database
from UILib.add_windows.AddMainWindow import AddMainWindow
from PyQt5.QtCore import pyqtSignal


class AddCamera(AddMainWindow):
    '''
    添加监控摄像头
    IP地址使用网络摄像头ip地址，
    本地文件可以选择本地视频路径或者本地摄像头索引
    '''
    signal_addcamera = pyqtSignal(str)  # 用于提交的信号，以字符串形式保存摄像头信息

    def __init__(self, database, parent=None):
        super().__init__(parent, "data/UI/AddCamera.ui")
        self.database = database  # 使用的车辆记录数据库
        self.function_list = ['心率监测', '待添加']  # 功能列表
        self.file_browse.clicked.connect(
            lambda: self.getFile(self.file))  # 文件选择槽函数绑定
        self.function_choice.clear()  # 初始化选择栏
        self.function_choice.addItems(
            name for name in self.function_list)  # 添加功能列表
        self.function_choice.setCurrentIndex(0)


    def emit_signal(self, info):
        self.signal_addcamera.emit(info)  # 提交信号到主界面

    def addToDatabase(self):
        camera_id = str(self.id.text())  # 摄像头id
        camera_ip = str(self.ip.text())  # 摄像头ip
        file = str(self.file.text())  # 文件路径
        group = str(self.group.text())  # 摄像头所属集群
        location = str(self.location.text())  # 摄像头所属具体位置
        func = str(self.function_choice.currentText())  # 摄像头承担检测功能
        self.database.insert_into_camera(
            camera_id, camera_ip, file, group, location, func)  # 更新数据库
        info = group + '-' + camera_id + '-' + \
            location + '-' + file  # 保存摄像头信息，用于以字符串的形式提交信号
        self.emit_signal(info)

    def getFile(self, lineEdit):
        lineEdit.setText(QFileDialog.getOpenFileName()[0])  # 获取文件路径
