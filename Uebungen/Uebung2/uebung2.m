%%% clean environment
clear all
clc
close all

%%% Uebung2 %%%

%%% Aufgabe 2: Lochkamera

% Lochkamera
Weltpunkte = load('pointdata_3d.csv');

% Flaeche der Lochkamera-Fotoleinwand: 200x200
breite = 150;
hoehe  = 150;

% 2 Brennweiten
brennweite1 = 3000;
brennweite2 = 5000;

WPx = Weltpunkte(1,:);
WPy = Weltpunkte(2,:);
WPz = Weltpunkte(3,:);

Bildpunkte1 = [ WPx.*brennweite1./WPz ;
                WPy.*brennweite1./WPz ];

Bildpunkte2 = [ WPx.*brennweite2./WPz ;
                WPy.*brennweite2./WPz ];

Bildpunkte1(1,:) = Bildpunkte1(1,:) + breite/2.0;
Bildpunkte1(2,:) = Bildpunkte1(2,:) + hoehe/2.0;
Bildpunkte1 = uint8(Bildpunkte1);

Bildpunkte2(1,:) = Bildpunkte2(1,:) + breite/2.0;
Bildpunkte2(2,:) = Bildpunkte2(2,:) + hoehe/2.0;
Bildpunkte2 = uint8(Bildpunkte2);

img1 = ones(breite,hoehe);
img2 = ones(breite,hoehe);

for koord = Bildpunkte1
    if (koord(1) >= 0 && koord(1) <= breite && koord(2) >= 0 && koord(2) <= hoehe)
        img1(koord(2),koord(1)) = 0;
    end
end

for koord = Bildpunkte2
    if (koord(1) >= 0 && koord(1) <= breite && koord(2) >= 0 && koord(2) <= hoehe)
        img2(koord(2),koord(1)) = 0;
    end
end

%img1 = logical(img1);
%img2 = logical(img2);

% show image in a window
figure('NumberTitle','off','Name','sub image bayer raw');
hold on
imshow(img1);
title('image 1: breenweite 3000, 150x150px')

% show image in a window
figure('NumberTitle','off','Name','image 2');
hold on
imshow(img2);
title('image 2: brennweite 5000, 150x150px')

%%% Aufgabe 3

A = importdata('image_bayer_raw.csv');
imgRaw = uint8(A(:,1:1282));
%imgRGB = demosaic(imgRaw,'gbrg'); % scheint ein guter Filter zu sein...

% Bayer-Filter
G = imgRaw(2:2:721,2:2:1282);
B = imgRaw(1:2:720,2:2:1282);
R = imgRaw(2:2:721,1:2:1282); 

% Bild zusammenbauen
imgRGB(:,:,1) = R;
imgRGB(:,:,2) = G;
imgRGB(:,:,3) = B;

% Unterbild bestimmen
subimgRGB = imgRGB(180:260,350:440,:);

% show image in a window
figure('NumberTitle','off','Name','sub image bayer raw');
hold on
imshow(imgRGB);
%imshow(subimgRGB);
title('sub image of image\_bayer\_raw')