import { Component } from "preact";
import { signal } from "@preact/signals";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState, Levels } from "./api_data.ts";
import { MixerInput, type InputActions, type SendActions } from "./input.tsx";
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
                  buses={mstate.buses}
                  idx={i}
                  levels={this.levels}
                  actions={this.mk_input_actions(i)}
                  qui={this.props.qui}
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
                  qui={this.props.qui}
                />
              ))}
            </div>
          </div>
        </div>
      </section>
    );
  }

  private mk_input_actions(i: number): InputActions {
    return {
      set_gain: (gain) => this.client.set_in_gain(i, gain),
      set_master_fader: (fader) =>
        this.client.set_channel_master_fader(i, fader),
      set_master_unmuted: (unmuted) =>
        this.client.set_channel_master_unmuted(i, unmuted),
      set_phantom: (on) => this.client.set_phantom(i, on),
      send: (bus) => this.mk_send_actions(i, bus),
    };
  }

  private mk_send_actions(chan: number, bus: number): SendActions {
    return {
      set_volume: (volume) => this.client.set_matrix_volume(chan, bus, volume),
      set_unmuted: (unmuted) => this.client.set_matrix_send(chan, bus, unmuted),
      set_pre_fader: (pre) =>
        this.client.set_send_pre_master_fader(chan, bus, pre),
      set_pre_mute: (pre) =>
        this.client.set_send_pre_master_mute(chan, bus, pre),
    };
  }

  private mk_output_actions(i: number): OutputActions {
    return {
      set_master_fader: (fader) => this.client.set_bus_master_fader(i, fader),
      set_master_unmuted: (unmuted) =>
        this.client.set_bus_master_unmuted(i, unmuted),
    };
  }

  private set_levels(levels: Levels) {
    this.levels.value = levels;
  }
}
