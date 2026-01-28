#!/usr/bin/env python3

channels = ['IN1', 'IN2', 'IN3', 'PC', 'USB1', 'USB2']
buses = ['OUT1', 'OUT2', 'HP1', 'HP2', 'USB1', 'USB2']

def mute_mask(ch, bus):
    return 1<<((ch * len(channels)) + bus)

def main():
    routes = {ch: [] for ch in channels}

    for i in [1, 2, 3]:
        routes[f'IN{i}'] = ['OUT1', 'OUT2', 'HP1', 'HP2', 'USB1', 'USB2']

    for j in [1, 2]:
        routes[f'USB{j}'] = ['OUT1', 'OUT2', 'HP1', 'HP2', 'PC']

    mutes = (1 << (len(channels) * len(buses))) - 1 

    for i, ch in enumerate(channels):
        for j, bus in enumerate(buses):

            if bus in routes[ch]:
                
                mutes &= ~mute_mask(i, j)

    print(routes)
    print(mutes)


    for i, ch in enumerate(channels):
        for j, bus in enumerate(buses):
            if not(mutes & mute_mask(i, j)):
                print(ch, bus)

if __name__ == "__main__":
    main()
