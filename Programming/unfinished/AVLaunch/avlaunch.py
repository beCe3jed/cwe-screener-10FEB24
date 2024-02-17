import os


global rotation_count # tracks the number of rotations that occur

# Our class for our node object
class Node(object):
    # when adding a node, we set its values accoringly
    def __init__(self, value, flag):
        self.value = value
        self.flag = flag
        self.left = None
        self.right = None
        self.height = 1     # used to recursively add height

class Tree(object):
    def __init__(self):
        self.rotations = 0

    # will be used recursively returning a value of 0 or 1 in each instance
    def getHeight(self, root):
        if not root:
            return 0
        else:
            return root.height

    # uses the getHeight() function to find the difference between left and right subtrees, giving the current root's balance
    def getBalance(self, root):
        if not root:
            return 0
        else:
            return self.getHeight(root.left) - self.getHeight(root.right)
    
    def printFlag(self, root):
        return root.flag

    # recursively executes finding the place to insert and then updates the height, and balance
    # root: root of subtree, value: number to be compared
    def insert(self, root, value, flag):

        if not root:
            debug_root_value = None
        else:
            debug_root_value = root.value 
        # print("Root: {}, value: {}, flag: {}".format(debug_root_value, value, flag))

        # search tree for placement
        if not root: # insert node here
            return Node(value, flag)
        elif value < root.value: # go to the left sub-tree
            root.left = self.insert(root.left, value, flag)
        elif value > root.value: # go to the right sub-tree
            root.right = self.insert(root.right, value, flag)
        else: 
            print("The values are equal!! {} vs. {}".format(value, root.value))
        

        # update height by getting the greater value of both sub-trees
        root.height = 1 + max(self.getHeight(root.left), self.getHeight(root.right))

        # balance the tree based off of heights of subtrees
        balance = self.getBalance(root)

        # balance accordingly to the scenario
        if balance > 1 and value < root.left.value: # execute right-rotate
            return self.rotateRight(root)
        elif balance < -1 and value > root.right.value: # execute left-rotate
            return self.rotateLeft(root)
        elif balance > 1 and value > root.left.value: # rotate left, then right
            # self.rotations += 2
            root.left = self.rotateLeft(root.left)
            return self.rotateRight(root)
            #print!
        elif balance < -1 and value < root.right.value: # rotate right, then left
            # self.rotations += 2
            root.right = self.rotateRight(root.right)
            return self.rotateLeft(root)
            #print!

        return root
        
    def rotateRight(self, old_root):
        self.rotations += 1
        # print("Rotation count: {} ".format(self.rotations))
        # make our variable to represent the nodes to be rotated
        new_root = old_root.left
        temp = new_root.right # temporarily hold this value

        # execute rotation
        new_root.right = old_root
        old_root.left = temp

        # update heights
        old_root.height = 1 + max(self.getHeight(old_root.left), self.getHeight(old_root.right))
        new_root.height = 1 + max(self.getHeight(new_root.left), self.getHeight(new_root.right))

        return new_root
    
    def rotateLeft(self, old_root):
        self.rotations += 1
        # print("Rotation count: {} ".format(self.rotations))
        # make our variable to represent the nodes to be rotated
        new_root = old_root.right
        temp = new_root.left # temporarily hold this value

        # execute rotation
        new_root.left = old_root
        old_root.right = temp

        # update heights
        old_root.height = 1 + max(self.getHeight(old_root.left), self.getHeight(old_root.right))
        new_root.height = 1 + max(self.getHeight(new_root.left), self.getHeight(new_root.right))

        return new_root

    # def preOrder(self, root): 
  
    #     if not root: 
    #         return
  
        
    #     self.preOrder(root.left) 
    #     print("{0} ".format(root.value), end="") 
    #     self.preOrder(root.right) 
    



myTree = Tree() 
root = None
complete_flag = ""

filename = 'nodes.txt'
# check if file is in the current directory
if os.path.exists(filename):
    print("File exists in the current directory.")
else:
    print("File does not exist in the current directory.")

# open file to save data into memory
with open(filename, 'r') as f:
    lines = f.readlines()

# parse the data into appropriate data
num_lines = 0
for line in lines:
    num_lines += 1
    # print("Line #{}: {}".format(num_lines, line.strip()))

    index = line.strip().find(',')
    if index == -1:
        print("comma not found at line #{}: {}".format(num_lines, line.strip()))
        exit(1)
    
    # print("Comma found at index: {}".format(index))
    value = abs(int(line.strip()[:index]))
    flag  = line.strip()[index + 1:]

    if line.strip() != "" and (value == "" or  flag == ""):
        print("error on line #{}".format(num_lines))
        exit(1)

    #input onto the avl tree
    myTree.rotations = 0
    root = myTree.insert(root, value, flag)
    if myTree.rotations >= 2:
        # complete_flag += line.strip()[:index] + ", "
        complete_flag += flag
    


print("flag{" + complete_flag + "}")

# at 10) flag{wyr}
# at 20) flag{wyr%U@,j}
# at 50) flag{wyr%U@,ja>h4hZlk&}

