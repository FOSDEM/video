import type { ChannelState, SendState } from "./api_data.ts";
import { useId, useState } from "preact/hooks";
import { Slider, Checkbox } from "./widgets.tsx";

type Props = {
  channel: ChannelState;
  actions: InputActions;
};

export type InputActions = {
  set_phantom: (on: boolean) => void;
};

export function MixerInput(props: Props) {
  const channel = props.channel;
  const id = useId();
  const [setup, setSetup] = useState(false);

  return (
    <div className="channel">
      <h3 title={channel.name}>{channel.label}</h3>
      <div className="controls">
        <Checkbox
          id={`${id}-setup`}
          checked={setup}
          label={"\u{1F527}"}
          onInput={setSetup}
        />
        {setup && (
          <div className="sliders gain">
            <span className="label">gain</span>
            <Slider value={channel.gain} />
          </div>
        )}
        <div className="sliders master">
          <span className="label">master</span>
          <Slider value={channel.master_fader} />
        </div>
        {setup && (
          <Checkbox
            id={`${id}-phantom`}
            checked={channel.phantom}
            onInput={props.actions.set_phantom}
            label={"\u{1F47B}"}
          />
        )}
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          checked={channel.master_unmuted}
          label="unmuted"
        />
      </div>
      {setup && (
        <div className="sends">
          {channel.sends.map((send, i) => (
            <SendMap key={i} id={`${id}-send`} send={send} i={i} />
          ))}
        </div>
      )}
    </div>
  );
}

function SendMap(props: { id: string; send: SendState; i: number }) {
  const { id, send, i } = props;
  return (
    <div className="send">
      <h4>bus {i}</h4>
      <Slider value={send.volume} />
      <Checkbox
        id={`${id}-${i}-unmuted`}
        className="mute"
        checked={send.unmuted}
        label="unmuted"
      />
      <Checkbox
        id={`${id}-${i}-pre-fader`}
        checked={send.pre_channel_fader}
        label="pre-fader"
      />
      <Checkbox
        id={`${id}-${i}-pre-mute`}
        checked={send.pre_channel_mute}
        label="pre-mute"
      />
    </div>
  );
}
