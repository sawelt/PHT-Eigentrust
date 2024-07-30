import sys
import json
import typing
import matplotlib.pyplot as plt
if len(sys.argv) < 3:
    print("usage: visualiser [command] [path to simulation report file]")
    exit(1)

command = sys.argv[1]
file_path = sys.argv[2]

def get_malicious_stations(json_data) -> typing.Set[int]:
    s = set()
    for peer in json_data["peers"]:
        if peer["malicious"] and peer["role"] == "station":
            s.add(peer["id"])
    return s

def get_benign_stations(json_data) -> typing.Set[int]:
    s = set()
    for peer in json_data["peers"]:
        if not peer["malicious"] and peer["role"] == "station":
            s.add(peer["id"])
    return s

def write_to_file(benign, malicious):
    with open("results.csv","w") as f:
        f.write(f"i,b,m\n")
        for i in range(0,len(benign)):
            f.write(f"{i},{benign[i]},{malicious[i]}\n")

def produce_earnings_graph(json_data):
    c_l_tokens_b = []
    c_l_tokens_m = []
    for sim in json_data["simulations"]:
        m = get_malicious_stations(sim)
        b = get_benign_stations(sim)
        l_tokens_b = []
        l_tokens_m = []
        for cycle in sim["cycles"]:
            tokens_b = 0
            tokens_m = 0
            for f in cycle["post_funds"]:
                if int(f) in m:
                    tokens_m += cycle["post_funds"][f]
                if int(f) in b:
                    tokens_b += cycle["post_funds"][f]
            l_tokens_b.append(tokens_b / len(b))
            l_tokens_m.append(tokens_m / len(m))
        c_l_tokens_b.append(l_tokens_b)
        c_l_tokens_m.append(l_tokens_m)
    a_l_tokens_b = []
    a_l_tokens_m = []
    for i in range(0,len(c_l_tokens_m[0])):
        sum_b = 0
        sum_m = 0
        for j in range(0,len(c_l_tokens_m)):
            sum_b += c_l_tokens_b[j][i]
            sum_m += c_l_tokens_m[j][i]
        a_l_tokens_b.append(sum_b/len(c_l_tokens_m))
        a_l_tokens_m.append(sum_m/len(c_l_tokens_m))
    fig, ax = plt.subplots()
    ax.plot([0] + a_l_tokens_b, label="benign")
    ax.plot([0] + a_l_tokens_m, label="malicious")
    ax.set(xticks=range(0, len(a_l_tokens_m)+1))
    plt.legend(loc="upper left")
    plt.show()
    write_to_file([0] + a_l_tokens_b, [0] + a_l_tokens_m)
    print("malicious:" + str(l_tokens_m))
    print("benign:" + str(l_tokens_b))

def produce_reputation_list(json_data):
    c_l_reputation_b = []
    c_l_reputation_m = []
    for sim in json_data["simulations"]:
        m = get_malicious_stations(sim)
        b = get_benign_stations(sim)
        last_cycle = sim["cycles"][-1]
        for key in last_cycle["post_global_trust_values"]:
            if int(key) in m:
                c_l_reputation_m.append(last_cycle["post_global_trust_values"][key])
            if int(key) in b:
                c_l_reputation_b.append(last_cycle["post_global_trust_values"][key])
    with open("final_reputation_values_b.csv") as f:
        f.print(f"rep\n")
        for r in c_l_reputation_b:
            f.print(f"{r}")
    with open("final_reputation_values_m.csv") as f:
        f.print(f"rep\n")
        for r in c_l_reputation_m:
            f.print(f"{r}")

with open(file_path) as f:
    json_data = json.load(f)

if command == "earnings":
    produce_earnings_graph(json_data)
if command == "reputation":
    produce_reputation_list(json_data)