import pyb, sensor, image, time, math
enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.skip_frames(time = 2000)
clock = time.clock()

# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.

number = 0
xx1 = 0.0
yy1 = 0.0
xx2 = 0.0
yy2 = 0.0
bool_average = 1

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
   clock.tick()
   img = sensor.snapshot()
   if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

   # `merge_distance` controls the merging of nearby lines. At 0 (the default), no
   # merging is done. At 1, any line 1 pixel away from another is merged... and so
   # on as you increase this value. You may wish to merge lines as line segment
   # detection produces a lot of line segment results.

   # `max_theta_diff` controls the maximum amount of rotation difference between
   # any two lines about to be merged. The default setting allows for 15 degrees.

   ROI=(5,90,180,30)
   img.draw_rectangle(ROI)
   for l in img.find_line_segments(ROI,merge_distance = 50, max_theta_diff = 5):
      img.draw_line(l.line(), color = (255, 0, 0))
      print_line = (l.x1(),l.y1(),l.x2(),l.y2())
      if((l.x2()-l.x1()) < 10):
            number = number + 1
      if(l.x2()-l.x1() < 10 and number < 50):
            xx1 = xx1 + l.x1()
            yy1 = yy1 + l.y1()
            xx2 = xx2 + l.x2()
            yy2 = yy2 + l.y2()
            print("x1:%f, y1:%f, x2:%f, y2:%f\r\n" % print_line)
            print("%d \r\n" % number)
      if(number == 50):
            bool_average = 0
      if(bool_average == 0 and number == 51):
            xx1 = xx1/50
            yy1 = yy1/50
            xx2 = xx2/50
            yy2 = yy2/50
            line_average = (xx1, yy1, xx2, yy2)
            uart.write(("/line/run %f %f %f %f\r\n" % line_average).encode())
      # print("xx1:%f, yy1:%f, xx2:%f ,yy2:%f" % line_average)
