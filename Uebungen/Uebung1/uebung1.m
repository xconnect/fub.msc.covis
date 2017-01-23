%%% Uebung 1 %%%

%%% clean environment
clear all
clc
close all

% Aufgabe 1

% import image
image = importdata('image.jpg');

% show image in a window
figure('NumberTitle','off','Name','image');
hold on
imshow(image);
title('image')


% create a subimage
subimage = image(50:110,110:170,:);

% show subimage in a window
figure('NumberTitle','off','Name','subimage');
hold on
imshow(subimage);
title('subimage')


% only use the R (red) channel of the image
imageR = image(:,:,1);

% show only the red channel of the image in a window
figure('NumberTitle','off','Name','image red channel');
hold on
imshow(imageR);
title('image red channel')


% flip the image (first x-axis, then y-axis)
imageFlipX = flip(image,1);
imageFlipY = flip(imageFlipX,2);

% show flipped image in a window
figure('NumberTitle','off','Name','image first flip x-axis');
hold on
imshow(imageFlipX);
title('image flip x-axis')

figure('NumberTitle','off','Name','image then flip y-axis');
hold on
imshow(imageFlipY);
title('image flip y-axis')


% change rgb to gray scale
imageG = rgb2gray(image);
% invert the image color (gray scale)
imageInvert = max(imageG(:))-imageG;

% show image (gray scale) in a window
figure('NumberTitle','off','Name','image gray scale');
hold on
imshow(imageG);
title('image gray scale')

% show inverted image (gray scale) in a window
figure('NumberTitle','off','Name','inverted image gray scale');
hold on
imshow(imageInvert);
title('inverted image gray scale')