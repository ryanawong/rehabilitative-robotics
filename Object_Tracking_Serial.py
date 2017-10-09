import numpy as np
import cv2
import imutils
import serial
import time
from skimage import measure

cap = cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX
ser = serial.Serial('COM8', 9600)
print 'Connected'

#for i in range(5):
#    print 'Ping'
#    bluetooth.write(b"BOOP " + str.encode(str(i)))
#    input_date = bluetooth.readline()
#    print(input_date.decode())
#    time.sleep(0.1)

while(True):
    ret, frame = cap.read()
    if ret:
        hsv = cv2.cvtColor(frame, cv2.COLOR_RGB2HSV)

        lower_color = np.array([80, 100, 100])
        upper_color = np.array([95, 255, 255])

        mask = cv2.inRange(hsv, lower_color, upper_color)
        res = cv2.bitwise_and(frame, frame, mask=mask)
        labels = measure.label(res, neighbors=8, background=0)
        mask = np.zeros(res.shape, dtype="uint8")

        position = 0
        if np.count_nonzero(res) > 100:
            nonzero_pixels = np.transpose(np.nonzero(res))
            col_array = []
            for ar in nonzero_pixels:
                col_array.append(ar[1])
            standard_deviation = np.std(col_array)
            average = np.mean(col_array)

            lower = min(average - 2 * standard_deviation, 0)
            upper = max(average + 2 * standard_deviation, 640)

            total = 0
            count = 0
            for col in col_array:
                if col > lower and col < upper:
                    total += col
                    count += 1
            position = total / count
            print(position, np.count_nonzero(res))

        # STOP = 1, LEFT = 2, RIGHT = 3, FORWARD = 4
        if np.count_nonzero(res) < 500:
            cv2.putText(frame, "Cannot Find Object", (0, 460),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)
            ser.write(str(1))
        else:
            if position < 225:
                cv2.putText(frame, "Turn Left", (0, 460),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)
                ser.write(str(2))
            elif position > 425:
                cv2.putText(frame, "Turn Right", (0, 460),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)
                ser.write(str(3))
            else:
                cv2.putText(frame, "Go Straight", (0, 460),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)
                ser.write(str(4))
    
        cv2.imshow('frame', frame)
        cv2.imshow('mask', mask)
        cv2.imshow('res', res)
        if cv2.waitKey(20) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()