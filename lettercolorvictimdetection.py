import serial
import cv2
import numpy as np
from math import sqrt
import time

ser = serial.Serial("/dev/ttyAMA0", 9600)

lower_red = np.array([100,100,0])
upper_red = np.array([179,255,255])
lower_yellow = np.array([0, 112, 115])
upper_yellow = np.array([40,190,204])
lower_green = np.array([40,50,0])
upper_green = np.array([90,255,255])


key = [[2, 1, 2], [1, 1, 1], [2, 2, 1]]
chars = ["h", "s", "u"]

images = np.loadtxt("images2.txt").astype('float32')
letters = np.loadtxt("letters2.txt").astype('float32')
letters = letters.reshape(letters.shape[0], 1)

knn = cv2.ml.KNearest_create()
knn.train(images, cv2.ml.ROW_SAMPLE, letters)

cams = [cv2.VideoCapture(1), cv2.VideoCapture(0)]

def isH(vd):
    ret, img = vd.read()
    img = cv2.flip(img, -1)
#     cv2.imshow("img"+str(cams.index(vd)), img)
    img = cv2.GaussianBlur(img, (3, 3), 0)

    #_, thresh = cv2.threshold(cv2.cvtColor(img.copy(), cv2.COLOR_BGR2GRAY), 0, 255, cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
    _, thresh = cv2.threshold(cv2.cvtColor(img.copy(), cv2.COLOR_BGR2GRAY), 75, 255, cv2.THRESH_BINARY_INV)
#     cv2.imshow("thresh", thresh)

    contours, h = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    minArea = 3900
    maxArea = 40000
    for contour in contours:
        if cv2.contourArea(contour) < minArea:
            continue
        if cv2.contourArea(contour) > maxArea:
            continue
        
        rect = cv2.minAreaRect(contour)
        box = cv2.boxPoints(rect) #returns points as floats
        box = np.int0(box) #returns ints
        angle = rect[2]

        #boundingrect
        x,y,w,h = cv2.boundingRect(contour)
        img = cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)
        #slice
        slice1 = thresh[y:(y+h), x:(x+w)]
        ###cv2.imshow("slice1", slice1)
        cols = slice1.shape[1]
        rows = slice1.shape[0]
        #create space
        dia = int(sqrt((w*w) + (h*h)))
        rot = np.zeros((dia+50, dia+50), dtype='uint8')
        #rotate
        M = cv2.getRotationMatrix2D((slice1.shape[0]/2,slice1.shape[1]/2),angle,1)
        rot[0:cols, 0:rows] = cv2.warpAffine(slice1,M,(slice1.shape[0],slice1.shape[1]))
        ###cv2.imshow("rot", rot)
        #slice
        contours, h = cv2.findContours(rot, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) == 0:
            continue
        csorted = sorted(contours, key=lambda x: cv2.contourArea(x))
        x,y,w,h = cv2.boundingRect(csorted[-1])
        slice2 = rot[y:(y+h), x:(x+w)]
        ###cv2.imshow("slice2", slice2)
        
        
        for i in range (4):
            
            M = cv2.getRotationMatrix2D((slice2.shape[0]/2,slice2.shape[1]/2),90,1)
            rot = cv2.warpAffine(slice2,M,(slice2.shape[0],slice2.shape[1]))
            ###cv2.imshow("rot" + str(i), rot)
            
            #extra crop
            '''
            r, c = rot.shape
            crop = 0.01
            remr = int(r * crop)
            remc = int(c * crop)
            rot = rot[remr:r-remr, remc:c-remc]
            '''
            
            slice2 = rot #for debug
            rot = cv2.resize(rot, (20, 30))
            rot = np.reshape(rot, (1, 600))
            _, results, neighbors, dist = knn.findNearest(rot.astype('float32'), 7)
                
            if i == 0:
                usedSliced = slice2
                usedNeighbors = neighbors
                lowDist = dist[0][0]
                letter = results[0][0]
            elif dist[0][0] < lowDist:
                usedSliced = slice2
                usedNeighbors = neighbors
                lowDist = dist[0][0]
                letter = results[0][0]
                    
            #print(lowDist)
            #print(chr(letter))
   
        if lowDist > 8000000:
            continue
   
        #cv2.imshow(chr(letter), usedSliced)
        img = cv2.drawContours(img,[box],0,(0,0,255),2)
            
