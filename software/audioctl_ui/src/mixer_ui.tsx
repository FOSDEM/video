import { Component } from "preact";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState } from "./api_data.ts";
import { MixerInput, type InputActions } from "./input.tsx";
import { MixerOutput } from "./output.tsx";

type Props = {
  client: MisirkaClient;
};

type State = {
  mstate: MixerState | null;
  setup_mode: boolean;
};

export class MixerUI extends Component<Props, State> {
  state: State = { mstate: null, setup_mode: false };

  private client = new MixerClient(this.props.client);

  componentDidMount() {
    this.client.subscribe_state((mstate) => this.setState({ mstate }));
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
                <MixerInput key={channel.name} channel={channel} actions={this.mk_input_actions(i)} />
              ))}
            </div>
          </div>
          <div className="mixer">
            <h2>Outputs</h2>
            <div className="outputs channellist">
              {mstate.buses.map((bus) => (
                <MixerOutput key={bus.name} bus={bus} />
              ))}
            </div>
          </div>
        </div>
      </section>
    );
  }

  private mk_input_actions(i: number): InputActions {
    // for now we just fork the actions off, later we will handle
    // asyncness in a more civilised way
    return {
      set_phantom: (on: boolean) => {
        this.client.set_phantom(i, on);
      },
    };
  }
}
