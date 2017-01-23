# want to minimize f = sqrt( sum_{x \in R}( F(x + h) - G(x) )^2 )
# where F(x) is the pixel intensity at coordinate (x[0], x[1]) in the big picture I, and G(x) is the same in the small picture R that we are looking for in picture I

# our I is 640 x 480 pixels big

# Video in individuelle Frames zerhacken mit: avconv -i beedance.avi -f image2 image-%03d.png
# die pngs zu jpgs: for file in $(ls); do convert $file ${file}.jpg; done
# die jpgs zu video: cat *.jpg | avconv -f image2pipe -r 100 -vcodec mjpeg -i - -vcodec libx264 test3.mp4

from __future__ import division
import numpy as np
from scipy.misc import imread, imsave
import os, glob, cv2

width = 640
height = 480


# Parameters for lucas kanade optical flow
lk_params = dict( winSize  = (15,15),
                 maxLevel = 2,
                 criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

# Create some random colors
color = np.random.randint(0,255,(100,3))

# Take first frame and find corners in it
#ret, old_frame = cap.read() #retval, image = cv2.VideoCapture.read([image]). ret verwenden wir nirgendwo.
old_frame = imread('framespng/image-001.png')
old_gray = old_frame[:,:,0] # grab one channel only (all are gray)
with open('corners_fixed.cvs') as f:
    corners = [(int(line.rstrip('\n').split(',')[0]), int(line.rstrip('\n').split(',')[1])) for line in f]
    p0 = np.asarray(corners)
    p0 = p0.reshape(219,1,2)
    #print("p0: "+str(p0))
    #p0 = cv2.goodFeaturesToTrack(old_gray, mask = None, **feature_params) # type(p0): <type 'numpy.ndarray'>, p0.shape: (3, 1, 2)

# Create a mask image for drawing purposes
mask = np.zeros_like(old_frame)

image = 0
for infile in sorted(glob.glob('framespng/*.png')): # iterate over the frames (which are now files)
    image += 1
    print(infile)
    #ret,frame = cap.read()
    if image == 1 or image == 5: # frame nr. 5 is damaged
        continue
    frame = imread(infile)
    frame_gray = frame[:,:,0] # grab one channel only (all are gray)
    
    # calculate optical flow
    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
    
    # Select good points
    good_new = p1[st==1]
    good_old = p0[st==1]
    print("p1: "+str(p1))
    
    print("going to draw the tracks")
    # draw the tracks
    for i,(new,old) in enumerate(zip(good_new,good_old)):
        a,b = new.ravel()
        c,d = old.ravel()
        mask = cv2.line(mask, (a,b),(c,d), color[i].tolist(), 2)
        frame = cv2.circle(frame,(a,b),5,color[i].tolist(),-1)
    img = cv2.add(frame,mask)

    cv2.imshow('frame',img)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break

    # Now update the previous frame and previous points
    old_gray = frame_gray.copy()
    p0 = good_new.reshape(-1,1,2)



"""
for infile in sorted(glob.glob('framespng/*.png')): # iterate over the frames (which are now files)
    print(infile)
    im = imread(infile)
    if i != 1 and i != 5: # frame 5 enthaelt diesen komischen schwarzen Balken. Skippen.
        for corner in range(0, len(corners)):
            h_x = 0
            h_y = 0
            x, y = corners[corner]
            roi = getROI(x, y, width, height) # consider 16x16 pixels around each Harris corner
            for iterCounter in range(0, 100):
                #Gxd =  # derivative of pixel intensity in old frame. Equation
                #Fx =  # pixel intensity in new frame at position x
                
                # equation (10) in paper:
                sum1_x, sum2_x, sum1_y, sum2_y = 0, 0, 0, 0
                #print("roi: "+str(roi))
                for X in roi:
                    if X == None:
                        continue
                    Fxd2_y = deriv_y(X[0], X[1], h_x, h_y, width, height, im)**2 # derivative of pixel intensity in new frame along y axis, at position x, squared
                    Fxd2_x = deriv_x(X[0], X[1], h_x, h_y, width, height, im)**2 # derivative of pixel intensity in new frame along x axis, at position x, squared
                    #wx = [Fxd2_x, Fxd2_y]
                    Fxhd_y = deriv_y(X[0] + h_x, X[1] + h_y, h_x, h_y, width, height, im)
                    Fxhd_x = deriv_x(X[0] + h_x, X[1] + h_y, h_x, h_y, width, height, im)
                    
                    Gx = oldim[X[1], X[0]][0]
                    Fx = im[X[1] + h_y, X[0] + h_x][0]
                    
                    #print("Fxd2_x * Fxhd_x * (Gx - Fx): "+str([Fxd2_x, Fxhd_x, Gx, Fx]))
                    sum1_x += Fxd2_x * Fxhd_x * (Gx - Fx)
                    sum1_y += Fxd2_y * Fxhd_y * (Gx - Fx)
                    sum2_x += Fxd2_x * Fxhd_x**2
                    sum2_y += Fxd2_y * Fxhd_y**2
                h_x = h_x +  (sum1_x / sum2_x)
                h_y = h_y +  (sum1_y / sum2_y)
                
            corners[corner] = (x + h_x, y + h_y)
            print("h after a 100 iterations: "+str([h_x, h_y]))
            
            im[y + h_y, x + h_x] = [255,0,0]
            break# debug
            
        imsave('res/' + infile, im)
        oldim = im
    i += 1
    if i == 3: #debug
        break
"""
