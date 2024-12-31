links = []
nodes = set()

def get_edges(node):
    res = []
    for e1, e2 in links:
        if e1 == node:
            res.append(e2)
        if e2 == node:
            res.append(e1)
        
    return res

with open("input", "r") as f:
    for l in f.readlines():
        l = l.strip()
        v1, v2 = (v for v in l.split('-'))
        links.append((v1, v2))
        nodes.add(v1)
        nodes.add(v2)

    nodes = list(nodes)

    names = {}
    graph = {}
    for i, n in enumerate(nodes):
        if n == "start":
            start = i
        if n == "end":
            end = i

        edges = []
        ll = get_edges(n)
        for l in ll:
            edges.append(nodes.index(l))
        graph.update({ i : [nodes.index(link) for link in get_edges(n)] })
        names.update({ i : n })


    all_paths = []
    def dfs(current, destination, max_small_cave_visit=1, path=[]):
        path.append(current)

        if current == destination:
            all_paths.append(list(path))

        for neighbor in graph.get(current):
            name = names.get(neighbor)
            if name.islower():
                occurrences = path.count(neighbor)
                if occurrences > 0:
                    if (name == "start" or name == "end") or max_small_cave_visit == 1:
                        continue
                    else:
                        # if any other small cave was visited more than once, we don't push this cave
                        forbidden = False
                        for cave in path:
                            if names.get(cave).islower() and path.count(cave) == max_small_cave_visit:
                                forbidden = True
                                break
                        if forbidden:
                            continue
            dfs(neighbor, destination, max_small_cave_visit, path)
        path.pop()

    dfs(start, end)
    print("part1: ", len(all_paths))

    all_paths = []
    dfs(start, end, 2)
    print("part2: ", len(all_paths))
