import { useSignal } from "@preact/signals";
import type { MisirkaClient } from "misirka";
import { MixerClient } from "./mixerclient.ts";
import type { MixerState, Levels } from "./api_data.ts";
import { MixerInput, type InputActions, type SendActions } from "./input.tsx";
import { MixerOutput, type OutputActions } from "./output.tsx";
import { useEffect, useState } from "preact/hooks";

type Props = {
  client: MisirkaClient;
};

export function MixerUI(props: Props) {
  const client = new MixerClient(props.client);

  const [mixerState, setMixerState] = useState<MixerState | null>(null);

  const levels = useSignal<Levels | null>(null);

  useEffect(() => {
    client.subscribe_state((s) => setMixerState(s));
    client.subscribe_levels((l) => (levels.value = l));
  }, []);

  const mk_send_actions: (ch: number, bus: number) => SendActions = (
    chan,
    bus,
  ) => ({
    set_volume: (volume) => client.set_matrix_volume(chan, bus, volume),
    set_unmuted: (unmuted) => client.set_matrix_send(chan, bus, unmuted),
    set_pre_fader: (pre) => client.set_send_pre_master_fader(chan, bus, pre),
    set_pre_mute: (pre) => client.set_send_pre_master_mute(chan, bus, pre),
  });

  const mk_input_actions: (i: number) => InputActions = (i) => ({
    set_gain: (gain) => client.set_in_gain(i, gain),
    set_master_fader: (fader) => client.set_channel_master_fader(i, fader),
    set_master_unmuted: (unmuted) =>
      client.set_channel_master_unmuted(i, unmuted),
    set_phantom: (on) => client.set_phantom(i, on),
    send: (bus) => mk_send_actions(i, bus),
  });

  const mk_output_actions: (i: number) => OutputActions = (i) => ({
    set_master_fader: (fader) => client.set_bus_master_fader(i, fader),
    set_master_unmuted: (unmuted) => client.set_bus_master_unmuted(i, unmuted),
  });

  if (!mixerState) return <span>Connected, waiting for first state</span>;

  return (
    <section>
      <div className="mixer">
        <div className="mixer">
          <h2>Inputs</h2>
          <div className="inputs channellist">
            {mixerState.channels.map((channel, i) => (
              <MixerInput
                key={`chan-${i}`}
                channel={channel}
                buses={mixerState.buses}
                idx={i}
                levels={levels}
                actions={mk_input_actions(i)}
              />
            ))}
          </div>
        </div>
        <div className="mixer">
          <h2>Outputs</h2>
          <div className="outputs channellist">
            {mixerState.buses.map((bus, i) => (
              <MixerOutput
                key={`bus-${i}`}
                bus={bus}
                idx={i}
                levels={levels}
                actions={mk_output_actions(i)}
              />
            ))}
          </div>
        </div>
      </div>
    </section>
  );
}
