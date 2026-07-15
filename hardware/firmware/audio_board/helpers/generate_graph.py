import math

import networkx as nx


class Node:
    def __init__(self, type, name):
        self.type = type
        self.name = name
        self.x = None
        self.y = None


inputs = [
    (Node("AudioInputTDM2", "tdm1"), 0, 1, 2, 3),
    (Node("AudioInputUSB", "usb1"), 0, 1),
    (Node("AsyncAudioInputSPDIF3", "spdif1"), 0, 1),
    (Node("AudioSynthNoisePink", "pink1"), 0),
    (Node("AudioSynthWaveformSine", "sine1"), 0),
]

outputs = [
    (Node("AudioOutputI2SQuad", "i2s_quad1"), 0, 1, 2, 3),
    (Node("AudioOutputUSB", "usb2"), 0, 1),
    (Node("AudioOutputSPDIF3", "spdif2"), 0, 1),
]

print("// GUItool: begin automatically generated code")


class Graph:
    def __init__(self):
        self.nodes = []
        self.nodes_sorted = []
        self.edges = []

    def add(self, node: Node):
        self.nodes.append(node)

    def edge(self, snode: str, sport: int, dnode: str, dport: int):
        self.edges.append((snode, sport, dnode, dport))

    def __str__(self):
        result = ""

        nlist = self.nodes
        if self.nodes_sorted:
            nlist = self.nodes_sorted

        for node in nlist:
            result += f"{node.type}  {node.name};"
            if node.x is not None and node.y is not None:
                result += f'    //xy={int(node.x)},{int(node.y)}'
            result += '\n'
        for i, edge in enumerate(self.edges):
            result += f"AudioConnection patchCord{i}({edge[0]}, {edge[1]}, {edge[2]}, {edge[3]});\n"
        return result

    def find_node(self, name):
        for n in self.nodes:
            if n.name == name:
                return n
        return None

    def render(self):
        G = nx.DiGraph()
        for node in self.nodes:
            G.add_node(node.name, type=node.type)
        for edge in self.edges:
            G.add_edge(edge[0], edge[2], source_port=edge[1], dest_port=edge[3])

        pos = nx.drawing.nx_agraph.graphviz_layout(G, prog='dot', args='-Grankdir=LR')
        min_y = 999999999
        max_y = 0
        for node in pos:
            min_y = min(min_y, pos[node][1])
            max_y = max(max_y, pos[node][1])

        for node in pos:
            n = self.find_node(node)
            n.x = pos[node][0] * 2
            y = max_y - pos[node][1]
            n.y = y * 1.05

        self.nodes_sorted = []
        ts = list(nx.topological_sort(G))
        for n in ts:
            self.nodes_sorted.append(self.find_node(n))

    def print_matrix(self):
        pass


g = Graph()

index = 1
cmap = []
for i in inputs:
    g.add(i[0])
    for channel in i[1:]:
        if 'Synth' not in i[0].type:
            g.add(Node("AudioAnalyzePeak", f"peak{index}"))
            g.edge(i[0].name, channel, f"peak{index}", 0)
            g.add(Node("AudioAnalyzeRMS", f"rms{index}"))
            g.edge(i[0].name, channel, f"rms{index}", 0)
            index += 1
        cmap.append((i[0].name, channel))

bmap = []
for o in outputs:
    g.add(o[0])
    for channel in o[1:]:
        g.add(Node("AudioAnalyzePeak", f"peak{index}"))
        g.add(Node("AudioAnalyzeRMS", f"rms{index}"))

        bmap.append((
            (f"peak{index}", 0),
            (f"rms{index}", 0),
            (o[0].name, channel),
        ))

        index += 1

index = 1
matrix = []
for bus in bmap:
    medge = []
    for group in range(math.ceil(float(len(cmap)) / 4)):
        g.add(Node("AudioMixer4", f"mixer{index}"))
        for gc in range(4):
            cnum = group * 4 + gc
            if cnum < len(cmap):
                g.edge(*cmap[cnum], f"mixer{index}", gc)
        medge.append((f"mixer{index}", 0))
        index += 1

    g.add(Node("AudioMixer4", f"mixer{index}"))
    mline = []
    for i, m in enumerate(medge):
        g.edge(*m, f"mixer{index}", i)
        mline.append(m[0])
    matrix.append(mline)
    for dest in bus:
        g.edge(f"mixer{index}", 0, *dest)
    index += 1

g.render()

print(g, end='')
print("// GUItool: end automatically generated code")
print("\n\n")

print(f"AudioMixer4* matrix[{len(matrix)}][{len(matrix[0])}] = " + '{')
for row in matrix:
    print("\t{&" + ', &'.join(row) + '},')
print('};\n')

num_rms = len(cmap) + len(bmap) - 2
print(f"AudioAnalyzeRMS* ent_rms[{num_rms}] = " + '{')
for i in range(num_rms):
    print(f"\t&rms{i + 1},")
print('};\n')
print(f"AudioAnalyzePeak* ent_peak[{num_rms}] = " + '{')
for i in range(num_rms):
    print(f"\t&peak{i + 1},")
print('};')
