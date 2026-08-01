import { Component } from "preact";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState } from "./api_data.ts";

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
    if (this.state.mstate) {
      return <span>We have {this.state.mstate.channels.length} channels.</span>;
    } else {
      return <span>Connected, waiting for first state</span>;
    }
  }
}
