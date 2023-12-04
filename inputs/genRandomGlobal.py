class Cord:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Tile:
    def __init__(self, name, llx, lly, width, height):
        self.name = name
        self.w = width
        self.h = height 
        self.ll = Cord(llx, lly)
        self.lr = Cord(llx + width, lly)
        self.ul = Cord(llx, lly + height)
        self.ur = Cord(llx + width, lly + height)

    def display(self):
        print(f"{self.name}: ({self.ll.x}, {self.ll.y}), W/H = {self.w}/{self.h}, ({self.ur.x}, {self.ur.y})")

    def intersects(self, other):

        return (   other.ll.x < self.ur.x and
                   other.ur.x > self.ll.x and
                   other.ur.y > self.ll.y and
                   other.ll.y < self.ur.y)

if __name__ == '__main__':

    CHIP_W = 11250
    CHIP_H = 9700

    a = Tile("one", 5, 6, 3, 2)
    b = Tile("dus", 7, 4, 2, 3)

    a.display()
    b.display()
    print(a.intersects(b))
    print(b.intersects(a))
