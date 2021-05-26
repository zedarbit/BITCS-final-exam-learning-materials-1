# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification.
# No part of this specification may be reproduced in any form or means,
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice.
# Yingping Liang assumes no responsibility for any errors contained herein.


import sqlite3 as lite
from enum import Enum
from PyQt5.QtGui import QPixmap


class KEYS(Enum):
    LOCATION = 'location'
    CARID = 'carid'
    CARCOLOR = 'carcolor'
    FIRSTSIGHTED = 'firstsighted'
    CARIMAGE = 'carimage'
    LICENSENUMBER = 'licensenumber'
    LICENSEIMAGE = 'licenseimage'
    NUMRULESBROKEN = 'numrulesbroken'
    CAROWNER = 'carowner'
    RULENAME = 'rulename'
    RULEFINE = 'rulefine'
    TIME = 'time'
    RULEID = 'ruleid'


class Database:

    def __init__(self):

        A = [('初始界面', '救护车01号', None, '待添加')]
        self.group_camera_list = {'TCP服务端': A}
        self.camera_info = {}
        for cam, place, video, func in A:
            self.camera_info[cam] = [place, video, func]

    def get_licenses(self):
        return

    def get_max_car_id(self):
        return

    def insert_into_violations(self, camera, car, rule, time):
        return

    def insert_into_rules(self, rule, fine):
        return

    def insert_into_camera(self, camera_id, camera_ip, file, group, location, func):
        if not group in self.group_camera_list:
            self.group_camera_list.update({group: []})
        # ('cam_04', '蓬莱路路口', 'traffic.avi')
        if len(camera_ip):
            print('connected.')
            value = (camera_id, location, camera_ip, func)
        else:
            value = (camera_id, location, file, func)
        self.camera_info[camera_id] = [location, file, func]
        self.group_camera_list[group].append(value)

    def search(self, cam=None, color=None, license=None, time=None):
        return

    def get_violations_from_cam(self, cam, cleared=False):
        return

    def delete_violation(self, carid, ruleid, time):
        return

    def get_cam_details(self, cam_id):
        info = self.camera_info[cam_id]
        return 'None', info[0], info[1], info[2]

    def delete_all_cars(self):
        return

    def delete_all_violations(self):
        return

    def get_cam_list(self, group):
        if group is None:
            return self.group_camera_list['区域A']
        return self.group_camera_list[group]

    def get_cam_group_list(self):

        groups = list(self.group_camera_list.keys())

        return groups

    def clear_cam_log(self):
        pass

    def covert_time_to_bd(self, time):
        pass

    def convert_time_to_GUI(self, time):
        pass
