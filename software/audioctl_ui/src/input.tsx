import type { ChannelState, SendState, BusState, Levels } from "./api_data.ts";
import { useId } from "preact/hooks";
import { type Signal, useSignal } from "@preact/signals";
import { VUSlider, Checkbox, VUMeter } from "./widgets.tsx";

type Props = {
  channel: ChannelState;
  buses: BusState[];
  idx: number;
  levels: Signal<Levels | null>;
  actions: InputActions;
};

export interface InputActions {
  set_gain: (gain: number) => Promise<void>;
  set_master_fader: (fader: number) => Promise<void>;
  set_master_unmuted: (unmuted: boolean) => Promise<void>;
  set_phantom: (on: boolean) => Promise<void>;
  send: (bus: number) => SendActions;
}

export interface SendActions {
  set_volume: (volume: number) => Promise<void>;
  set_unmuted: (unmuted: boolean) => Promise<void>;
  set_pre_fader: (pre: boolean) => Promise<void>;
  set_pre_mute: (pre: boolean) => Promise<void>;
}

export function MixerInput(props: Props) {
  const channel = props.channel;
  const actions = props.actions;
  const id = useId();
  const setup = useSignal(false);

  return (
    <div className="channel">
      <h3 title={channel.name}>{channel.label}</h3>
      <div className="controls">
        <Checkbox
          id={`${id}-setup`}
          checked={setup.value}
          label={"\u{1F527}"}
          on_new_user_val={(val) => (setup.value = val)}
        />
        {setup.value && (
          <div className="sliders gain">
            <span className="label">gain</span>
            <VUSlider
              id={`${id}-gain`}
              value={channel.gain}
              min={-80}
              max={60}
              direction="vertical"
              on_new_user_val={actions.set_gain}
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
            on_new_user_val={actions.set_master_fader}
            reset_after={1300}
          />
          <VUMeter levels={props.levels} kind="inputs" idx={props.idx} />
        </div>
        {setup.value && (
          <Checkbox
            id={`${id}-phantom`}
            checked={channel.phantom}
            on_new_user_val={actions.set_phantom}
            label={"\u{1F47B}"}
            reset_after={1300}
          />
        )}
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          checked={channel.master_unmuted}
          on_new_user_val={actions.set_master_unmuted}
          label="unmuted"
          reset_after={1300}
        />
      </div>
      {setup.value && (
        <div className="sends">
          {channel.sends.map((send, i) => (
            <SendMap
              key={i}
              id={`${id}-send`}
              send={send}
              bus={props.buses[i]}
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
  bus: BusState;
  i: number;
  actions: SendActions;
}) {
  const { id, send, bus, i, actions } = props;
  return (
    <div className="send">
      <h4 title={bus.name}>{bus.label}</h4>
      <VUSlider
        id={`${id}-${i}-volume`}
        value={send.volume}
        min={-80}
        max={60}
        direction="vertical"
        on_new_user_val={actions.set_volume}
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-unmuted`}
        className="mute"
        checked={send.unmuted}
        on_new_user_val={actions.set_unmuted}
        label="unmuted"
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-pre-fader`}
        checked={send.pre_channel_fader}
        on_new_user_val={actions.set_pre_fader}
        label="pre-fader"
        reset_after={1300}
      />
      <Checkbox
        id={`${id}-${i}-pre-mute`}
        checked={send.pre_channel_mute}
        on_new_user_val={actions.set_pre_mute}
        label="pre-mute"
        reset_after={1300}
      />
    </div>
  );
}
