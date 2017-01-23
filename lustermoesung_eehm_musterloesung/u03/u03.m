source('camshift.m');

img      = imread('./racecar.png');
img_sub  = img(230:360, 460:660, :);
fromavi  = glob('./captured/racecar_*.png');
img_h    = rgb2hsv(img)(:, :, 1);
img_s    = rgb2hsv(img)(:, :, 2);
img_v    = rgb2hsv(img)(:, :, 3);
img_hue  = rgb2hue(img);
hist     = hueHist(img_hue);
hist_sub = hueHist(rgb2hue(img_sub(31:130, 21:180, :)));
img_prob = hue2prob(img_hue, hist_sub);

for (i = 1:rows(fromavi))
	frames{i}{1} = imread(fromavi{i});

	if (i == 1)
		frames{i}{2} = camshift(frames{i}{1}, hist_sub, [560, 295, 80, 50]);
	else
		frames{i}{2} = camshift(frames{i}{1}, hist_sub, frames{i-1}{2});
	endif
endfor

mkdir('generated');
h = figure();
imshow(double(img_sub) ./ 255);
saveas(h, './generated/sub', 'png');
h = figure();
imshow(img_h);
saveas(h, './generated/hsv_h', 'png');
h = figure();
imshow(img_s);
saveas(h, './generated/hsv_s', 'png');
h = figure();
imshow(img_v);
saveas(h, './generated/hsv_v', 'png');
h = figure();
imshow(255 .* img_hue);
saveas(h, './generated/img_hue', 'png');
h = figure();
imshow(255 .* img_prob);
saveas(h, './generated/img_prob', 'png');
h = figure();
plot(hist);
xlabel('hue');
ylabel('frequency');
saveas(h, './generated/hist_img', 'png');
h = figure();
plot(hist_sub);
xlabel('hue');
ylabel('frequency');
saveas(h, './generated/hist_sub', 'png');
disp('press any key to continue');
pause();
close('all', 'hidden');
h = figure();
hold();

for (i = 1:rows(fromavi))
	clf();
	f = frames{i};
	imshow(double(f{1}) ./ 255);
	rectangle('Position', [f{2}(1)-f{2}(3), f{2}(2)-f{2}(4), ...
		2*f{2}(3), 2*f{2}(4)], 'LineWidth', 3, 'EdgeColor', 'r');
	saveas(h, sprintf('./generated/frame_%02d', i), 'png');
	disp('press any key to continue');
endfor

close('all', 'hidden');
