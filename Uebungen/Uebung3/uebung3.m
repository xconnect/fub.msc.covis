%%% clean environment
clear all
clc
close all

%%% Uebung 3 %%%

%%% Aufgabe 1: Histogram
% - lade das angehaengte Bild "racecar.png" 
% - konvertiere das Bild in den HSV-Farbraum
% - stelle das Histogramm ueber dem Hue-Kanal fuer das gesamte
%   Bild (Abgabe 1.1) und fuer den Ausschnitt (x,y) = (460, 260) bis
%   (660, 360) (Abgabe 1.2) 
% ! Bitte beachten: In Matlab wird zuerst die Zeile referenziert,
%   also die Y-Koordinate. 

imgRC = imread('racecar.png');
imgRC_hsv_all = rgb2hsv(imgRC);
imgRC_hsv_sub = imgRC_hsv_all(260:360,460:660);

% Abgabe 1.1

% show histogram in a window
figure('NumberTitle','off','Name','histogram 1');
hold on
A1_1 = histogram(imgRC_hsv_all,256);
title('Histogram 1.1')

% Abgabe 1.2

% show histogram in a window
figure('NumberTitle','off','Name','histogram 2');
hold on
A1_2 = histogram(imgRC_hsv_sub,256);
title('Histogram 1.2')


%%% Aufgabe 2: Camshift
% - lies das paper: http://opencv.jp/opencv-1.0.0_org/docs/papers/camshift.pdf
% - implementiere eine Methode, die Dir - gegeben ein Hue-Histogramm - die
%   Objekt-Wahrscheinlichkeitsverteilung für ein neues Bild berechnet (1)
% - waehle aus dem angehängten Video "racecar.avi" ein Einzelbild (2)
% - wende (1) fuer das Objekthistogramm aus Abgabe 1.2 auf (2) an und stelle
%   das Ergebniss dar (Abgabe 2.1)

% Hint: http://de.mathworks.com/help/vision/examples/face-detection-and-tracking-using-camshift.html

% LookUp Histogram erstellen
LookUp = (A1_2.Values)./(sum(A1_2.Values));

% Video laden
vr = VideoReader('racecar.avi');

% Frame aus dem Video holen
vidRC = rgb2hsv(readFrame(vr));

% Hue-Ebene aus dem Frame holen
% .*255 um Werte zwischen 0 und 255 daraus zu machen
% uint8(...) weil Integer oder Logica benoetigt werden
% +1 weil 0-255 Werte => 1 bis 256 Werte
vidRC = uint8(vidRC(:,:,1).*255)+1;

% erstelle neues Bild mit Hilfe des LookUp-Histograms
newpic = LookUp(vidRC);

% plot
figure('NumberTitle','off','Name','bild 2.1');
hold on
A2_1= imshow(newpic.*255);
title('Bild 2.1')
