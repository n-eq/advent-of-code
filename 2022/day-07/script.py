import os

# file = open('input_test', 'r')
file = open('input', 'r')

current_dir = "/"
subdirs = {}
tree = {}

for line in file.readlines():
    line = line.strip("\n")

    if line[0] == '$':
        # we only parse cd to set the current dir and its corresponding values
        # in the tree and subdirs
        if line[0:4] == '$ cd':
            _, _, dir = line.split()

            if dir == '/':
                current_dir = os.path.normpath("/")
            else:
                current_dir = os.path.normpath(os.path.join(current_dir, dir))
            if current_dir not in tree:
                tree[current_dir] = 0
                subdirs[current_dir] = []


    else:
        sz_or_dir, name = line.split()
        if sz_or_dir == "dir":
            subdirs[current_dir].append(os.path.normpath(os.path.join(current_dir, name)))
        else:
            tree[current_dir] += int(sz_or_dir)

def dirsize(dirname):
    dsize = tree[dirname]
    for i in subdirs[dirname]:
        if i in tree:
            dsize += dirsize(i)
    return dsize

part1 = 0
for d in tree:
    dsize = dirsize(d)
    if dsize <= 100000:
        part1 += dsize
print("part1: %d " % part1)


device_space = 70000000
available_space = device_space - dirsize("/") 
total_needed = 30000000

minsize = device_space
to_delete = None
for d in tree:
    dsize = dirsize(d)

    tmp = dsize + available_space 
    if tmp >= total_needed and dsize < minsize:
        minsize = dsize
        to_delete = d

print("part2: %d" % minsize)
