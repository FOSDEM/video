import { Component } from "preact";
import { signal } from "@preact/signals";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState, Levels } from "./api_data.ts";
import { MixerInput, type InputActions } from "./input.tsx";
import { MixerOutput, type OutputActions } from "./output.tsx";
import { QUI } from "rtui";

type Props = {
  client: MisirkaClient;
  qui: QUI;
};

type State = {
  mstate: MixerState | null;
  setup_mode: boolean;
};

export class MixerUI extends Component<Props, State> {
  state: State = { mstate: null, setup_mode: false };

  private client = new MixerClient(this.props.client);
  private levels = signal<Levels | null>(null);

  componentDidMount() {
    this.client.subscribe_state((mstate) => this.setState({ mstate }));
    this.client.subscribe_levels((levels) => this.set_levels(levels));
  }

  render() {
    const mstate = this.state.mstate;
    if (!mstate) {
      return <span>Connected, waiting for first state</span>;
    }

    return (
      <section>
        <div className="mixer">
          <div className="mixer">
            <h2>Inputs</h2>
            <div className="inputs channellist">
              {mstate.channels.map((channel, i) => (
                <MixerInput
                  key={channel.name}
                  channel={channel}
                  idx={i}
                  levels={this.levels}
                  actions={this.mk_input_actions(i)}
                />
              ))}
            </div>
          </div>
          <div className="mixer">
            <h2>Outputs</h2>
            <div className="outputs channellist">
              {mstate.buses.map((bus, i) => (
                <MixerOutput
                  key={bus.name}
                  bus={bus}
                  idx={i}
                  levels={this.levels}
                  actions={this.mk_output_actions(i)}
                />
              ))}
            </div>
          </div>
        </div>
      </section>
    );
  }

  private mk_input_actions(i: number): InputActions {
    // TODO: use qui for all actions
    return {
      set_gain: (gain: number) => {
        this.props.qui.add(async () => {
          await this.client.set_in_gain(i, gain);
        }, `inp-${i}-gain`);
      },
      set_master_fader: (fader: number) => {
        this.client.set_channel_master_fader(i, fader);
      },
      set_master_unmuted: (unmuted: boolean) => {
        this.client.set_channel_master_unmuted(i, unmuted);
      },
      set_phantom: (on: boolean) => {
        this.client.set_phantom(i, on);
      },
      send: (bus: number) => ({
        set_volume: (volume: number) => {
          this.client.set_matrix_volume(i, bus, volume);
        },
        set_unmuted: (unmuted: boolean) => {
          this.client.set_matrix_send(i, bus, unmuted);
        },
        set_pre_fader: (pre: boolean) => {
          this.client.set_send_pre_master_fader(i, bus, pre);
        },
        set_pre_mute: (pre: boolean) => {
          this.client.set_send_pre_master_mute(i, bus, pre);
        },
      }),
    };
  }

  private mk_output_actions(i: number): OutputActions {
    return {
      set_master_fader: (fader: number) => {
        this.client.set_bus_master_fader(i, fader);
      },
      set_master_unmuted: (unmuted: boolean) => {
        this.client.set_bus_master_unmuted(i, unmuted);
      },
    };
  }

  private set_levels(levels: Levels) {
    this.levels.value = levels;
  }
}