#         #double check with slicing method:
#         top = usedSliced[0:usedSliced.shape[0]//3, 0:usedSliced.shape[1]];
#         middle = usedSliced[(usedSliced.shape[0]//3+1):(usedSliced.shape[0]//3)*2, 0:usedSliced.shape[1]];
#         bottom = usedSliced[((usedSliced.shape[0]//3)*2+1):usedSliced.shape[0], 0:usedSliced.shape[1]];
#         
#         nContours = []
#         
#         ###cv2.imshow("top", top
#         scontours, _ = cv2.findContours(top, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#         nContours.append(len(scontours))
#         
#         ###cv2.imshow("middle", middle)
#         scontours, _ = cv2.findContours(middle, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#         nContours.append(len(scontours))
#         
#         ###cv2.imshow("bottom", bottom)
#         scontours, _ = cv2.findContours(bottom, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#         nContours.append(len(scontours))
#         
#         cont = True
#         for n in key:
#             if n == nContours:
#                 if chars[key.index(n)] == chr(letter):
#                     cont = False
#                     break
#                 else:
#                     break
#         if cont:
#             cv2.putText(img, chr(letter).upper(), tuple(box[3]), cv2.FONT_HERSHEY_DUPLEX, 2, (255, 255, 255))
#             cv2.putText(img, f"{lowDist:,}", tuple(box[1]), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 255))
#             continue

        print(cv2.contourArea(contour))
        if letter == 104:
            print("serial h")
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"h\n")
        if letter == 115:
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"s\n")
        if letter == 117:
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"u\n")
        
        cv2.putText(img, f"{lowDist:,}", tuple(box[1]), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 255))
        cv2.putText(img, chr(letter).upper(), tuple(box[3]), cv2.FONT_HERSHEY_DUPLEX, 2, (0, 0, 255))
#         cv2.imshow("img"+str(cams.index(vd)), img)
        print(chr(letter), letter, usedNeighbors)
        print("=====")
    #end for contour in contours
        

#     cv2.imshow("img"+str(cams.index(vd)), img)
    #if #cv2.waitKey(1) & 0xFF == ord('q'):
        #break
    letter = 0

def isGreen(vd):
    _,frame = vd.read()
    frame = cv2.flip(frame, -1)
#     cv2.imshow("frame"+str(cams.index(vd)), frame)
    
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    mask = cv2.inRange (hsv, lower_red, upper_red)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    contours = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[0]
    
    for cnt in contours:
        area = cv2.contourArea(cnt)
#         print(area)
        if(area > 6000): #victims are 16cm^2
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"r\n")
#             cv2.imshow('frame', frame)
#             cv2.imshow('mask', mask)
#             cv2.imshow('res',res)
#             
#             print(area)
#     
#             cv2.waitKey(0)
            cv2.destroyAllWindows()
    
    mask = cv2.inRange (hsv, lower_yellow, upper_yellow)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    contours = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[0]
    
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if(area > 6000): #victims are 16cm^2
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"y\n")
#             cv2.imshow('frame2', frame)
#             cv2.imshow('mask2', mask)
#             cv2.imshow('res2',res)
#             
#             print(area)
#     
#             cv2.waitKey(0)
            cv2.destroyAllWindows()
    
    mask = cv2.inRange (hsv, lower_green, upper_green)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    contours = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[0]
    
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if(area > 6000): #victims are 16cm^2
            if vd == cams[0]:
                ser.write(b"R\n")
            else:
                ser.write(b"L\n")
            ser.write(b"g\n")
            print("green")
            
#             cv2.imshow('frame3', frame)
#             cv2.imshow('mask3', mask)
#             cv2.imshow('res3',res)
#             
#             print(area)
#             
#             cv2.waitKey(0)
            cv2.destroyAllWindows()
            

while True:
    
    for cam in cams:
        isH(cam)
        isGreen(cam)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    


vd.release()
vd2.release()
cv2.destroyAllWindows()

