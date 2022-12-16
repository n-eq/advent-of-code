def compare(left, right):
    if isinstance(left, int) and isinstance(right, int):
        if left < right:
            return 1
        elif left > right:
            return -1
        return 0

    if type(left) == int:
        left = [left]
    if type(right) == int:
        right = [right]

    for i in range(min(len(left), len(right))):
        res = compare(left[i], right[i])
        if res != 0:
            return res

    if len(left) < len(right):
        return 1
    if len(right) < len(left):
        return -1
    return 0

with open("input") as f:
# with open("input_test") as f:
    lines = f.read().strip().split("\n\n")

part1 = 0

# part 2
intruders = [[[2]], [[6]]]

all_packets = list(intruders)
i = 1
for group in lines:
    left = eval(group.split("\n")[0])
    right = eval(group.split("\n")[1])

    all_packets.append(left)
    all_packets.append(right)

    if compare(left, right) == 1:
        part1 += i
    i += 1

print("part1: %d" % part1)


import functools
res = sorted(all_packets, key=functools.cmp_to_key(compare))[::-1]
print("part2: %d" % ((1 + res.index(intruders[0])) * (1 + res.index(intruders[1]))))
