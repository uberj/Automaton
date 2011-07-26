import Image
import ImageDraw
import numpy
import sys


#Colors
white = 255
black = 0
try:
    auto_file = open(sys.argv[1])
except Exception,e:
    print "File not found"

# Find out how many cells in a Latice and Num generations
lines = auto_file.readlines()
G = len(lines) # Number of generations
N = len(lines[0].strip()) # Cells in a Lattice
auto_file.seek(0) # Reset file pointer

# A ball-park estimate
width = 1000
# Determine appropriate sizes for cells
c_w = width/N # cell_width
width = c_w*N # We lost some precision with integer division
#c_h = height/G # cell_height
c_h = c_w
height = c_w*G # determine height


img = Image.new('L',(width,height))
draw = ImageDraw.Draw(img)

print "Number of Generations: "+str(G)
print "Latice size: "+str(N)
print "Cell height: "+str(c_h)
print "Cell width: "+str(c_w)
print "Image Height: "+str(height)
print "Image Width: "+str(width)

g_c = c_c = 0 # Generation count = Cell_count = 0
for line in auto_file.readlines():
    for cell in line.strip():
        if cell is '0':
            x1,y1 = c_c*c_w, g_c*c_h
            x2,y2 = (c_c + 1) * c_w, (g_c + 1)*c_h
            draw.rectangle([(x1,y1) ,(x2,y2)  ], black )
        elif cell is '1':
            x1,y1 = c_c*c_w, g_c*c_h
            x2,y2 = (c_c + 1) * c_w, (g_c + 1)*c_h
            draw.rectangle([(x1,y1) ,(x2,y2)  ], white )
        else:
            print "Invalid Character"
            print "exiting on Error...."
        c_c = c_c + 1

    c_c = 0
    g_c = g_c + 1
img.show()
img.save("automaton"+str(sys.argv[1])+".JPEG")
