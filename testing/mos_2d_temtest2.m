# compares e field
data = load('mos_2d_temtest2.dat');

len = size(data)
len = len(:,1)

compare = zeros(len, 8);
errors = zeros(len, 8);
#for i = 1:1
for i = 1:len
  x0 = data(i,1);
  x1 = data(i,2);
  x2 = data(i,3);
  y0 = data(i,4);
  y1 = data(i,5);
  y2 = data(i,6);
  p0 = data(i,7);
  p1 = data(i,8);
  p2 = data(i,9);
  ex = data(i,10);
  ey = data(i,11);
  exn0 = data(i,12);
  eyn0 = data(i,13);
  exn1 = data(i,14);
  eyn1 = data(i,15);
  exn2 = data(i,16);
  eyn2 = data(i,17);

  m = [(x0 - x1) (y0 - y1); (x0 - x2) (y0 - y2)];
  minv = inv(m);

  f = minv *  [(p1-p0); (p2 - p0)];
  f0 = minv *  [-1; -1];
  f1 = minv *  [1; 0];
  f2 = minv *  [0; 1];

  compare(i,:) = [f', f0', f1', f2'];
#  data(i,:)
end

for i=1:8
  j = 9+i;
  errors(:,i) = abs((compare(:,i) - (data(:,j)))./(abs(compare(:,i) + data(:,j)) + 1e-3)); 
end

# a small subset for comparison
d=538:560

for i=1:8
figure(i)
hold off
title(num2str(i))
plot(data(d,9+i))
hold on
plot(compare(d,i),'+');
hold off
end



