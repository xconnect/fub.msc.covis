## @var{width}, @var{height} only provide dimensions for the resulting image - no scaling is
## done here. If the projection looks tiny, try choosing a lens with a greater focallength.

function I = pinhole(xyz, width, height, focallength)
	pr = [xyz(1, :).*focallength./xyz(3, :); xyz(2, :).*focallength./xyz(3, :)];
	pr(1, :) .+= width/2.0;
	pr(2, :) .+= height/2.0;
	pr = int32(round(pr));
	I  = ones(height, width);

	for (xy = pr)
		if ((xy > [0; 0]) && (xy <= [width; height]))
			I(xy(2), xy(1)) = 0;
		endif
	endfor
	
	I = logical(I);
endfunction

## With unknown (and therefore assumed as orthogonal) direction of the projection and
## world-reference-frame == cam-reference-frame this projection is trivial.
## The resulting image is scaled to fit into a @var{height} x @var{width} sized box.

function I = parallel(xyz, width, height)
	pr = [xyz(1, :).-min(xyz(1, :)); xyz(2, :).-min(xyz(2, :))];
	pr(1, :) .*= (width-1)/max(pr(1, :));
	pr(2, :) .*= (height-1)/max(pr(2, :));
	pr       .+= 1;
	pr = int32(round(pr));
	I = ones(height, width);
	
	for (xy = pr)
		I(xy(2), xy(1)) = 0;
	endfor

	I = logical(I);
endfunction
