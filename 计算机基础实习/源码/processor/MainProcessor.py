# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification.
# No part of this specification may be reproduced in any form or means,
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice.
# Yingping Liang assumes no responsibility for any errors contained herein.

from func.FaceProcessor import FaceTracker
import imutils
import cv2


class TrafficProcessor(object):

    def __init__(self):
        self.firstFrame = None

    def feedCap(self, frame, right_direction='Rear', max_speed=120):

        self.frame = imutils.resize(frame, height=300)

        pack = {'frame': frame, 'reference': self.firstFrame,
                'faces': None, 'cnt': 0, 'graph_values': [], 'eye_values': None}

        return pack


class MainProcessor(object):

    def __init__(self, camera_id=None, main_window=True, p=None):
        self.p = p
        if main_window:
            self.processor = TrafficProcessor()
        else:
            self.processor = FaceTracker()
        self.face_id = 0

    def getProcessedImage(self, frame=None):
        pt = '0'
        dicti = self.processor.feedCap(frame)
        if not dicti['faces'] is None:
            pt = './data/{}.jpg'.format(self.face_id)
            cv2.imwrite(pt, dicti['faces'])
            self.face_id += 1
        if len(dicti['graph_values']):
            msg = '{},{},{}'.format(
                dicti['graph_values'][-1]/2, dicti['eye_values'], pt)
        else:
            msg = '0,0,{}'.format(pt)
        if not self.p is None:
            print('【发送消息】', msg)
            try:
                self.p.send(msg.encode('utf-8'))
            except ConnectionResetError as e:
                print('【连接错误】远程主机强迫关闭了一个现有的连接')
                self.p.shutdown(2)
        return dicti
