# dumpexif
A simple tool to extract/strip/re-assemble exif datas from jpeg files.

# usage

dump exif from jpeg:

    dumpjpeg IMAGE.jpg IMAGE.exif
  
strip exif off from jpeg

    stripjpeg IMAGE.jpg STRIP.jpg
  
re-assemble jpeg from a jpeg and a dumped exif

    exifjpeg IMAGE.jpg IMAGE.exif NEWIMAGE.jpg
