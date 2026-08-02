import type { ChannelState, SendState, BusState, Levels } from "./api_data.ts";
import { useId, useState } from "preact/hooks";
import type { Signal } from "@preact/signals";
import type { QUI } from "rtui";
import { VUSlider, Checkbox, VUMeter } from "./widgets.tsx";

type Props = {
  channel: ChannelState;
  buses: BusState[];
  idx: number;
  levels: Signal<Levels | null>;
  actions: InputActions;
  qui: QUI;
};

export type InputActions = {
  set_gain: (gain: number) => Promise<void>;
  set_master_fader: (fader: number) => Promise<void>;
  set_master_unmuted: (unmuted: boolean) => Promise<void>;
  set_phantom: (on: boolean) => Promise<void>;
  send: (bus: number) => SendActions;
};

export type SendActions = {
  set_volume: (volume: number) => Promise<void>;
  set_unmuted: (unmuted: boolean) => Promise<void>;
  set_pre_fader: (pre: boolean) => Promise<void>;
  set_pre_mute: (pre: boolean) => Promise<void>;
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
          value={setup}
          label={"\u{1F527}"}
          onInput={setSetup}
        />
        {setup && (
          <div className="sliders gain">
            <span className="label">gain</span>
            <VUSlider
              id={`${id}-gain`}
              value={channel.gain}
              min={-80}
              max={60}
              direction="vertical"
              onInput={actions.set_gain}
              qui={props.qui}
              reset_after={1300}
            />
          </div>
        )}
        <div className="sliders master">
          <VUSlider
            id={`${id}-master-fader`}
            value={channel.master_fader}
            min={-80}
            max={60}
            direction="vertical"
            onInput={actions.set_master_fader}
            qui={props.qui}
            reset_after={1300}
          />
          <VUMeter levels={props.levels} kind="inputs" idx={props.idx} />
        </div>
        {setup && (
          <Checkbox
            id={`${id}-phantom`}
            value={channel.phantom}
            onInput={actions.set_phantom}
            label={"\u{1F47B}"}
            qui={props.qui}
            reset_after={1300}
          />
        )}
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          value={channel.master_unmuted}
          onInput={actions.set_master_unmuted}
          label="unmuted"
          qui={props.qui}
          reset_after={1300}
        />
      </div>
      {setup && (
        <div className="sends">
          {channel.sends.map((send, i) => (
            <SendMap
              key={i}
              id={`${id}-send`}
              send={send}
              bus={props.buses[i]}
              i={i}
              actions={actions.send(i)}
              qui={props.qui}
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
  bus: BusState;
  i: number;
  actions: SendActions;
  qui: QUI;
}) {
  const { id, send, bus, i, actions, qui } = props;
  return (
    <div className="send">
      <h4 title={bus.name}>{bus.label}</h4>
      <VUSlider
        id={`${id}-${i}-volume`}
        value={send.volume}
        min={-80}
        max={60}
        direction="vertical"
        onInput={actions.set_volume}
        qui={qui}
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-unmuted`}
        className="mute"
        value={send.unmuted}
        onInput={actions.set_unmuted}
        label="unmuted"
        qui={qui}
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-pre-fader`}
        value={send.pre_channel_fader}
        onInput={actions.set_pre_fader}
        label="pre-fader"
        qui={qui}
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-pre-mute`}
        value={send.pre_channel_mute}
        onInput={actions.set_pre_mute}
        label="pre-mute"
        qui={qui}
        reset_after={1300}
      />
    </div>
  );
}
