#!/usr/bin/env python

import numpy as np
from PIL import Image

#* Oeffne das Testbild "image.jpg" und stelle es in einem Fenster dar. (Abgabe 1.1)
im = Image.open("image.jpg")
im.show()

#* Extrahiere ein 60 x 60 Pixel grosses Unterbild von Koordinate (50,110) und stelle es in einem Fenster dar. (Abgabe 1.2)
imCropped = im.crop((50, 110, 110, 170))
imCropped.show()

#* Stelle nur den Rotkanal dar. (Abgabe 1.3)
imNP = np.array(im)
imNP[:,:,1] *=0
imNP[:,:,2] *=0
imRed = Image.fromarray(imNP)
imRed.show()

#* Spiegele das Bild an der x-Achse und dann an der y-Achse (Abgabe 1.4)
imFlippedX = im.transpose(Image.FLIP_LEFT_RIGHT)
imFlippedX.show()
imFlippedY = im.transpose(Image.FLIP_TOP_BOTTOM)
imFlippedY.show()
imFlippedXY = imFlippedX.transpose(Image.FLIP_TOP_BOTTOM)
imFlippedXY.show()

#* Wandle das Bild in ein Grauwertbild um und stelle es invertiert dar (Abgabe 1.5)
imNP = np.array(im)
imNPGray = np.dot(imNP[...,:3], [0.299, 0.587, 0.144])
imGray = Image.fromarray(imNPGray)
imGray.show()
#ImageOps.invert(imGray) #  eine Alternative mit import ImageOps
imNPInverted = imNPGray * (-1) + 255
imInverted = Image.fromarray(imNPInverted)
imInverted.show()