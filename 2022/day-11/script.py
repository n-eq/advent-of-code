# set to True for part 1
part1 = False

with open("input") as f:
# with open("input_test") as f:
    lines = f.read().strip().split("\n")

# Ugly (and lazy) way to keep track of the input data in list of lists
# instead of encapsulating eveything in a dict or class
nb_monkeys = 8
starting_items = [[]] * nb_monkeys
operation = [""] * nb_monkeys # operation to do (parsed to make it easy to use eval)
test = [0] * nb_monkeys # divisibility test to perform (by number)
throw_to = [[]] * nb_monkeys # [a, b]: a recipient if True, b is False

# For part2, evaluate the GCD (greatest common denominator)
# of all the "test" numbers, this allows us to make less
# CPU-hungry number operations
gcd = 1

# parse all the lines and define the variables above
current_monkey = -1
for line in lines:
    if "Monkey" == line[0:6]:
        current_monkey = int(line.split(' ')[1][0])

    if line == "":
        continue

    # cheap regex parsing
    if "Starting items" in line:
        starting_items[current_monkey] = []
        for item in line.split(': ')[1].split(','):
            starting_items[current_monkey].append(int(item))
    elif "Operation: " in line:
        operation[current_monkey] = line.split("=")[1][1:]
    elif "Test: " in line:
        test[current_monkey] = int(line.split("by")[1][1:])
        gcd *= test[current_monkey]
    elif "If true: " in line:
        throw_to[current_monkey] = [0, 0]
        throw_to[current_monkey][0] = int(line.split("monkey")[1][1:])
    elif "If false: " in line:
        throw_to[current_monkey][1] = int(line.split("monkey")[1][1:])


round = 0
if part1:
    max_rounds = 20
else:
    max_rounds = 10000
inspected = [0] * nb_monkeys

while round < max_rounds:
    if part1 or (not part1 and round % 1000) == 0:
        print("\n\n~~~~~~New round %d" % round)

    for monkey in range(nb_monkeys):
        while starting_items[monkey] != []:
            # current item
            old = int(starting_items[monkey][0])
            # dequeu from the current monkey's starting items
            starting_items[monkey] = starting_items[monkey][1:]
            inspected[monkey] += 1

            worry_level = eval(operation[monkey])

            if part1:
                worry_level = worry_level // 3

            if worry_level % test[monkey] == 0:
                recipient = throw_to[monkey][0]
            else:
                recipient = throw_to[monkey][1]

            # change owner, recipient monkey receives a new item
            starting_items[recipient].append(worry_level % gcd)

        monkey += 1

    if not part1 and round % 1000 == 0:
        for i in range(nb_monkeys):
            print("Monkey %d inspected %d items" % (i, inspected[i]))


    done = True
    for _ in starting_items:
        if _ != []:
            done = False # some monkey still has some items
    if done:
        break

    round += 1

inspected.sort(reverse = True)
if part1:
    print("part1: %d" % (inspected[1] * inspected[0]))
else:
    print("part2: %d" % (inspected[1] * inspected[0]))
