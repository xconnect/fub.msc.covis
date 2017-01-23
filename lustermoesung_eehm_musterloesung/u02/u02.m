source('bayer2rgb.m')
source('world2cam.m')

function task2()
	pts  = importdata('pointdata_3d.csv');
	fl1  = pinhole(pts, 128, 128, 5000);
	fl2  = pinhole(pts, 128, 128, 2500);
	fl12 = fl1 & fl2;
	p    = parallel(pts, 128, 128);
	imshow(fl1);
	imwrite(fl1, 'capture_2-1.png');
	figure();
	imshow(fl2);
	imwrite(fl2, 'capture_2-2.png');
	figure();
	imshow(fl12);
	imwrite(fl12, 'capture_2-3.png');
	figure();
	imshow(p);
	imwrite(p, 'capture_2-4.png');
	pause();
endfunction

function task3()
	img     = importdata('image_bayer_raw.csv');
	img_raw = img(:, 1:1282);
	img_rgb = bayer2rgb(img_raw);
	img_sub = img_rgb(350:440, 180:260, :);
	imshow(img_rgb);
	imwrite(img_rgb, 'capture_3-1.png');
	figure();
	imshow(img_sub);
	imwrite(img_sub, 'capture_3-2.png');
	pause();
endfunction

task2();
task3();
