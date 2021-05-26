# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
from tqdm import tqdm
import os
from os import getcwd


def convert(size, box):
    dw = 1. / (size[0])
    dh = 1. / (size[1])
    x = (box[0] + box[1]) / 2.0 - 1
    y = (box[2] + box[3]) / 2.0 - 1
    w = box[1] - box[0]
    h = box[3] - box[2]
    x = x * dw
    w = w * dw
    y = y * dh
    h = h * dh
    return x, y, w, h


def convert_annotation(image_id):
    # try:
    in_file = open('VOCData/images/{}.xml'.format(image_id), encoding='utf-8')
    out_file = open('VOCData/labels/{}.txt'.format(image_id),
                    'w', encoding='utf-8')
    tree = ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    w = int(size.find('width').text)
    h = int(size.find('height').text)
    for obj in root.iter('object'):
        difficult = obj.find('difficult').text
        cls = obj.find('name').text
        if cls not in classes or int(difficult) == 1:
            continue
        cls_id = classes.index(cls)
        xmlbox = obj.find('bndbox')
        b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text),
             float(xmlbox.find('ymax').text))
        b1, b2, b3, b4 = b
        # 鏍囨敞瓒婄晫淇
        if b2 > w:
            b2 = w
        if b4 > h:
            b4 = h
        b = (b1, b2, b3, b4)
        bb = convert((w, h), b)
        out_file.write(str(cls_id) + " " +
                       " ".join([str(a) for a in bb]) + '\n')
    # except Exception as e:
    #     print(e, image_id)


if __name__ == '__main__':

    sets = ['train', 'val']

    image_ids = [v.split('.')[0]
                 for v in os.listdir('VOCData/images/') if v.endswith('.xml')]

    split_num = int(0.95 * len(image_ids))

    classes = ['cube', 'ball', 'cylinder', 'human body', 'tyre', 'circle cage', 'square cage', 'metal bucket']

    if not os.path.exists('VOCData/labels/'):
        os.makedirs('VOCData/labels/')

    list_file = open('train.txt', 'w')
    for image_id in tqdm(image_ids[:split_num]):
        list_file.write('VOCData/images/{}.bmp\n'.format(image_id))
        convert_annotation(image_id)
    list_file.close()

    list_file = open('val.txt', 'w')
    for image_id in tqdm(image_ids[split_num:]):
        list_file.write('VOCData/images/{}.bmp\n'.format(image_id))
        convert_annotation(image_id)
    list_file.close()
