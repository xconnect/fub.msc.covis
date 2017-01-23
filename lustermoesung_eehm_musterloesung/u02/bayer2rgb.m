function I = bayer2rgb(img)
	I   = uint32(zeros(rows(img), columns(img), 3));	
	rgb = uint32(zeros(rows(img), columns(img), 3));

	% save each color to a distinct plane
	rgb(2:2:rows(img), 1:2:columns(img), 1) = img(2:2:rows(img), 1:2:columns(img));
	rgb(1:2:rows(img), 1:2:columns(img), 2) = img(1:2:rows(img), 1:2:columns(img));
	rgb(2:2:rows(img), 2:2:columns(img), 2) = img(2:2:rows(img), 2:2:columns(img));
	rgb(1:2:rows(img), 2:2:columns(img), 3) = img(1:2:rows(img), 2:2:columns(img));

	% every source pixel influences 9 target pixels
	I(1:rows(img)-1, 1:columns(img)-1, :) .+= rgb(2:rows(img)+0, 2:columns(img)+0, :);
	I(1:rows(img)-1, 1:columns(img)+0, :) .+= rgb(2:rows(img)+0, 1:columns(img)+0, :);
	I(1:rows(img)-1, 2:columns(img)+0, :) .+= rgb(2:rows(img)+0, 1:columns(img)-1, :);
	I(1:rows(img)+0, 1:columns(img)-1, :) .+= rgb(1:rows(img)+0, 2:columns(img)+0, :);
	I(1:rows(img)+0, 1:columns(img)+0, :) .+= rgb(1:rows(img)+0, 1:columns(img)+0, :);
	I(1:rows(img)+0, 2:columns(img)+0, :) .+= rgb(1:rows(img)+0, 1:columns(img)-1, :);
	I(2:rows(img)+0, 1:columns(img)-1, :) .+= rgb(1:rows(img)-1, 2:columns(img)+0, :);
	I(2:rows(img)+0, 1:columns(img)+0, :) .+= rgb(1:rows(img)-1, 1:columns(img)+0, :);
	I(2:rows(img)+0, 2:columns(img)+0, :) .+= rgb(1:rows(img)-1, 1:columns(img)-1, :);

	% normalize
	I .*= (rgb == 0);
	I .+= rgb;
	mask = ones(rows(img), columns(img), 3);
	mask(1:2:rows(img), 1:2:columns(img), 1) = 2;
	mask(1:2:rows(img), 1:2:columns(img), 3) = 2;
	mask(1:2:rows(img), 2:2:columns(img), 1) = 4;
	mask(1:2:rows(img), 2:2:columns(img), 2) = 4;
	mask(2:2:rows(img), 1:2:columns(img), 2) = 4;
	mask(2:2:rows(img), 1:2:columns(img), 3) = 4;
	mask(2:2:rows(img), 2:2:columns(img), 1) = 2;
	mask(2:2:rows(img), 2:2:columns(img), 3) = 2;
	I ./= mask;
	I = uint8(I);
endfunction
