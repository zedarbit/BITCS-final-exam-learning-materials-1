# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification. 
# No part of this specification may be reproduced in any form or means, 
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice. 
# Yingping Liang assumes no responsibility for any errors contained herein.


from PyQt5.QtWidgets import QMainWindow
from PyQt5.uic import loadUi


class AddMainWindow(QMainWindow):
    '''
    添加用窗口的父类
    '''
    def __init__(self, parent=None, ui=None):
        super(AddMainWindow, self).__init__(parent)
        loadUi(ui, self) # 加载.ui文件，初始化布局
        self.add.clicked.connect(self.addToDatabase) # 添加到数据库
        self.add.clicked.connect(self.close) # 绑定关闭功能
        self.cancel.clicked.connect(self.close) # 绑定取消功能

    def close(self):
        self.destroy(True) # 销毁窗口

    def addToDatabase(self):
        pass
