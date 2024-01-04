import os
import sys
import matplotlib.pyplot as plt
import scapy.all as scapy

files = sys.argv[1:]
out_folder = "results"
os.makedirs(out_folder, exist_ok=True)

t_vals = []
l_vals = []
p_names = []

for file in files:
    pckt = scapy.rdpcap(file)
    start = pckt[0].time
    end = pckt[-1].time
    total=0
    for i in pckt:
        total += len(i)
    t_bps = (8 * total) / (end - start)
    t_mbps = t_bps / 1000000
    avg_latency = ((end - start) / len(pckt)) * 1000
    t_vals.append(t_mbps)
    l_vals.append(avg_latency)
    name = os.path.splitext(os.path.basename(file))[0]
    p_names.append(name)
    print("%s - throughput: %.4f Mbps \naverage latency: %.4f milliseconds\n" % (name, t_mbps, avg_latency))
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10), sharex=True)
ax1.plot(p_names, t_vals, marker='o', linestyle='-', color='black', label='Throughput')
ax1.set_title('Throughput and Latency for PCAP Files')
ax1.set_ylabel('Throughput (Mbps)')
ax1.legend()
ax2.plot(p_names, l_vals, marker='o', linestyle='-', color='blue', label='Latency')
ax2.set_ylabel('Latency (ms)')
ax2.set_xlabel('PCAP File')
ax2.legend()
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.show()
