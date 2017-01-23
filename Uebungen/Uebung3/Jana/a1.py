#!/usr/bin/env python
from __future__ import division
from PIL import Image
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

#lade das angehaengte Bild "racecar.png"
im = Image.open("racecar.png")
print("im.mode: "+str(im.mode))

#konvertiere das Bild in den HSV-Farbraum
#im = im.convert('HSV')

imNP = np.array(im)
imNP = imNP / 255.0
imNP = matplotlib.colors.rgb_to_hsv(imNP)


#stelle das Histogramm ueber dem Hue-Kanal fuer das gesamte Bild (Abgabe 1.1) und fuer den Ausschnitt (x,y) = (460, 260) bis (660, 360) (Abgabe 1.2)
print("imNP.shape: "+str(imNP.shape))
H = imNP[:,:,0]  # 0..360
#S = imNP[:,:,1]  # 0..100
#V = imNP[:,:,2]  # 0..100

#imsize = 921600 # 1280 * 720
#rhist = np.histogram(R, bins=256, range=(0.0, 256.0))[0]
Hhist = np.histogram(H, bins=100, range=(0.0, 1.0))[0]
#Hhist = Hhist / imsize

plt.bar(range(0,100), Hhist)
plt.xlabel("Hue")
plt.ylabel("Number of occurences")
plt.show()


imCropped = im.crop((460, 260, 660, 360))
imNPCropped = np.array(imCropped)
imNPCropped = imNPCropped / 255.0
imNPCropped = matplotlib.colors.rgb_to_hsv(imNPCropped)
Hcropped = imNPCropped[:,:,0]  # 0..360
HhistCropped = np.histogram(Hcropped, bins=100, range=(0.0, 1.0))[0]

plt.bar(range(0,100), HhistCropped)
plt.xlabel("Hue of cropped image")
plt.ylabel("Number of occurences")
plt.show()
