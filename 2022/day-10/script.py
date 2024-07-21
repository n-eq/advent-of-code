# with open('input_test', 'r') as f:
with open('input', 'r') as f:
    data = f.readlines()
    cmds = [x.replace('\n', '').split() for x in data]

cycles = 0
x = 1
rssi = {}
for cmd in cmds:
    if cmd[0] == "addx" :
        cycles += 1
        rssi[cycles] = (x, x * cycles)
        cycles += 1
        rssi[cycles] = (x, x * cycles)
        x += int(cmd[1])
    else:
        cycles += 1
        rssi[cycles] = (x, x * cycles)

# part 1
sum = 0
for _ in [20, 60, 100, 140, 180, 220]:
    sum += rssi[_][1]
print(sum)


# part 2
crt = [[' ' for x in range(40)] for y in range(6)]
for iter in rssi:
    x = rssi[iter][0]
    if (iter - 1) % 40 in [x - 1, x, x + 1]:
        crt[(iter - 1) // 40][(iter - 1) % 40] = "o"

for l in crt:
    print("".join(l))


