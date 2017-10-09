import numpy as np
import cv2
np.set_printoptions(threshold=np.nan)

cap = cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX

def print_color(event,x,y,flags,param):
    if event == cv2.EVENT_LBUTTONDBLCLK:
        color = np.uint8([[[frame[x,y][0],frame[x,y][1],frame[x,y][2]]]])
        hsv_color = cv2.cvtColor(color,cv2.COLOR_BGR2HSV)
        print hsv_color

while(True):
    ret, frame = cap.read()
    if ret:
        hsv = cv2.cvtColor(frame, cv2.COLOR_RGB2HSV)

        lower_color = np.array([80, 100, 100])
        upper_color = np.array([95, 255, 255])

        mask = cv2.inRange(hsv, lower_color, upper_color)
        res = cv2.bitwise_and(frame, frame, mask=mask)


        
        img = np.zeros((512,512,512), np.uint8)
        cv2.namedWindow('frame')
        cv2.setMouseCallback('frame', print_color)
    
        cv2.imshow('frame', frame)
        cv2.imshow('mask', mask)
        cv2.imshow('res', res)
        if cv2.waitKey(20) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()