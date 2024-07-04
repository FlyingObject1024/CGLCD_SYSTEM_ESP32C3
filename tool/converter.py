import os
import sys
import cv2
import numpy as np
from moviepy.editor import ImageSequenceClip

np.set_printoptions(threshold=np.inf)

base_dir = f"{os.path.dirname(__file__)}"

filename = "imagemap.gif"

space_clf_file = f"{base_dir}/space_svm_clf.pickle"

filepath = f"{base_dir}/{filename}"
outputfilepath = f"{base_dir}/output.txt"

gif = cv2.VideoCapture(filepath)
fps = gif.get(cv2.CAP_PROP_FPS)  # fpsは１秒あたりのコマ数

images = []
i = 0
while True:
    is_success, img = gif.read()
    if not is_success:
        break

    images.append(img)
    i += 1

print("frame_num: "+ str(len(images)))

for t in range(len(images)):
    #print(images[t])
    f = open(outputfilepath, 'w')
    f.write('const uint16_t ' + filename.split('.')[0] +'['+str(len(images[t]))+']['+str(len(images[t][0]))+'] PROGMEM = {\n')
    for i in range(len(images[t])):
        f.write('    {')
        for j in range(len(images[t][i])):
            color = 0
            color = images[t][i][j][0] >> 3
            color |= ((images[t][i][j][1] & 0xFC) << 3)
            color |= ((images[t][i][j][2] & 0xF8) << 8)
            f.write('{:#06x}, '.format(color))
    
        f.write('},\n')
    
    f.write('};\n')
    f.close()



#cv2.destroyAllWindows()
