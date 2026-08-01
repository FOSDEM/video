import { Component } from "preact";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState } from "./api_data.ts";
import { MixerInput } from "./input.tsx";

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
              {mstate.channels.map((channel) => (
                <MixerInput key={channel.name} channel={channel} />
              ))}
            </div>
          </div>
        </div>
      </section>
    );
  }
}
