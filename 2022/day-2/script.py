# 0: rock, 1: paper, 2: scissors
from csv import reader
l = []
with open('input.txt', 'r') as read_obj:
    csv_reader = reader(read_obj, delimiter=' ')
    for row in csv_reader:
        l.append((row[0], row[1]))

def normalize(p1, p2):
# A: rock, B: paper, C: scissors
# X: rock, Y: paper, Z: scissors
# A -> 0, B -> 1, C -> 2
# X -> 0, Y -> 1, Z -> 2
    return (ord(p1) % 65, (ord(p2) - 23) % 65)

# draw: 3, win: 6, lose: 0
def game_score(p1, p2):
    def raw_score(p1, p2):
        def win(p1, p2):
            return (p2 == 0 and p1 == 2) or (p2 == 1 and p1 == 0) or (p2 == 2 and p1 == 1)

        if p1 == p2:
            return 3
        elif win(p1, p2):
            return 6
        else:
            return 0

    return raw_score(p1, p2) + (p2 + 1)

def game_score_v2(p1, p2):
    def transform(p2):
        new_p2 = 0
        if p2 == 0:
            # we need to lose
            if p1 == 0:
                new_p2 = 2
            elif p1 == 1:
                new_p2 = 0
            else:
                new_p2 = 1
            
        elif p2 == 1:
            # we need to draw
            new_p2 = p1

        elif p2 == 2:
            # we need to win
            if p1 == 0:
                new_p2 = 1
            elif p1 == 1:
                new_p2 = 2
            else:
                new_p2 = 0
        return new_p2

    # print("game score: " + str((p1, p2)) + " " + str((p1, new_p2)))
    return game_score(p1, transform(p2))




l = [normalize(p1, p2) for (p1, p2) in l]
score = 0
for (p1, p2) in l:
    score  += game_score_v2(p1, p2)

print(score)
