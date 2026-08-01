import type { ChannelState, SendState, Levels } from "./api_data.ts";
import { useId, useState } from "preact/hooks";
import type { Signal } from "@preact/signals";
import { Slider, Checkbox, VUMeter } from "./widgets.tsx";

type Props = {
  channel: ChannelState;
  idx: number;
  levels: Signal<Levels | null>;
  actions: InputActions;
};

export type InputActions = {
  set_gain: (gain: number) => void;
  set_master_fader: (fader: number) => void;
  set_master_unmuted: (unmuted: boolean) => void;
  set_phantom: (on: boolean) => void;
  send: (bus: number) => SendActions;
};

export type SendActions = {
  set_volume: (volume: number) => void;
  set_unmuted: (unmuted: boolean) => void;
  set_pre_fader: (pre: boolean) => void;
  set_pre_mute: (pre: boolean) => void;
};

export function MixerInput(props: Props) {
  const channel = props.channel;
  const actions = props.actions;
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
            <Slider value={channel.gain} onInput={actions.set_gain} />
          </div>
        )}
        <div className="sliders master">
          <span className="label">master</span>
          <Slider
            value={channel.master_fader}
            onInput={actions.set_master_fader}
          />
        </div>
        <VUMeter levels={props.levels} kind="inputs" idx={props.idx} />
        {setup && (
          <Checkbox
            id={`${id}-phantom`}
            checked={channel.phantom}
            onInput={actions.set_phantom}
            label={"\u{1F47B}"}
          />
        )}
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          checked={channel.master_unmuted}
          onInput={actions.set_master_unmuted}
          label="unmuted"
        />
      </div>
      {setup && (
        <div className="sends">
          {channel.sends.map((send, i) => (
            <SendMap
              key={i}
              id={`${id}-send`}
              send={send}
              i={i}
              actions={actions.send(i)}
            />
          ))}
        </div>
      )}
    </div>
  );
}

function SendMap(props: {
  id: string;
  send: SendState;
  i: number;
  actions: SendActions;
}) {
  const { id, send, i, actions } = props;
  return (
    <div className="send">
      <h4>bus {i}</h4>
      <Slider value={send.volume} onInput={actions.set_volume} />
      <Checkbox
        id={`${id}-${i}-unmuted`}
        className="mute"
        checked={send.unmuted}
        onInput={actions.set_unmuted}
        label="unmuted"
      />
      <Checkbox
        id={`${id}-${i}-pre-fader`}
        checked={send.pre_channel_fader}
        onInput={actions.set_pre_fader}
        label="pre-fader"
      />
      <Checkbox
        id={`${id}-${i}-pre-mute`}
        checked={send.pre_channel_mute}
        onInput={actions.set_pre_mute}
        label="pre-mute"
      />
    </div>
  );
}
