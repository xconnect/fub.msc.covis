function valid = saterr(hsv)
	valid = hsv(:, :, 2) > 0.2;
endfunction

% TODO: was Besseres finden
function s = boxsize(m00, m01, m10, m20, m02)
	s = 3.5 * sqrt(m00);
endfunction

function hue = rgb2hue(rgb, errfun = @saterr)
	hsv = rgb2hsv(rgb);
	hue = round(254 .* hsv(:, :, 1));
	hue = 1 .+ uint16(max(255.*(1.-errfun(hsv)), hue));
endfunction

function hist = hueHist(hue)
	hist = sum(histc(hue, 1:256, 2));
	hist(256) = 0;
	hist ./= sum(hist);
endfunction

function prob = hue2prob(hue, lut)
	prob = lut(hue);
endfunction

function [x, y, dx, dy, m00, m10, m01, m20, m02] = ...
		meanshift(rgb, lut, window, errfun = @saterr)
	prob  = hue2prob(rgb2hue(rgb, errfun), lut);
	x     = window(1);
	y     = window(2);
	dx    = window(3);
	dy    = window(4);
	m00_2 = 0;
	m00   = sum(sum(prob(y-dy:y+dy, x-dx:x+dx)))

	while (m00 > m00_2)
		w     = prob(y-dy:y+dy, x-dx:x+dx);
		m10   = sum(sum(w, 1) .* (1:(2*dx+1)));
		m01   = sum(sum(w, 2) .* transpose(1:(2*dy+1)));
		x    += round(m10/m00) - dx - 1;
		y    += round(m01/m00) - dy - 1;
		x     = min(max(x, dx+1), columns(rgb)-dx);
		y     = min(max(y, dy+1), rows(rgb)-dy);
		m00_2 = m00;
		m00   = sum(sum(prob(y-dy:y+dy, x-dx:x+dx)));
	endwhile

	m20 = sum(sum(w, 1) .* (1:(2*dx+1)).^2);
	m02 = sum(sum(w, 2) .* transpose(1:(2*dy+1)).^2);
endfunction

function window = camshift(rgb, lut, window, errfun = @saterr, sfun = @boxsize)
	[x, y, dx, dy, m00, m10, m01, m20, m02] = ...
		meanshift(rgb, lut, window, errfun);
	s  = sfun(m00, m01, m10, m20, m02);
	ar = (m20*(m00/m10)^2) / (m02*(m00/m01)^2);
	window = [x, y, ...
		round(min(min(s*ar, x-1), columns(rgb)-x)), ...
		round(min(min(s/ar, y-1), rows(rgb)-y))];
endfunction
